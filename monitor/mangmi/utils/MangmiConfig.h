//
// Created by xshx on 2025/6/16.
//

#ifndef INPUTREADER_MANGMICONFIG_H
#define INPUTREADER_MANGMICONFIG_H

#include <string>
#include <vector>
#include <map>
#include <linux/input.h>
#include <sstream>
#include "../Macros.h"
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
    bool operator<(const JoystickConfig& other) const {
        if( type!= other.type){
            return type<other.type;
        }else if( id != other.id){
            return id< other.id;
        }else if( slotId != other.slotId){
            return slotId<other.slotId;
        }else if( centerX != other.centerX ){
            return centerX<other.centerX;
        }else if( centerY != other.centerY){
            return centerY <other.centerY;
        }else if( radius <other.radius){
            return  radius< other.radius;
        }
        return reverseJoystickX < other.reverseJoystickX;
    }
    bool operator==(const JoystickConfig& other) const {
        return type==other.type && id ==other.id && slotId==other.slotId &&
                    centerX==other.centerX &&centerY==other.centerY && radius==other.radius;
    }
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
    std::string toString() const {
        std::ostringstream oss;
        oss << "KeyConfig{"
            << "duration=" << duration
            << ", exclusiveJoystick=" << (exclusiveJoystick ? "true" : "false")
            << ", targetCode=" << targetCode
            << ", triggerType=" << triggerType
            << ", width=" << width
            << ", centerX=" << centerX
            << ", centerY=" << centerY
            << ", followPointerPosition=" << (followPointerPosition ? "true" : "false")
            << ", hideFromUi=" << (hideFromUi ? "true" : "false")
            << ", id=" << id
            << ", minEffectiveRadiusPercent=" << minEffectiveRadiusPercent
            << ", radius=" << radius
            << ", reverseJoystickX=" << (reverseJoystickX ? "true" : "false")
            << ", reverseJoystickY=" << (reverseJoystickY ? "true" : "false")
            << ", sensitivityX=" << sensitivityX
            << ", sensitivityY=" << sensitivityY
            << ", speed=" << speed
            << ", type=" << type
            << ", slotId=" << slotId
            << "}";
        return oss.str();
    }
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
