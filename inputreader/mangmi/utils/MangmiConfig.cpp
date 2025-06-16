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

std::vector<JoystickConfig> MangmiConfig::getJoystickConfigsFromId( int inputId ){
    std::vector<JoystickConfig> configs;
    int iJS = 0;

    iJS = gamePadConfig.joystickConfigs.size();
    if(iJS <= 0)
        return configs;

    for(int i = 0; i < iJS; ++i) {
        if(inputId == gamePadConfig.joystickConfigs[i].id) {
            configs.push_back( gamePadConfig.joystickConfigs[i]);
        }
    }
    //ALOGI("---getSlotIdFromIdConfig---ilen = %d", ilen);
    return configs;
}

/* 查出全部inputID 对应的config, 存在 一个inputId 对应多个config 的情况 */
std::vector<KeyConfig> MangmiConfig::getKeyConfigsByInputId(int inputId ){
    std::vector<KeyConfig> keyConfigs;
    for(auto& config :gamePadConfig.keyConfigs){
        if( config.id == inputId ){
            keyConfigs.push_back( config);
        }
    }
    return  keyConfigs;
}

std::vector<KeyConfig> MangmiConfig::getKeyConfigsByInputIdAndType( int inputId, int type){
    std::vector<KeyConfig>  retKeyConfig;
    for(int i=0; i<gamePadConfig.keyConfigs.size(); i++){
        if( inputId == gamePadConfig.keyConfigs[i].id &&
            type == gamePadConfig.keyConfigs[i].type){
            retKeyConfig.push_back( gamePadConfig.keyConfigs[i]);
        }
    }
    return retKeyConfig;
}

std::vector<KeyConfig> MangmiConfig::getKeyConfigs( ){
    ALOGD("getKeyConfigs");
    std::vector<KeyConfig>   keyConfigs;
    int iKey = gamePadConfig.keyConfigs.size();
    if( iKey <=0 ){
        return keyConfigs;
    }
    for(int i=0; i<iKey; i++){
        keyConfigs.push_back( gamePadConfig.keyConfigs[i] );
    }
    return  keyConfigs;
}


std::map<int , std::vector<KeyConfig>> MangmiConfig::getKeyConfigsMap() {
    std::map<int, std::vector<KeyConfig>>  keyConfigMap;

    for(const auto& keyConfig: gamePadConfig.keyConfigs){
        keyConfigMap[keyConfig.type].push_back(keyConfig);
    }
    return keyConfigMap;
}