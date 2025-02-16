#include "SpriteManager.h"

std::vector<Sprite> sprites = {};
SpriteManager::SpriteManager(gui::IGUIEnvironment* env, 
    gui::IGUIElement* parent, 
    s32 id, IMenuManager* menumgr, 
    Client* client)
    : IGUIElement(gui::EGUIET_ELEMENT, env, parent, id, 
    core::rect<s32>(0, 0, 0, 0)), 
    m_menumgr(menumgr),
    m_client(client),
    env(env)
{
    driver = env->getVideoDriver();
}

void SpriteManager::create()
{
    core::rect<s32> screenRect(0, 0, 
        env->getVideoDriver()->getScreenSize().Width, 
        env->getVideoDriver()->getScreenSize().Height);
    setRelativePosition(screenRect);
    IGUIElement::setVisible(true);
    Environment->setFocus(this);
    m_menumgr->createdMenu(this);
}

bool SpriteManager::OnEvent(const irr::SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT)
    {
        if (event.KeyInput.Key == KEY_ESCAPE)
        {
            close();
            return true; 
        }
    }

    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            for (size_t i = 0; i < sprites.size(); i++) {
                if (sprites[i].get_rect().isPointInside(core::vector2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
                    selectedSprite = &sprites[i];
                    isDragging = true;
                    selectedSprite->set_moved(true);
                    offsetX = event.MouseInput.X - selectedSprite->get_position()[0];
                    offsetY = event.MouseInput.Y - selectedSprite->get_position()[1];
                    return true;
                }
            }
        } else if (event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
            isDragging = false;
            selectedSprite->set_moved(false);
            selectedSprite = nullptr;
            return true;
        } else if (event.MouseInput.Event == irr::EMIE_MOUSE_MOVED && isDragging && selectedSprite) {
            selectedSprite->set_position(event.MouseInput.X - offsetX, event.MouseInput.Y - offsetY);
            return true;
        }
    }

    return Parent ? Parent->OnEvent(event) : false; 
}

void SpriteManager::close()
{
    Environment->removeFocus(this);
    m_menumgr->deletingMenu(this);
    IGUIElement::setVisible(false);
}

void SpriteManager::draw()
{
	for (size_t i = 0; i < sprites.size(); i++)
	{
		sprites[i].draw(driver);
	}
}

SpriteManager::~SpriteManager()
{
}