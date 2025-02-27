#ifndef BUTTON_H
#define BUTTON_H

#include <irrlicht.h>
#include "IGUIEnvironment.h"
#include "gui/modalMenu.h"
#include <vector>
#include "client/client.h"
#include "irrlichttypes_extrabloated.h"
#include "IVideoDriver.h"
#include "client/game.h"
#include "client/fontengine.h"
#include "IGUIFont.h"

using namespace irr;
using namespace gui;

class Button
{
public:

    void addButton(core::rect<s32> position, std::wstring title);

    void draw(video::IVideoDriver *driver);

    inline bool IsVisible() { return is_visible; }

    void setVisible(bool flag) { this->is_visible = flag; }

    bool isPressed(const irr::SEvent &event);

    void setColor(video::SColor color);

    void setFontColor(video::SColor font_color);

    void setOnClick(std::function<void()> callback)
    {
        onClickCallback = callback;
    }

private:
    core::rect<s32> position;
    std::wstring title;
    bool is_visible = false;

    video::SColor color = video::SColor(255, 0, 0, 0);
    video::SColor font_color = video::SColor(255, 255, 255, 255);

    std::function<void()> onClickCallback;
};


#endif