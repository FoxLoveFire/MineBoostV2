#include "Menu.h"

#define WIDTH_ 800
#define HEIGHT_ 450

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
                setColor(video::SColor(255, 0, 255, 0));
                return true;
            }
        }
    }

    if (event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
        setColor(video::SColor(115, 0, 255, 0));
        return true;
    }
    
    return false;
}

void drawBackground(video::IVideoDriver* driver, s32 screenW, s32 screenH) {
    s32 x = (screenW - WIDTH_) / 2;
    s32 y = (screenH - HEIGHT_) / 2;
    driver->draw2DRectangle(video::SColor(165, 52, 2, 94), core::rect<s32>(x, y, x + WIDTH_, y + HEIGHT_));

    s32 lineOffsetX = 190;
    s32 lineX = x + lineOffsetX;
    s32 lineYStart = y;
    s32 lineYEnd = y + HEIGHT_;

    driver->draw2DLine(core::vector2d<s32>(lineX, lineYStart), core::vector2d<s32>(lineX, lineYEnd), video::SColor(255, 121, 121, 121));
}

Menu::Menu(gui::IGUIEnvironment* env, 
    gui::IGUIElement* parent, 
    s32 id, IMenuManager* menumgr, 
    Client* client)
    : IGUIElement(gui::EGUIET_ELEMENT, env, parent, id, 
    core::rect<s32>(0, 0, 0, 0)), 
    m_menumgr(menumgr),
    m_client(client),
    env(env), driver(env->getVideoDriver())
{
    screenW = driver->getScreenSize().Width;
    screenH = driver->getScreenSize().Height;

    initCategoryButtons(parent);
}

void Menu::initCategoryButtons(gui::IGUIElement* parent)
{
    s32 x = (screenW - WIDTH_) / 2;
    s32 y = (screenH - HEIGHT_) / 2;

    Button button;
    button.addButton(core::rect<s32>(x + 15, y + 15, x + 15 + 160, y + 15 + 30), L"PVP");
    button.setColor(video::SColor(115, 0, 255, 0));
    buttons.push_back(button);

    Button button_v;
    button_v.addButton(core::rect<s32>(x + 15, y + 15 + 45, x + 15 + 160, y + 15 + 30 + 45), L"PVE");
    button_v.setColor(video::SColor(115, 0, 255, 0));
    buttons.push_back(button_v);

    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].setVisible(false);
    }
}

void Menu::create()
{
    core::rect<s32> screenRect(0, 0, 
        env->getVideoDriver()->getScreenSize().Width, 
        env->getVideoDriver()->getScreenSize().Height);
    setRelativePosition(screenRect);
    IGUIElement::setVisible(true);
    Environment->setFocus(this);
    m_menumgr->createdMenu(this);
    isOpen = true;
    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].setVisible(true);
    }
}

void Menu::close()
{
    Environment->removeFocus(this);
    m_menumgr->deletingMenu(this);
    IGUIElement::setVisible(false);
    isOpen = false;
    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].setVisible(false);
    }
}

bool Menu::OnEvent(const irr::SEvent& event)
{
    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].isPressed(event);
    }

    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown) {
            close();
            return true; 
        }
    }

    return Parent ? Parent->OnEvent(event) : false; 
}

void Menu::draw()
{
    if (isOpen) {
        drawBackground(driver, screenW, screenH);
        
        for (size_t i = 0; i < buttons.size(); i++) {
            buttons[i].draw(driver);
        }
    }
}
Menu::~Menu()
{

}