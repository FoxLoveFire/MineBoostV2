#include "Menu.h"

Sprite_ Menu::coords_sprite = Sprite_();

core::vector2d<s32> offset_f;
Sprite_ fov_sprite = Sprite_();

Sprite_ chat = Sprite_();
core::vector2d<s32> offset_chp;

Sprite_ keystr = Sprite_();
core::vector2d<s32> offset_keys;

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

void Menu::updateScrollBarPosition(gui::IGUIScrollBar* scrollbar, int screenW, int screenH)
{
    if (!scrollbar) return;
    const int SCROLL_WIDTH = 300;
    const int SCROLL_HEIGHT = 20;   
    const int BOTTOM_OFFSET = 25;
    int bgLeft = (screenW - WIDTH_) / 2;
    int bgTop = (screenH - HEIGHT_) / 2;
    int bgBottom = bgTop + HEIGHT_;
    int left = bgLeft + (WIDTH_ - SCROLL_WIDTH) / 2;
    int top = bgBottom - BOTTOM_OFFSET - SCROLL_HEIGHT;
    int right = left + SCROLL_WIDTH;
    int bottom = bgBottom - BOTTOM_OFFSET;
    scrollbarTop = bgBottom - BOTTOM_OFFSET - SCROLL_HEIGHT;

    scrollbar->setRelativePosition(core::rect<s32>(left, top, right, bottom));
}

void Menu::updateFpsScrollBarPosition(gui::IGUIScrollBar* scrollbar, int screenW, int screenH)
{
    if (!scrollbar) return;
    const int SCROLL_WIDTH = 300;
    const int SCROLL_HEIGHT = 20;   
    const int BOTTOM_OFFSET = 65;
    int bgLeft = (screenW - WIDTH_) / 2;
    int bgTop = (screenH - HEIGHT_) / 2;
    int bgBottom = bgTop + HEIGHT_;
    int left = bgLeft + (WIDTH_ - SCROLL_WIDTH) / 2;
    int top = bgBottom - BOTTOM_OFFSET - SCROLL_HEIGHT;
    int right = left + SCROLL_WIDTH;
    int bottom = bgBottom - BOTTOM_OFFSET;
    fpsScrollbarTop = bgBottom - BOTTOM_OFFSET - SCROLL_HEIGHT;

    scrollbar->setRelativePosition(core::rect<s32>(left, top, right, bottom));
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

    fps_scrollbar = env->addScrollBar(true, core::rect<s32>((screenW - 300) / 2 + (-45), screenH - 130 + (-85),
        (screenW + 300) / 2 + (-45), screenH - 110 + (-85)), nullptr, 106);
    fps_scrollbar->setMax(1000);
    fps_scrollbar->setMin(1);
    fps_scrollbar->setSmallStep(1);
    fps_scrollbar->setLargeStep(1);
    fps_scrollbar->setPos(g_settings->getFloat("fps_max"));
    fps_scrollbar->setVisible(false);

	coords_sprite.width = 140;
	coords_sprite.height = 30;
    if (!g_settings->exists("coords_sprite")) {
        coords_sprite.x = 5;
        coords_sprite.y = (Environment->getVideoDriver()->getScreenSize().Height - coords_sprite.height / 2 + g_fontengine->getTextHeight() - coords_sprite.height);
    } else {
        v2f data = g_settings->getV2F("coords_sprite");
        coords_sprite.x = data[0];
        coords_sprite.y = data[1];
    }

    fov_sprite.width = 140;
    fov_sprite.height = 30;
    if (!g_settings->exists("fov_coords")) {
        fov_sprite.x = 5;
        fov_sprite.y = (Environment->getVideoDriver()->getScreenSize().Height - coords_sprite.height / 2 + g_fontengine->getTextHeight() - coords_sprite.height);
    } else {
        v2f fov_data = g_settings->getV2F("fov_coords");
        fov_sprite.x = fov_data[0];
        fov_sprite.y = fov_data[1];
    }

	chat.width = 1200;
	chat.height = 200;
	chat.x = g_settings->getS32("chat_x");
	chat.y = g_settings->getS32("chat_y");

    keystr.width = 132;
    keystr.height = 150;
    keystr.x = g_settings->getS32("keys_x");
    keystr.y = g_settings->getS32("keys_y");

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
                fps_scrollbar->setVisible(true);
            } else {
                scrollbar->setVisible(false);
                fps_scrollbar->setVisible(false);
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

    Button button_mouse;
    button_mouse.addButton(core::rect<s32>(x + 15, y + 15 + 90, x + 15 + 160, y + 15 + 30 + 90),
    L"Mouse");
    button_mouse.setColor(video::SColor(115, 0, 0, 0));
    button_mouse.setOnClick([this]() { onCategoryButtonClick(SettingCategory::MISC); });
    buttons.push_back(button_mouse);

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
    if (this-> current_category == SettingCategory::RENDER) {
        scrollbar->setVisible(true);
        fps_scrollbar->setVisible(true);
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
    scrollbar->setVisible(false);
    fps_scrollbar->setVisible(false);
}

bool Menu::OnEvent(const irr::SEvent& event)
{
    s32 screenWidth = Environment->getVideoDriver()->getScreenSize().Width, screenHeight = Environment->getVideoDriver()->getScreenSize().Height;
    
    if (event.EventType == irr::EET_KEY_INPUT_EVENT && isOpen) {
        if (event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown) {
            close();
            return true;
        }
        
        if (event.KeyInput.Key == KEY_KEY_E && event.KeyInput.PressedDown && 
            (event.KeyInput.Shift || GetAsyncKeyState(VK_MENU) || GetAsyncKeyState(VK_LMENU) || GetAsyncKeyState(VK_RMENU))) {
            editMode = !editMode;
            return true;
        }
    }

    if (editMode && isOpen) {
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
            switch (event.MouseInput.Event) {
                case irr::EMIE_LMOUSE_PRESSED_DOWN:
                    if (coords_sprite.get_rect().isPointInside(core::vector2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
                        coords_sprite.isDragging = true;
                        offset = core::vector2d<s32>(event.MouseInput.X - coords_sprite.x, event.MouseInput.Y - coords_sprite.y);
                        return true;
                    }

                    if (fov_sprite.get_rect().isPointInside(core::vector2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
                        fov_sprite.isDragging = true;
                        offset_f = core::vector2d<s32>(event.MouseInput.X - fov_sprite.x, event.MouseInput.Y - fov_sprite.y);
                        return true;
                    }

                    if (chat.get_rect().isPointInside(core::vector2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
                        chat.isDragging = true;
                        offset_chp = core::vector2d<s32>(event.MouseInput.X - chat.x, event.MouseInput.Y - chat.y);
                        return true;
                    }

                    if (keystr.get_rect().isPointInside(core::vector2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
                        keystr.isDragging = true;
                        offset_keys = core::vector2d<s32>(event.MouseInput.X - keystr.x, event.MouseInput.Y - keystr.y);
                        return true;
                    }
                    break;
                    
                case irr::EMIE_LMOUSE_LEFT_UP:
                    coords_sprite.isDragging = false;
                    fov_sprite.isDragging = false;
                    chat.isDragging = false;
                    keystr.isDragging = false;
                    break;
                    
                case irr::EMIE_MOUSE_MOVED:
                    if (coords_sprite.isDragging) {
                        coords_sprite.x = event.MouseInput.X - offset.X;
                        coords_sprite.y = event.MouseInput.Y - offset.Y;
                        coords_sprite.save(screenWidth, screenHeight, "coords_sprite");
                    }

                    if (fov_sprite.isDragging) {
                        fov_sprite.x = event.MouseInput.X - offset_f.X;
                        fov_sprite.y = event.MouseInput.Y - offset_f.Y;
                        fov_sprite.save(screenWidth, screenHeight, "fov_coords");
                    }

                    if (chat.isDragging) {
                        chat.x = event.MouseInput.X - offset_chp.X;
                        chat.y = event.MouseInput.Y - offset_chp.Y;
                        chat.save(screenWidth, screenHeight, "chat_x", "chat_y");
                    }

                    if (keystr.isDragging) {
                        keystr.x = event.MouseInput.X - offset_keys.X;
                        keystr.y = event.MouseInput.Y - offset_keys.Y;
                        keystr.save(screenWidth, screenHeight, "keys_x", "keys_y");
                    }
                    break;
                    
                default:
                    break;
            }
        }
        return Parent ? Parent->OnEvent(event) : false;
    }
    
    if (isOpen && !editMode) {
        for (size_t i = 0; i < buttons.size(); i++) {
            buttons[i].isPressed(event);
        }

        for (size_t i = 0; i < items.size(); i++) {
            items[i].isPressed(event);
        }
    }

    return Parent ? Parent->OnEvent(event) : false;
}

void Menu::draw()
{
    updateScrollBarPosition(scrollbar, screenW, screenH);
    updateFpsScrollBarPosition(fps_scrollbar, screenW, screenH);
    
    if (editMode && isOpen) {
        driver->draw2DRectangleOutline(core::rect<s32>(coords_sprite.get_rect()), video::SColor(255, 255, 0, 255));
        driver->draw2DRectangleOutline(core::rect<s32>(fov_sprite.get_rect()), video::SColor(255, 255, 0, 255));
        driver->draw2DRectangleOutline(core::rect<s32>(chat.get_rect()), video::SColor(255, 255, 0, 255));
        driver->draw2DRectangleOutline(core::rect<s32>(keystr.get_rect()), video::SColor(255, 255, 0, 255));
    }
    
    if (isOpen && !editMode) {
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

        g_settings->setU16("fps_max", fps_scrollbar->getPos());

        if (current_category == SettingCategory::RENDER) {
            std::wstring wfov = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes("FOV: " + std::to_string(int(g_settings->getFloat("fov_custom.data"))));
            
            int offsetX = 190;
            font->draw(wfov.c_str(), core::rect<s32>(((screenW - 300) / 2 + (-45)) * 1.72 - offsetX, scrollbarTop,
            (screenW + 300) / 2 + (-45) - offsetX, scrollbarTop + 20), video::SColor(255, 255, 255, 255));
            
            std::wstring wfps = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes("FPS: " + std::to_string(int(fps_scrollbar->getPos())));
            font->draw(wfps.c_str(), core::rect<s32>(((screenW - 300) / 2 + (-45)) * 1.72 - offsetX, fpsScrollbarTop,
            (screenW + 300) / 2 + (-45) - offsetX, fpsScrollbarTop + 20), video::SColor(255, 255, 255, 255));
        }
    } else {
        scrollbar->setVisible(false);
        fps_scrollbar->setVisible(false);
    }
}

Menu::~Menu()
{
    if (scrollbar) scrollbar->remove();
    if (fps_scrollbar) fps_scrollbar->remove();
}