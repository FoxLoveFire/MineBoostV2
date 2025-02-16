#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <irrlicht.h>
#include "IGUIEnvironment.h"
#include "gui/modalMenu.h"
#include <vector>
#include "irrlichttypes_extrabloated.h"
#include "IVideoDriver.h"
#include "client/game.h"

class Client;
std::vector<Sprite> sprites;

using namespace irr;
using namespace video;
using namespace gui;

class SpriteManager: public IGUIElement
{
public:
    SpriteManager(gui::IGUIEnvironment* env, gui::IGUIElement* parent, s32 id, 
    IMenuManager* menumgr, Client *client);
    ~SpriteManager();

    void create();

    void close();

    virtual bool OnEvent(const irr::SEvent& event);

    virtual void draw();

    bool isDragging = false;
    
private:
    IMenuManager* m_menumgr; 
    Client* m_client;
    IVideoDriver *driver = nullptr;
    gui::IGUIEnvironment* env;
    Sprite* selectedSprite = nullptr;
    int offsetX = 0;
    int offsetY = 0;
};

#endif