#ifndef L_SPRITE_H
#define L_SPRITE_H

#include "lua_api/l_base.h"
#include "lua_api/l_internal.h"
#include "gui/Sprite.h"
#include "gui/SpriteManager.h"

class l_sprite: public ModApiBase
{
public:
    l_sprite();
    ~l_sprite();

    static int l_create_sprite(lua_State* L);

    static int l_get_id(lua_State* L);

    static int l_set_visible(lua_State* L);
    
    static int l_set_position(lua_State* L);

    static int l_get_position(lua_State* L);

    static int l_get_rectangle(lua_State* L);

    static int l_get_width_offset(lua_State* L);

    static int l_get_height_offset(lua_State* L);

    static int l_get_real_position(lua_State* L);

    static int l_is_moved(lua_State* L);
    
    static void Initialize(lua_State *L, int top);


private:
    int id;
    int X, Y;
    int Width, Height;
};


#endif