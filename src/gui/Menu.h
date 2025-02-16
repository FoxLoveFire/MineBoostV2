#ifndef MENU_H
#define MENU_H

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

private:
    core::rect<s32> position;
    std::wstring title;
    bool is_visible = false;

    video::SColor color = video::SColor(255, 0, 0, 0);
    video::SColor font_color = video::SColor(255, 255, 255, 255);
};

class Menu: public IGUIElement
{
public:
    Menu(gui::IGUIEnvironment* env, gui::IGUIElement* parent, s32 id, 
    IMenuManager* menumgr, Client *client);
    ~Menu();

    void create();

    void close();

    void initCategoryButtons(gui::IGUIElement* parent);
    virtual bool OnEvent(const irr::SEvent& event);

    virtual void draw();

private:
    IMenuManager* m_menumgr; 
    Client* m_client;
    gui::IGUIEnvironment* env;

    video::IVideoDriver* driver = nullptr;

    bool isOpen = false;
    s32 screenW, screenH;
    std::vector<Button> buttons;
};

#endif