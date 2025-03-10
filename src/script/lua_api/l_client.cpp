// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>
// Copyright (C) 2017 nerzhul, Loic Blot <loic.blot@unix-experience.fr>

#include "l_client.h"
#include "chatmessage.h"
#include "client/client.h"
#include "client/renderingengine.h"
#include "gui/Sprite.h"
#include "gui/SpriteManager.h"
#include "client/clientevent.h"
#include "client/sound.h"
#include "client/clientenvironment.h"
#include "common/c_content.h"
#include "common/c_converter.h"
#include "cpp_api/s_base.h"
#include "gettext.h"
#include "l_internal.h"
#include "lua_api/l_nodemeta.h"
#include "gui/mainmenumanager.h"
#include "map.h"
#include "util/string.h"
#include "nodedef.h"

#define checkCSMRestrictionFlag(flag) \
	( getClient(L)->checkCSMRestrictionFlag(CSMRestrictionFlags::flag) )

// Not the same as FlagDesc, which contains an `u32 flag`
struct CSMFlagDesc {
	const char *name;
	u64 flag;
};

/*
	FIXME: This should eventually be moved somewhere else
	It also needs to be kept in sync with the definition of CSMRestrictionFlags
	in network/networkprotocol.h
*/
const static CSMFlagDesc flagdesc_csm_restriction[] = {
	{"load_client_mods",  CSM_RF_LOAD_CLIENT_MODS},
	{"chat_messages",     CSM_RF_CHAT_MESSAGES},
	{"read_itemdefs",     CSM_RF_READ_ITEMDEFS},
	{"read_nodedefs",     CSM_RF_READ_NODEDEFS},
	{"lookup_nodes",      CSM_RF_LOOKUP_NODES},
	{"read_playerinfo",   CSM_RF_READ_PLAYERINFO},
	{NULL,      0}
};

// get_current_modname()
int ModApiClient::l_get_current_modname(lua_State *L)
{
	std::string s = ScriptApiBase::getCurrentModNameInsecure(L);
	if (!s.empty())
		lua_pushstring(L, s.c_str());
	else
		lua_pushnil(L);
	return 1;
}

// get_modpath(modname)
int ModApiClient::l_get_modpath(lua_State *L)
{
	std::string modname = readParam<std::string>(L, 1);
	// Client mods use a virtual filesystem, see Client::scanModSubfolder()
	std::string path = modname + ":";
	lua_pushstring(L, path.c_str());
	return 1;
}

// print(text)
int ModApiClient::l_print(lua_State *L)
{
	NO_MAP_LOCK_REQUIRED;
	std::string text = luaL_checkstring(L, 1);
	rawstream << text << std::endl;
	return 0;
}

// display_chat_message(message)
int ModApiClient::l_display_chat_message(lua_State *L)
{
	if (!lua_isstring(L, 1))
		return 0;

	std::string message = luaL_checkstring(L, 1);
	getClient(L)->pushToChatQueue(new ChatMessage(utf8_to_wide(message)));
	lua_pushboolean(L, true);
	return 1;
}

// send_chat_message(message)
int ModApiClient::l_send_chat_message(lua_State *L)
{
	if (!lua_isstring(L, 1))
		return 0;

	// If server disabled this API, discard

	if (checkCSMRestrictionFlag(CSM_RF_CHAT_MESSAGES))
		return 0;

	std::string message = luaL_checkstring(L, 1);
	getClient(L)->sendChatMessage(utf8_to_wide(message));
	return 0;
}

// clear_out_chat_queue()
int ModApiClient::l_clear_out_chat_queue(lua_State *L)
{
	getClient(L)->clearOutChatQueue();
	return 0;
}

// get_player_names()
int ModApiClient::l_get_player_names(lua_State *L)
{
	if (checkCSMRestrictionFlag(CSM_RF_READ_PLAYERINFO))
		return 0;

	auto plist = getClient(L)->getConnectedPlayerNames();
	lua_createtable(L, plist.size(), 0);
	int newTable = lua_gettop(L);
	int index = 1;
	for (const std::string &name : plist) {
		lua_pushstring(L, name.c_str());
		lua_rawseti(L, newTable, index);
		index++;
	}
	return 1;
}

// show_formspec(formspec)
int ModApiClient::l_show_formspec(lua_State *L)
{
	if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
		return 0;

	ClientEvent *event = new ClientEvent();
	event->type = CE_SHOW_LOCAL_FORMSPEC;
	event->show_formspec.formname = new std::string(luaL_checkstring(L, 1));
	event->show_formspec.formspec = new std::string(luaL_checkstring(L, 2));
	getClient(L)->pushToEventQueue(event);
	lua_pushboolean(L, true);
	return 1;
}
// disconnect()
int ModApiClient::l_disconnect(lua_State *L)
{
	// Stops badly written Lua code form causing boot loops
	if (getClient(L)->isShutdown()) {
		lua_pushboolean(L, false);
		return 1;
	}

	g_gamecallback->disconnect();
	lua_pushboolean(L, true);
	return 1;
}

// gettext(text)
int ModApiClient::l_gettext(lua_State *L)
{
	std::string text = strgettext(luaL_checkstring(L, 1));
	lua_pushstring(L, text.c_str());

	return 1;
}

// get_node_or_nil(pos)
// pos = {x=num, y=num, z=num}
int ModApiClient::l_get_node_or_nil(lua_State *L)
{
	// pos
	v3s16 pos = read_v3s16(L, 1);

	// Do it
	bool pos_ok;
	MapNode n = getClient(L)->CSMGetNode(pos, &pos_ok);
	if (pos_ok) {
		// Return node
		pushnode(L, n);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// get_langauge()
int ModApiClient::l_get_language(lua_State *L)
{
#ifdef _WIN32
	char *locale = setlocale(LC_ALL, NULL);
#else
	char *locale = setlocale(LC_MESSAGES, NULL);
#endif
	std::string lang = gettext("LANG_CODE");
	if (lang == "LANG_CODE")
		lang.clear();

	lua_pushstring(L, locale);
	lua_pushstring(L, lang.c_str());
	return 2;
}

// get_meta(pos)
int ModApiClient::l_get_meta(lua_State *L)
{
	v3s16 p = read_v3s16(L, 1);

	// check restrictions first
	bool pos_ok;
	getClient(L)->CSMGetNode(p, &pos_ok);
	if (!pos_ok)
		return 0;

	NodeMetadata *meta = getEnv(L)->getMap().getNodeMetadata(p);
	NodeMetaRef::createClient(L, meta);
	return 1;
}

// get_server_info()
int ModApiClient::l_get_server_info(lua_State *L)
{
	Client *client = getClient(L);
	Address serverAddress = client->getServerAddress();
	lua_newtable(L);
	lua_pushstring(L, client->getAddressName().c_str());
	lua_setfield(L, -2, "address");
	lua_pushstring(L, serverAddress.serializeString().c_str());
	lua_setfield(L, -2, "ip");
	lua_pushinteger(L, serverAddress.getPort());
	lua_setfield(L, -2, "port");
	lua_pushinteger(L, client->getProtoVersion());
	lua_setfield(L, -2, "protocol_version");
	return 1;
}

// get_item_def(itemstring)
int ModApiClient::l_get_item_def(lua_State *L)
{
	IGameDef *gdef = getGameDef(L);
	assert(gdef);

	IItemDefManager *idef = gdef->idef();
	assert(idef);

	if (checkCSMRestrictionFlag(CSM_RF_READ_ITEMDEFS))
		return 0;

	if (!lua_isstring(L, 1))
		return 0;

	std::string name = readParam<std::string>(L, 1);
	if (!idef->isKnown(name))
		return 0;
	const ItemDefinition &def = idef->get(name);

	push_item_definition_full(L, def);

	return 1;
}

// get_node_def(nodename)
int ModApiClient::l_get_node_def(lua_State *L)
{
	IGameDef *gdef = getGameDef(L);
	assert(gdef);

	const NodeDefManager *ndef = gdef->ndef();
	assert(ndef);

	if (!lua_isstring(L, 1))
		return 0;

	if (checkCSMRestrictionFlag(CSM_RF_READ_NODEDEFS))
		return 0;

	std::string name = readParam<std::string>(L, 1);
	const ContentFeatures &cf = ndef->get(ndef->getId(name));
	if (cf.name != name) // Unknown node. | name = <whatever>, cf.name = ignore
		return 0;

	push_content_features(L, cf);

	return 1;
}

// get_privilege_list()
int ModApiClient::l_get_privilege_list(lua_State *L)
{
	const Client *client = getClient(L);
	lua_newtable(L);
	for (const std::string &priv : client->getPrivilegeList()) {
		lua_pushboolean(L, true);
		lua_setfield(L, -2, priv.c_str());
	}
	return 1;
}

// get_builtin_path()
int ModApiClient::l_get_builtin_path(lua_State *L)
{
	lua_pushstring(L, BUILTIN_MOD_NAME ":");
	return 1;
}

// get_csm_restrictions()
int ModApiClient::l_get_csm_restrictions(lua_State *L)
{
	u64 flags = getClient(L)->getCSMRestrictionFlags();
	const CSMFlagDesc *flagdesc = flagdesc_csm_restriction;

	lua_newtable(L);
	for (int i = 0; flagdesc[i].name; i++) {
		setboolfield(L, -1, flagdesc[i].name, !!(flags & flagdesc[i].flag));
	}
	return 1;
}
int ModApiClient::l_create_sprite(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "x");
    float normalized_x = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "y");
    float normalized_y = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "width");
    int width = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "height");
    int height = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

	float screenWidth = RenderingEngine::get_video_driver()->getScreenSize().Width;
	float screenHeight = RenderingEngine::get_video_driver()->getScreenSize().Height;
    int x = static_cast<int>(normalized_x * screenWidth);
    int y = static_cast<int>(normalized_y * screenHeight);

    Sprite sprite(x, y, width, height);
    sprites.push_back(sprite);

    lua_pushinteger(L, sprite.get_id());
    return 1;
}

int ModApiClient::l_set_visible(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "id");
    unsigned int id = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "flag");
    bool flag = lua_toboolean(L, -1);

    for (size_t i = 0; i < sprites.size(); i++)
    {
        if (sprites[i].get_id() == id) {
            sprites[i].setVisible(flag);
        }
    }

    return 1;
}

int ModApiClient::l_get_position(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "id");
    unsigned int id = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].get_id() == id) {
            auto position = sprites[i].get_position();
			float screenWidth = RenderingEngine::get_video_driver()->getScreenSize().Width;
			float screenHeight = RenderingEngine::get_video_driver()->getScreenSize().Height;
            float normalized_x = position[0] / screenWidth;
            float normalized_y = position[1] / screenHeight;

            lua_pushnumber(L, normalized_x);
            lua_pushnumber(L, normalized_y);
            return 2;
        }
    }

    std::cerr << "Error: Sprite with ID " << id << " not found." << std::endl;
    return 0;
}

int ModApiClient::l_get_real_position(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "id");
    unsigned int id = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].get_id() == id) {
            auto position = sprites[i].get_position();

            lua_pushnumber(L, position[0]);
            lua_pushnumber(L, position[0]);
            return 2;
        }
    }

    std::cerr << "Error: Sprite with ID " << id << " not found." << std::endl;
    return 0;
}

int ModApiClient::l_get_width_offset(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "id");
    unsigned int id = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].get_id() == id) {
			float screenWidth = RenderingEngine::get_video_driver()->getScreenSize().Width;
            float normalized_w = sprites[i].get_rect().getWidth() / screenWidth;

            lua_pushnumber(L, normalized_w);
            return 1;
        }
    }

    std::cerr << "Error: Sprite with ID " << id << " not found." << std::endl;
    return 0;
}

int ModApiClient::l_get_height_offset(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "id");
    unsigned int id = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].get_id() == id) {
			float screenHeight = RenderingEngine::get_video_driver()->getScreenSize().Height;
            float normalized_h = sprites[i].get_rect().getHeight() / screenHeight;

            lua_pushnumber(L, normalized_h);
            return 1;
        }
    }

    std::cerr << "Error: Sprite with ID " << id << " not found." << std::endl;
    return 0;
}

int ModApiClient::l_get_rectangle(lua_State* L)
{
    unsigned int id  = luaL_checkinteger(L, 1);
    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].get_id() == id) {
            lua_pushinteger(L, sprites[i].get_rect().getWidth());
            lua_pushinteger(L, sprites[i].get_rect().getHeight());

            return 2;
        }
   }

   return 0;
}

int ModApiClient::l_is_moved(lua_State* L)
{
    unsigned int id  = luaL_checkinteger(L, 1);
    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].get_id() == id) {
            lua_pushboolean(L, sprites[i].is_moved());
            return 1;
        }
    }

    return 0;
}

void ModApiClient::Initialize(lua_State *L, int top)
{
	API_FCT(get_current_modname);
	API_FCT(get_modpath);
	API_FCT(print);
	API_FCT(display_chat_message);
	API_FCT(send_chat_message);
	API_FCT(clear_out_chat_queue);
	API_FCT(get_player_names);
	API_FCT(show_formspec);
	API_FCT(gettext);
	API_FCT(get_node_or_nil);
	API_FCT(disconnect);
	API_FCT(get_meta);
	API_FCT(get_server_info);
	API_FCT(get_item_def);
	API_FCT(get_node_def);
	API_FCT(get_privilege_list);
	API_FCT(get_builtin_path);
	API_FCT(get_language);
	API_FCT(get_csm_restrictions);
	API_FCT(create_sprite);
    API_FCT(set_visible);
    API_FCT(get_position);
    API_FCT(get_rectangle);
    API_FCT(get_width_offset);
    API_FCT(get_height_offset);
    API_FCT(is_moved);
}
