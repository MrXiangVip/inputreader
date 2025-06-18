//
// Created by xshx on 2025/6/16.
//

#include "MangmiConfig.h"
#include "../json/json.h"
#include "../Macro.h"

MangmiConfig* MangmiConfig::instance= NULL;

MangmiConfig::MangmiConfig() {
    ALOGD("make MangmiConfig");
}

MangmiConfig * MangmiConfig::getInstance() {
    if(instance ==NULL){
        instance = new MangmiConfig();
    }
    return instance;
}

bool MangmiConfig::parseJson(std::string  sJson){
    ALOGD("parseJson %s",sJson.c_str());
    Json::Reader reader;
    Json::Value root;

    // 尝试解析 JSON 数据
    if (!reader.parse(sJson, root)) {
        // 如果解析失败，输出错误信息
        ALOGI("Failed to parse JSON: %s", reader.getFormattedErrorMessages().c_str());
        return -1;
    }

    // 解析 GamepadConfig
    gamePadConfig.alpha = root["alpha"].asFloat();
    gamePadConfig.createdTime = root["createdTime"].asString();
    gamePadConfig.desc = root["desc"].asString();
    gamePadConfig.id = root["id"].asString();
    gamePadConfig.packageName = root["packageName"].asString();
    gamePadConfig.title = root["title"].asString();
    gamePadConfig.toolbarLocation = root["toolbarLocation"].asInt();
    gamePadConfig.type = root["type"].asInt();
    gamePadConfig.updatedTime = root["updatedTime"].asString();
    gamePadConfig.version = root["version"].asInt();

    // 解析 JoystickConfigs
    gamePadConfig.joystickConfigs.clear();
    const Json::Value& joystickConfigs = root["joystickConfigs"];
    if(joystickConfigs.isArray() && joystickConfigs.empty())
    {
        ALOGI("---%s---joystickConfigs is null", __func__);
    }else {
        ALOGI("---%s--- 处理 joystickConfigs ", __func__);
        for (const auto& item : joystickConfigs) {
            JoystickConfig joystickConfig;
            joystickConfig.centerX = item["centerX"].asFloat();
            joystickConfig.centerY = item["centerY"].asFloat();
            joystickConfig.followPointerPosition = item["followPointerPosition"].asBool();
            joystickConfig.hideFromUi = item["hideFromUi"].asBool();
            joystickConfig.id = item["id"].asInt();
            joystickConfig.minEffectiveRadiusPercent = item["minEffectiveRadiusPercentage"].asFloat();
            joystickConfig.radius = item["radius"].asFloat();
            joystickConfig.reverseJoystickX = item["invertJoystickX"].asBool();
            joystickConfig.reverseJoystickY = item["invertJoystickY"].asBool();
            joystickConfig.sensitivityX = item["sensitivityX"].asFloat();
            joystickConfig.sensitivityY = item["sensitivityY"].asFloat();
            joystickConfig.speed = item["speed"].asInt();
            joystickConfig.type = item["type"].asInt();
            gamePadConfig.joystickConfigs.push_back(joystickConfig);
        }
    }

    // 解析 KeyConfigs
    gamePadConfig.keyConfigs.clear();
    const Json::Value& keyConfigs = root["keyConfigs"];
    if(keyConfigs.isArray() && keyConfigs.empty())
    {
        ALOGI("---%s---keyConfigs is null", __func__);
    }else {
        ALOGI("---%s--- 处理 keyConfigs ", __func__);
        for (const auto& item : keyConfigs) {
            KeyConfig keyConfig;
            keyConfig.duration = item["duration"].asInt();
            keyConfig.exclusiveJoystick = item["exclusiveJoystick"].asBool();
            keyConfig.targetCode = item["targetCode"].asInt();
            keyConfig.triggerType = item["triggerType"].asInt();
            keyConfig.width = item["width"].asInt();
            keyConfig.centerX = item["centerX"].asFloat();
            keyConfig.centerY = item["centerY"].asFloat();
            keyConfig.followPointerPosition = item["followPointerPosition"].asBool();
            keyConfig.hideFromUi = item["hideFromUi"].asBool();
            keyConfig.id = item["id"].asInt();
            keyConfig.minEffectiveRadiusPercent = item["minEffectiveRadiusPercentage"].asFloat();
            keyConfig.radius = item["radius"].asFloat();
            keyConfig.reverseJoystickX = item["invertJoystickX"].asBool();
            keyConfig.reverseJoystickY = item["invertJoystickY"].asBool();
            keyConfig.sensitivityX = item["sensitivityX"].asFloat();
            keyConfig.sensitivityY = item["sensitivityY"].asFloat();
            keyConfig.speed = item["speed"].asInt();
            keyConfig.type = item["type"].asInt();
            gamePadConfig.keyConfigs.push_back(keyConfig);
        }
    }
    return 0;
}

GamepadConfig& MangmiConfig::getGamepadConfig( ) {
    return gamePadConfig;
}


/*  将inputId符合的keyConfig 按照type 为key ， keyconfig列表为值重新组合 */
std::map<int, std::vector<KeyConfig>> MangmiConfig::getKeyConfigsMapByInputId(int inputId){
    std::map<int, std::vector<KeyConfig>>  keyConfigMap;
    for(const auto& keyConfig: gamePadConfig.keyConfigs){
        if( keyConfig.id == inputId){
            keyConfigMap[keyConfig.type].push_back(keyConfig);
        }
    }
    return keyConfigMap;
}

/* 将inputId符合的JoystickConfig ,按照type 为key ， JoystickConfig 列表为值重新组合  */
std::map<int, std::vector<JoystickConfig>> MangmiConfig::getJoystickConfigsMapByInputId(int inputId){
    std::map<int, std::vector<JoystickConfig>> joystickConfigMap;
    for(const auto &joystickConfig:gamePadConfig.joystickConfigs){
        if(joystickConfig.id == inputId ){
            joystickConfigMap[joystickConfig.type].push_back( joystickConfig);
        }
    }
    return joystickConfigMap;
}



