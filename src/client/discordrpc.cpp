// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later
// MineBoostV2 Discord Rich Presence

#include "discordrpc.h"
#include "convert_json.h"
#include "porting.h"
#include "settings.h"
#include "log.h"

#include <json/json.h>
#include <cstring>
#include <ctime>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace
{
constexpr uint32_t OP_HANDSHAKE = 0;
constexpr uint32_t OP_FRAME = 1;
constexpr uint32_t RECONNECT_INTERVAL_MS = 2000;

long getCurrentPid()
{
#ifdef _WIN32
	return (long)GetCurrentProcessId();
#else
	return (long)getpid();
#endif
}
}

DiscordRPC &DiscordRPC::get()
{
	static DiscordRPC instance;
	return instance;
}

DiscordRPC::~DiscordRPC()
{
	shutdown();
}

void DiscordRPC::init(const std::string &client_id)
{
	m_client_id = client_id;
	m_configured = !client_id.empty();
	m_enabled = m_configured && g_settings->getBool("discord_rpc_enabled");
	m_next_reconnect_attempt_ms = 0; // try immediately
	if (m_enabled)
		connectPipe();
}

void DiscordRPC::shutdown()
{
	if (m_connected)
		clearActivity();
	closePipe();
	m_enabled = false;
	m_configured = false;
}

void DiscordRPC::poll()
{
	if (!m_configured)
		return;

	// React live to the "discord_rpc_enabled" checkbox, whether it's
	// flipped from the settings menu, the in-game GUI toggle, or the
	// pause-menu checkbox.
	bool wants_enabled = g_settings->getBool("discord_rpc_enabled");
	if (wants_enabled != m_enabled) {
		if (!wants_enabled) {
			// Send the "clear" frame first, while still marked enabled
			// and connected, otherwise clearActivity() no-ops.
			clearActivity();
			closePipe();
		}
		m_enabled = wants_enabled;
		if (m_enabled)
			m_next_reconnect_attempt_ms = 0; // try immediately
	}

	if (!m_enabled)
		return;

	if (!m_connected) {
		uint64_t now = porting::getTimeMs();
		if (now < m_next_reconnect_attempt_ms)
			return;
		m_next_reconnect_attempt_ms = now + RECONNECT_INTERVAL_MS;
		if (connectPipe())
			resendCachedActivity();
		return;
	}

	drainIncoming();
}

#ifdef _WIN32

bool DiscordRPC::connectPipe()
{
	closePipe();

	for (int i = 0; i < 10; i++) {
		std::string name = "\\\\.\\pipe\\discord-ipc-" + std::to_string(i);
		HANDLE h = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE,
				0, nullptr, OPEN_EXISTING, 0, nullptr);
		if (h != INVALID_HANDLE_VALUE) {
			m_pipe = h;
			if (sendHandshake()) {
				m_connected = true;
				verbosestream << "DiscordRPC: connected via " << name << std::endl;
				return true;
			}
			closePipe();
		}
	}
	return false;
}

void DiscordRPC::closePipe()
{
	if (m_pipe) {
		CloseHandle((HANDLE)m_pipe);
		m_pipe = nullptr;
	}
	m_connected = false;
}

bool DiscordRPC::sendFrame(uint32_t opcode, const std::string &payload)
{
	if (!m_pipe)
		return false;

	uint32_t length = (uint32_t)payload.size();
	std::string buf;
	buf.reserve(8 + payload.size());
	buf.append(reinterpret_cast<const char *>(&opcode), 4);
	buf.append(reinterpret_cast<const char *>(&length), 4);
	buf.append(payload);

	DWORD written = 0;
	BOOL ok = WriteFile((HANDLE)m_pipe, buf.data(), (DWORD)buf.size(),
			&written, nullptr);
	if (!ok || written != buf.size()) {
		m_connected = false;
		return false;
	}
	return true;
}

void DiscordRPC::drainIncoming()
{
	if (!m_pipe)
		return;

	char buf[8192];
	DWORD avail = 0;
	if (!PeekNamedPipe((HANDLE)m_pipe, nullptr, 0, nullptr, &avail, nullptr)) {
		// Pipe likely closed on Discord's end.
		m_connected = false;
		closePipe();
		return;
	}
	if (avail == 0)
		return;

	DWORD read = 0;
	if (!ReadFile((HANDLE)m_pipe, buf, sizeof(buf), &read, nullptr)) {
		m_connected = false;
		closePipe();
	}
	// Contents are intentionally discarded; we don't need Discord's
	// acknowledgements for this to work, we just need to keep draining
	// so the pipe doesn't back up.
}

#else // POSIX

namespace
{
std::string getIpcDir()
{
	static const char *vars[] = { "XDG_RUNTIME_DIR", "TMPDIR", "TMP", "TEMP" };
	for (const char *var : vars) {
		const char *val = std::getenv(var);
		if (val && *val)
			return std::string(val);
	}
	return "/tmp";
}
}

bool DiscordRPC::connectPipe()
{
	closePipe();

	std::string dir = getIpcDir();

	for (int i = 0; i < 10; i++) {
		int fd = socket(AF_UNIX, SOCK_STREAM, 0);
		if (fd < 0)
			continue;

		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		std::string path = dir + "/discord-ipc-" + std::to_string(i);
		strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
			m_socket = fd;
			if (sendHandshake()) {
				int flags = fcntl(fd, F_GETFL, 0);
				fcntl(fd, F_SETFL, flags | O_NONBLOCK);
				m_connected = true;
				verbosestream << "DiscordRPC: connected via " << path << std::endl;
				return true;
			}
			closePipe();
		} else {
			close(fd);
		}
	}
	return false;
}

void DiscordRPC::closePipe()
{
	if (m_socket >= 0) {
		close(m_socket);
		m_socket = -1;
	}
	m_connected = false;
}

bool DiscordRPC::sendFrame(uint32_t opcode, const std::string &payload)
{
	if (m_socket < 0)
		return false;

	uint32_t length = (uint32_t)payload.size();
	std::string buf;
	buf.reserve(8 + payload.size());
	buf.append(reinterpret_cast<const char *>(&opcode), 4);
	buf.append(reinterpret_cast<const char *>(&length), 4);
	buf.append(payload);

	ssize_t written = send(m_socket, buf.data(), buf.size(), 0);
	if (written != (ssize_t)buf.size()) {
		m_connected = false;
		return false;
	}
	return true;
}

void DiscordRPC::drainIncoming()
{
	if (m_socket < 0)
		return;

	char buf[8192];
	ssize_t n = recv(m_socket, buf, sizeof(buf), MSG_DONTWAIT);
	if (n == 0) {
		// Peer closed the connection.
		m_connected = false;
		closePipe();
	} else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
		m_connected = false;
		closePipe();
	}
	// Contents are intentionally discarded, see Windows comment above.
}

#endif

bool DiscordRPC::sendHandshake()
{
	Json::Value root;
	root["v"] = 1;
	root["client_id"] = m_client_id;
	return sendFrame(OP_HANDSHAKE, fastWriteJson(root));
}

void DiscordRPC::setActivity(const std::string &details, const std::string &state,
		const std::string &large_image, const std::string &large_text,
		const std::string &small_image, const std::string &small_text,
		bool reset_timestamp)
{
	// Cache regardless of current connection state, so this can be
	// re-sent automatically the moment we (re)connect -- e.g. right
	// after the user flips "discord_rpc_enabled" back on mid-session.
	m_has_activity = true;
	m_last_details = details;
	m_last_state = state;
	m_last_large_image = large_image;
	m_last_large_text = large_text;
	m_last_small_image = small_image;
	m_last_small_text = small_text;

	if (reset_timestamp || m_start_timestamp == 0)
		// Must be a real Unix epoch timestamp (Discord computes elapsed
		// time as "now - start" using wall-clock time on its end).
		// porting::getTimeS() is monotonic/boot-relative on some
		// platforms, which produced a nonsensical multi-decade elapsed
		// counter, so std::time() is used here instead.
		m_start_timestamp = (int64_t)std::time(nullptr);

	if (!m_enabled || !m_connected)
		return;

	sendActivityFrame();
}

void DiscordRPC::resendCachedActivity()
{
	if (!m_has_activity || !m_enabled || !m_connected)
		return;
	sendActivityFrame();
}

void DiscordRPC::sendActivityFrame()
{
	Json::Value activity;
	if (!m_last_details.empty())
		activity["details"] = m_last_details;
	if (!m_last_state.empty())
		activity["state"] = m_last_state;

	Json::Value timestamps;
	timestamps["start"] = (Json::Int64)m_start_timestamp;
	activity["timestamps"] = timestamps;

	Json::Value assets;
	if (!m_last_large_image.empty()) {
		assets["large_image"] = m_last_large_image;
		if (!m_last_large_text.empty())
			assets["large_text"] = m_last_large_text;
	}
	if (!m_last_small_image.empty()) {
		assets["small_image"] = m_last_small_image;
		if (!m_last_small_text.empty())
			assets["small_text"] = m_last_small_text;
	}
	if (!assets.empty())
		activity["assets"] = assets;

	Json::Value args;
	args["pid"] = (Json::Int)getCurrentPid();
	args["activity"] = activity;

	Json::Value root;
	root["cmd"] = "SET_ACTIVITY";
	root["args"] = args;
	root["nonce"] = std::to_string(porting::getTimeMs());

	if (!sendFrame(OP_FRAME, fastWriteJson(root)))
		closePipe();
}

void DiscordRPC::clearActivity()
{
	if (!m_enabled || !m_connected)
		return;

	Json::Value args;
	args["pid"] = (Json::Int)getCurrentPid();
	args["activity"] = Json::Value(Json::nullValue);

	Json::Value root;
	root["cmd"] = "SET_ACTIVITY";
	root["args"] = args;
	root["nonce"] = std::to_string(porting::getTimeMs());

	sendFrame(OP_FRAME, fastWriteJson(root));
}
