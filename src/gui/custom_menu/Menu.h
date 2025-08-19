#ifndef MENU_H
#define MENU_H

#define WIDTH_ 800
#define HEIGHT_ 450

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
#include "IGUIScrollBar.h"
#include "Button.h"
#include "Items.h"
#include "settings.h"

using namespace irr;
using namespace gui;
#include <map>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>

struct Sprite_
{
    int x = 0, y = 0;
    int width = 0;
    int height = 0;
    core::rect<s32> get_rect() {return core::rect<s32>(x, y, x + width, y + height);}
    bool isDragging = false;
    bool changedPos = false;

    void save(s32 screenWidth, s32 screenHeight, std::string name) {
        if (x < 0) {
            x = 0;
        } else if (y + height > screenHeight) {
            y = screenHeight - height;
        }
        g_settings->setV2F(name, v2f(x, y));
    }

	void save(s32 screenWidth, s32 screenHeight, std::string name, std::string name1) {
		g_settings->setS32(name, x);
		g_settings->setS32(name1, y);
	}

};

class Menu: public IGUIElement
{
public:
    Menu(gui::IGUIEnvironment* env, gui::IGUIElement* parent, s32 id,
    IMenuManager* menumgr, Client *client);
    ~Menu();

    SettingCategory current_category;
    void create();

    void close();

    void initCategoryButtons();

    void ItemsInit(SettingCategory category);

    void onCategoryButtonClick(SettingCategory category);

    virtual bool OnEvent(const irr::SEvent& event);

    virtual void draw();

    void updateScrollBarPosition(gui::IGUIScrollBar* scrollbar, int screenW, int screenH);
    
    std::vector<Setting> getSettings() {
        std::vector<Setting> settings;

        settings.push_back({"KeyStroker", "show_keys", SettingCategory::GUI});
        settings.push_back({"Show coords", "show_coords", SettingCategory::GUI});

        settings.push_back({"Fullbright", "fullbright", SettingCategory::RENDER});
        settings.push_back({"Water Effect", "small_post_effect_color", SettingCategory::RENDER});
        settings.push_back({"Node \n illumination", "node_illumination", SettingCategory::RENDER});
        settings.push_back({"Display sunrise", "display_sunrise", SettingCategory::RENDER});
        settings.push_back({"Disable stars", "disable_stars", SettingCategory::RENDER});
        settings.push_back({"Custom Fog", "use_custom_fog_color", SettingCategory::RENDER});
        settings.push_back({"Sky color","use_custom_sky_color", SettingCategory::RENDER});

        settings.push_back({"Fast place", "fast_place", SettingCategory::MISC, {Types::Boolean, 43, 124, "data"}});
        //settings.push_back({"Players HP", "enable_hp_bar", SettingCategory::RENDER});
        return settings;
    }

    std::wstring stringToWString(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }

private:
    IMenuManager* m_menumgr;
    Client* m_client;
    gui::IGUIEnvironment* env;

    video::IVideoDriver* driver = nullptr;

    bool isOpen = false;
    s32 screenW, screenH;
    std::vector<Button> buttons;
    std::vector<Items> items;

    gui::IGUIElement* parent;

    static Sprite_ coords_sprite;
    core::vector2d<s32> offset;
    core::rect<s32> pos;
    IGUIScrollBar* scrollbar;
    gui::IGUIFont* font = g_fontengine->getFont(FONT_SIZE_UNSPECIFIED, FM_Standard);

    int scrollbarTop;
};

#endif
