#include "Menu.h"

void drawBackground(video::IVideoDriver* driver, s32 screenW, s32 screenH) {
    s32 x = (screenW - WIDTH_) / 2;
    s32 y = (screenH - HEIGHT_) / 2;
    driver->draw2DRectangle(video::SColor(165, 0, 0, 0), core::rect<s32>(x, y, x + WIDTH_, y + HEIGHT_));
    driver->draw2DRectangleOutline(core::rect<s32>(x, y, x + WIDTH_, y + HEIGHT_), video::SColor(255, 75, 125, 250), 4);
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
    this->parent = parent;

    scrollbar = env->addScrollBar(true, core::rect<s32>((screenW - 300) / 2 + (-45), screenH - 90 + (-85),
    (screenW + 300) / 2 + (-45), screenH - 70 + (-85)), nullptr, 105);
    scrollbar->setMax(160);
    scrollbar->setMin(75);
    scrollbar->setPos(g_settings->getFloat("fov_custom.data"));
    scrollbar->setVisible(false);
    scrollbar->setBackgroundColor(video::SColor(90, 0, 0, 0), true);

    initCategoryButtons();
}

void Menu::ItemsInit(SettingCategory category)
{
    std::vector<Setting> settings = getSettings();
    s32 x = (screenW - WIDTH_) / 2;
    s32 y = (screenH - HEIGHT_) / 2;

    int startPosX = 190 + 25;
    int startPosY = y + 25;
    const int itemWidth = 120;
    const int itemHeight = 120;
    const int spacing = 15;
    items.clear();

    for (size_t i = 0; i < settings.size(); ++i) {
        if (settings[i].category == category) {
            if (settings[i].category == SettingCategory::RENDER) {
                scrollbar->setVisible(true);
            } else {
                scrollbar->setVisible(false);
            }
            int posX = x + startPosX + (items.size() % 4) * (itemWidth + spacing);
            int posY = startPosY + (items.size() / 4) * (itemHeight + spacing);

            Items it(core::rect<s32>(posX, posY, posX + itemWidth, posY + itemHeight));
            it.set_title(stringToWString(settings[i].name));
            it.set_setting(settings[i].value);
            it.setSetting(settings[i]);
            items.push_back(it);
        }
    }
}

void Menu::onCategoryButtonClick(SettingCategory category)
{
    this->current_category = category;
    ItemsInit(category);
}

void Menu::initCategoryButtons()
{
    s32 x = (screenW - WIDTH_) / 2;
    s32 y = (screenH - HEIGHT_) / 2;

    Button button_gui;
    button_gui.addButton(core::rect<s32>(x + 15, y + 15, x + 15 + 160, y + 15 + 30), L"GUI");
    button_gui.setColor(video::SColor(105, 0, 0, 0));
    button_gui.setOnClick([this]() { onCategoryButtonClick(SettingCategory::GUI); });
    buttons.push_back(button_gui);

    Button button_render;
    button_render.addButton(core::rect<s32>(x + 15, y + 15 + 45, x + 15 + 160, y + 15 + 30 + 45),
    L"Render");
    button_render.setColor(video::SColor(115, 0, 0, 0));
    button_render.setOnClick([this]() { onCategoryButtonClick(SettingCategory::RENDER); });
    buttons.push_back(button_render);

    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].setVisible(false);
    }
    ItemsInit(SettingCategory::GUI);
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

    for (size_t i = 0; i < items.size(); i++) {
        items[i].isPressed(event);
    }

    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown) {
            scrollbar->setVisible(false);
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

        for (size_t i = 0; i < items.size(); i++) {
            items[i].draw(driver, screenW, screenH);
        }

        if (g_settings->getBool("fov_custom")) {
            g_settings->setFloat("fov_custom.data", scrollbar->getPos());
        }

        if (current_category == SettingCategory::RENDER) {
            std::wstring wfov = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes("FOV: " + std::to_string(int(g_settings->getFloat("fov_custom.data"))));
            font->draw(wfov.c_str(), core::rect<s32>(((screenW - 300) / 2 + (-45)) * 1.72, screenH - 90 + (-85),
            (screenW + 300) / 2 + (-45), screenH - 70 + (-85)), video::SColor(255, 255, 255, 255));
        }

    }
}

Menu::~Menu()
{

}