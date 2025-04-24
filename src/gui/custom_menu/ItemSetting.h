#include <iostream>

enum SettingCategory {
    GUI,
    PVP,
    RENDER,
    MISC,
    NONE_C
};

enum Types {
    Text,
    Edit,
    Scroll,
    Boolean,
    NONE_T
};

struct Configuration
{
    Types t;
    float min, max;
    std::string save_data; //setting to set

    Configuration(Types t, float min, float max, std::string save_data):
    t(t), min(min), max(max), save_data(save_data) {}
};

struct Setting {
public:
    std::string name;
    std::string value;
    SettingCategory category;
    Configuration config;

    Setting() : name(""), value(""), category(SettingCategory::NONE_C), config(Types::NONE_T, 0, 0, "") {}
    Setting(std::string name, std::string value, SettingCategory category):
        name(name), value(value), category(category), config(Types::NONE_T, 0, 0, "") {}
    Setting(std::string name, std::string value, SettingCategory category, Configuration config):
        name(name), value(value), category(category), config(config) {}
};