#ifndef ITEMS_H
#define ITEMS_H

#include "IVideoDriver.h"
#include "irrlichttypes_extrabloated.h"
#include "client/fontengine.h"
#include "IGUIFont.h"
#include <string>
#include <locale>
#include <codecvt>


class Items
{
public:
    Items(core::rect<s32> position);
    ~Items();

    void set_color(irr::video::SColor color);
    
    irr::core::rect<s32> get_rect();

    void set_title(std::wstring title);
    
    void set_setting(std::string setting) { this->setting_item = setting;}

    std::string get_title();

    void draw(irr::video::IVideoDriver *driver, s32 screenW, s32 screenH);

    bool isPressed(const irr::SEvent& event);

private:
    int X, Y;
    int w, h;
    irr::video::SColor color_def = irr::video::SColor(105, 0, 0, 0);
    irr::video::SColor color_press = irr::video::SColor(255, 0, 255, 0);

    std::wstring title;
    std::string setting_item;
    core::rect<s32> position;
};

#endif