#include "Items.h"
#include "Menu.h"
#include "../settings.h"

Items::Items(core::rect<s32> position)
{
    this->position = position;
}

void Items::set_color(irr::video::SColor color)
{
    color_def = color;
}

irr::core::rect<s32> Items::get_rect()
{
    return irr::core::rect<s32>(X, Y, w, h);
}

void Items::set_title(std::wstring title)
{
    this->title = title;
}

bool Items::isPressed(const irr::SEvent& event)
{
    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            if (position.isPointInside(core::vector2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
                if (g_settings->exists(this->setting_item)) {
                    bool currentValue = g_settings->getBool(this->setting_item);
                    g_settings->setBool(this->setting_item, !currentValue);
                }
                return true;
            }
        }
    }

    return false;
}

std::string Items::get_title()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(title);
}

void Items::draw(irr::video::IVideoDriver *driver, s32 screenW, s32 screenH)
{
    driver->draw2DRectangle(color_def, position);
    if (g_settings->getBool(this->setting_item)) {
        driver->draw2DRectangleOutline(position, irr::video::SColor(255, 0, 255, 0));
    } else {
        driver->draw2DRectangleOutline(position, irr::video::SColor(230, 75, 125, 250));
    }

    gui::IGUIFont* font = g_fontengine->getFont(FONT_SIZE_UNSPECIFIED, FM_Standard);
    core::dimension2du textSize = font->getDimension(title.c_str());
    s32 textX = position.UpperLeftCorner.X + (position.getWidth() - textSize.Width) / 2;
    s32 textY = position.UpperLeftCorner.Y + (position.getHeight() - textSize.Height) / 5;

    font->draw(title.c_str(), core::rect<s32>(textX, textY, textX + textSize.Width, textY + textSize.Height),
               irr::video::SColor(255, 255, 255, 255));
}

Items::~Items()
{

}