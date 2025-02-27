#include "Button.h"

void Button::addButton(core::rect<s32> position, std::wstring title)
{
    this->position = position;
    this->title = title;
}

void Button::setColor(video::SColor color)
{
    this->color = color;
}

void Button::setFontColor(video::SColor font_color)
{
    this->font_color = font_color;
}

void Button::draw(video::IVideoDriver *driver)
{
    gui::IGUIFont* font = g_fontengine->getFont(FONT_SIZE_UNSPECIFIED, FM_Standard);

    if (IsVisible() && font) {
        driver->draw2DRectangle(color, position);

        driver->draw2DRectangleOutline(position, video::SColor(255, 255, 0, 0), 2);
        core::dimension2du textSize = font->getDimension(title.c_str());
        s32 textX = position.UpperLeftCorner.X + (position.getWidth() - textSize.Width) / 2;
        s32 textY = position.UpperLeftCorner.Y + (position.getHeight() - textSize.Height) / 2;

        font->draw(title.c_str(), core::rect<s32>(textX, textY, textX + textSize.Width, textY + textSize.Height),
         font_color);
    }
}

bool Button::isPressed(const irr::SEvent &event)
{
    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            if (position.isPointInside(core::vector2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
                setColor(video::SColor(166, 0, 0, 0));
                if (onClickCallback) {
                    onClickCallback();
                }
                return true;
            }
        }
    }

    if (event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
        setColor(video::SColor(105, 0, 0, 0));
        return true;
    }

    return false;
}
