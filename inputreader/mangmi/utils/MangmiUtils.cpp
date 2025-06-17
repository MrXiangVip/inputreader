//
// Created by xshx on 2025/5/26.
//

#include <vector>
#include "MangmiUtils.h"
#include "../Macro.h"

std::map<int, int> MangmiUtils::keyCodeScanCodeMap; // 定义

std::map<int, int> MangmiUtils::inputIdMaps; // 定义

 int MangmiUtils::initKeyCodeToScanCodeMap(){

    keyCodeScanCodeMap.clear();
    keyCodeScanCodeMap.insert({KEYCODE_BUTTON_A, BUTTON_A });//增加 keyCode 到 scanCode 映射关系
    keyCodeScanCodeMap.insert({KEYCODE_BUTTON_B, BUTTON_B });
    keyCodeScanCodeMap.insert({KEYCODE_BUTTON_X, BUTTON_X });
    keyCodeScanCodeMap.insert({KEYCODE_BUTTON_Y, BUTTON_Y });
    return  0;
}


int MangmiUtils::getScanCodeFromKeyCode( int keyCode){
    if( keyCodeScanCodeMap.size() ==0 ){ initKeyCodeToScanCodeMap( ) ;}
    auto it = keyCodeScanCodeMap.find( keyCode );
    if( it !=keyCodeScanCodeMap.end() ){
        int value = it->second;
        ALOGD("从keycode %d 获得 scancode %d", keyCode, value);
        return  value;

    }else{
        ALOGD("不需要从keycode %d 获得 scancode ", keyCode);
        return  0;
    }
}


int MangmiUtils::initInputIdMaps( ){
    ALOGD("initInputIdMaps");
    inputIdMaps.clear();
    inputIdMaps.insert({INPUT_ID_UP, DPAD_UP});
    inputIdMaps.insert({INPUT_ID_DOWN, DPAD_DOWN});
    inputIdMaps.insert({INPUT_ID_LEFT, DPAD_LEFT});
    inputIdMaps.insert({INPUT_ID_RIGHT, DPAD_RIGHT});

    inputIdMaps.insert({INPUT_ID_A, BUTTON_A});
    inputIdMaps.insert({INPUT_ID_B, BUTTON_B});
    inputIdMaps.insert({INPUT_ID_C, BUTTON_C});
    inputIdMaps.insert({INPUT_ID_X, BUTTON_X});
    inputIdMaps.insert({INPUT_ID_Y, BUTTON_Y});
    inputIdMaps.insert({INPUT_ID_Z, BUTTON_Z});
    inputIdMaps.insert({INPUT_ID_L1, BUTTON_L1});
    inputIdMaps.insert({INPUT_ID_L2, BUTTON_L2});
    inputIdMaps.insert({INPUT_ID_L3, BTN_THUMBL});

    inputIdMaps.insert({INPUT_ID_R1, BTN_TR});
    inputIdMaps.insert({INPUT_ID_R2, BTN_TR2});
    inputIdMaps.insert({INPUT_ID_R3, BTN_THUMBR});

    inputIdMaps.insert({INPUT_ID_SELECT, BTN_SELECT});
    inputIdMaps.insert({INPUT_ID_START, BTN_START});
    inputIdMaps.insert({INPUT_ID_HOME, BTN_MODE});

    inputIdMaps.insert({INPUT_ID_BACK, KEY_BACK});
    inputIdMaps.insert({INPUT_ID_MENU, KEY_MENU});
 }


 int  MangmiUtils::getInputIdFromEvcode(int evCode ){
     int inputIds=0;
     if( inputIdMaps.size() ==0){return inputIds;}
     for( const auto&pair : inputIdMaps){
         if( pair.second == evCode ){
             inputIds =  pair.first;
             return inputIds;
         }
     }
     return inputIds;
 }

 int MangmiUtils::getEvcodeFromInputId(int inputId){
     ALOGD("getInputIdFromEvcode %d", inputId);
     auto it = inputIdMaps.find(inputId);
     if( it != inputIdMaps.end() ){
         int value = it->second;
         return value;
     }else{
         return -1;
     }
 }


long long MangmiUtils::getSystemTimePosix() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;  // 微秒
}


bool MangmiUtils::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    size_t i = 0;
    // 处理正负号
    if (str[0] == '+' || str[0] == '-') {
        if (str.length() == 1) return false;
        i = 1;
    }
    // 检查剩余字符是否全为数字
    for (; i < str.length(); i++) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}