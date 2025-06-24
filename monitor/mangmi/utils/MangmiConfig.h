//
// Created by xshx on 2025/6/16.
//

#ifndef INPUTREADER_MANGMICONFIG_H
#define INPUTREADER_MANGMICONFIG_H

#include <string>
#include <vector>
#include <map>

#define KEY_SLOT_INDEX 60
#define JOYSTICK_SLOT_INDEX 100

struct JoystickConfig {
    float centerX;
    float centerY;
    bool followPointerPosition;
    bool hideFromUi;
    int id;
    float minEffectiveRadiusPercent;
    float radius;
    bool reverseJoystickX;
    bool reverseJoystickY;
    float sensitivityX;
    float sensitivityY;
    int speed;
    int type;
    int slotId;//增加一个slotID 的字段
};

struct KeyConfig {
    int duration;
    bool exclusiveJoystick;
    int targetCode;
    int triggerType;
    int width;
    float centerX;
    float centerY;
    bool followPointerPosition;
    bool hideFromUi;
    int id;
    float minEffectiveRadiusPercent;
    float radius;
    bool reverseJoystickX;
    bool reverseJoystickY;
    float sensitivityX;
    float sensitivityY;
    int speed;
    int type;
    int slotId;//增加一个slotID 的字段
};
struct GamepadConfig {
    float alpha;
    std::string createdTime;
    std::string desc;
    std::string id;
    std::vector<JoystickConfig> joystickConfigs;
    std::vector<KeyConfig> keyConfigs;
    std::string packageName;
    std::string title;
    int toolbarLocation;
    int type;
    std::string updatedTime;
    int version;
};


class MangmiConfig {

public:

    static MangmiConfig *getInstance();
    bool parseJson(std::string  json);
    GamepadConfig& getGamepadConfig();

    std::map<int, std::vector<KeyConfig>> getKeyConfigsMapByInputId(int inputId);//  将全部配置 先按照inputId 再按照type分类
    std::map<int, std::vector<JoystickConfig>> getJoystickConfigsMapByInputId(int inputId);
    void releaseAllSlots();
private:
    GamepadConfig  gamePadConfig;
    static MangmiConfig *instance;
    MangmiConfig();

};

#endif //INPUTREADER_MANGMICONFIG_H
