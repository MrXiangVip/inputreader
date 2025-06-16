//
// Created by xshx on 2025/6/16.
//

#ifndef INPUTREADER_MANGMICONFIG_H
#define INPUTREADER_MANGMICONFIG_H

#include <string>
#include <vector>
#include <map>

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
    std::vector<KeyConfig> getKeyConfigs(); // 返回策略配置
    std::vector<KeyConfig> getKeyConfigsByInputId(int inputId);
    std::vector<JoystickConfig> getJoystickConfigsFromId(int inputId);
    std::vector<KeyConfig>  getKeyConfigsByInputIdAndType(int inputId, int type);

    std::map<int, std::vector<KeyConfig>> getKeyConfigsMap();// 将全部配置按照 type 归类

private:
    GamepadConfig  gamePadConfig;
    static MangmiConfig *instance;
    MangmiConfig();

};

#endif //INPUTREADER_MANGMICONFIG_H
