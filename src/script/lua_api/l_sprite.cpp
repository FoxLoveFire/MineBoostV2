#include "l_sprite.h"
#include "client/renderingengine.h"

int l_sprite::l_create_sprite(lua_State* L) 
{
    luaL_checktype(L, 1, LUA_TTABLE);
    
    float screenWidth = RenderingEngine::get_video_driver()->getScreenSize().Width;
    float screenHeight = RenderingEngine::get_video_driver()->getScreenSize().Height;

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

    int x = static_cast<int>(normalized_x * screenWidth);
    int y = static_cast<int>(normalized_y * screenHeight);

    Sprite sprite(x, y, width, height);
    sprites.push_back(sprite);

    lua_pushinteger(L, sprite.get_id());
    return 1;
}

int l_sprite::l_set_visible(lua_State* L)
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

int l_sprite::l_get_position(lua_State* L)
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

int l_sprite::l_get_real_position(lua_State* L)
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

int l_sprite::l_get_width_offset(lua_State* L)
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

int l_sprite::l_get_height_offset(lua_State* L)
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

int l_sprite::l_get_rectangle(lua_State* L)
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

int l_sprite::l_is_moved(lua_State* L)
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

void l_sprite::Initialize(lua_State *L, int top)
{
    API_FCT(create_sprite);
    API_FCT(set_visible);
    API_FCT(get_position);
    API_FCT(get_rectangle);
    API_FCT(get_width_offset);
    API_FCT(get_height_offset);
    API_FCT(is_moved);
}