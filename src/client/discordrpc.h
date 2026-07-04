// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later
// MineBoostV2 Discord Rich Presence

#pragma once

#include <cstdint>
#include <string>

// Minimal Discord Rich Presence client.
//
// This talks directly to the local Discord desktop client over its IPC
// pipe/socket using the same lightweight JSON-over-framed-messages
// protocol as Discord's own (now legacy, but still fully supported)
// "discord-rpc" library. No external SDK or extra dependency is needed;
// it only uses jsoncpp (already part of this project) plus plain OS
// sockets/pipes.
//
// IMPORTANT: large_image/small_image below are *asset key names*, not
// file paths. To use your own pictures you first have to:
//   1. Create an application at https://discord.com/developers/applications
//   2. Copy its numeric "Application ID" (this is the client_id below)
//   3. Go to "Rich Presence" -> "Art Assets" and upload your images there,
//      giving each one a short key name (e.g. "mineboostv2_logo")
//   4. Use that key name (not the file) as large_image/small_image here.
// Discord does not accept raw image bytes over the RPC protocol.
class DiscordRPC
{
public:
	static DiscordRPC &get();

	// Connects (or schedules a retry) to the local Discord client.
	// client_id is the numeric Application ID from the developer portal.
	void init(const std::string &client_id);

	// Closes the connection to Discord and clears the shown activity.
	void shutdown();

	// Updates what's shown on the user's Discord profile / friend list.
	// Pass reset_timestamp = true to restart the "elapsed time" counter
	// (e.g. when a new game session starts).
	void setActivity(const std::string &details, const std::string &state,
			const std::string &large_image, const std::string &large_text,
			const std::string &small_image = "",
			const std::string &small_text = "",
			bool reset_timestamp = false);

	// Removes the activity without disconnecting.
	void clearActivity();

	// Call periodically (once every second or so is plenty) from the
	// main loop. Retries the connection if Discord wasn't running yet,
	// and drains incoming IPC traffic so the pipe doesn't back up.
	void poll();

private:
	DiscordRPC() = default;
	~DiscordRPC();

	DiscordRPC(const DiscordRPC &) = delete;
	DiscordRPC &operator=(const DiscordRPC &) = delete;

	bool connectPipe();
	void closePipe();
	bool sendFrame(uint32_t opcode, const std::string &payload);
	void drainIncoming();
	bool sendHandshake();
	void resendCachedActivity();
	void sendActivityFrame();

	bool m_connected = false;
	bool m_enabled = false;
	bool m_configured = false;
	std::string m_client_id;
	int64_t m_start_timestamp = 0;
	uint64_t m_next_reconnect_attempt_ms = 0;

	// Cached so it can be resent automatically whenever the pipe
	// reconnects (e.g. after the user toggles the setting off and back
	// on, or Discord itself restarts mid-session).
	bool m_has_activity = false;
	std::string m_last_details, m_last_state;
	std::string m_last_large_image, m_last_large_text;
	std::string m_last_small_image, m_last_small_text;

#ifdef _WIN32
	void *m_pipe = nullptr; // HANDLE, avoids pulling in windows.h here
#else
	int m_socket = -1;
#endif
};
