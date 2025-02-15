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

using namespace irr;
using namespace video;
using namespace gui;

class Menu: public IGUIElement
{
public:
    Menu(gui::IGUIEnvironment* env, gui::IGUIElement* parent, s32 id, 
    IMenuManager* menumgr, Client *client);
    ~Menu();

    void create();

    void close();

    virtual bool OnEvent(const irr::SEvent& event);

private:
    IMenuManager* m_menumgr; 
    Client* m_client;
    gui::IGUIEnvironment* env;
};

#endif