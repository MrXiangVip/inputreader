/*
 *
 * Created by xshx on 2025/5/29.
 * 职责是处理拦截逻辑
*/




#include <cstdio>
#include <vector>
#include "MangmiIntercepter.h"
#include "../Macros.h"
#include "../InputFilter.h"
#include "../MangmiPolicy.h"

std::set<InterceptKey> MangmiIntercepter::interceptKeySet; // 定义
//std::set<int> MangmiIntercepter::keySet;
//std::set<int> MangmiIntercepter::axisSet;

MangmiIntercepter *MangmiIntercepter::instance;
MangmiIntercepter *MangmiIntercepter::getInstance() {
    if( instance ==NULL){
        instance = new MangmiIntercepter();
    }
    return instance;
}


int MangmiIntercepter::insertInterceptEventKey(InterceptKey key) {
    ALOGD("%s, evCode:%d, serialNo:%d, type:%d, value:%d\n", __func__, key.evCode, key.serialNo, key.type, key.value);
    interceptKeySet.insert( key );
    return  0;
}

/* 将serialNo 相同的集合删除 */
int MangmiIntercepter::eraseInterceptEventBySerialNo(int serialNo) {
    ALOGD("%s, %d, size:%d\n", __func__, serialNo, (int)interceptKeySet.size());
    if( interceptKeySet.size() >0){
        for( auto it = interceptKeySet.begin(); it!=interceptKeySet.end();){
            if( it->serialNo == serialNo){
                it = interceptKeySet.erase( it);
            }else{
                ++it;
            }
        }
        return 1;
    }
    return 0;
}

int MangmiIntercepter::eraseInterceptEventKeyByInterceptKey(InterceptKey key) {
    ALOGD("%s, %d,%d,%d size:%d\n", __func__, key.serialNo, key.type, key.evCode, (int)interceptKeySet.size());
    if( interceptKeySet.size() >0){
        for( auto it = interceptKeySet.begin(); it!=interceptKeySet.end();){
            if( it->serialNo == key.serialNo && it->evCode==key.evCode && it->type==key.type &&it->value ==key.value){
                it = interceptKeySet.erase( it);
            }else{
                ++it;
            }
        }
        return 1;
    }
    return 0;
}

/* 查出 类型是按键的集合 */
std::set<int> MangmiIntercepter::getInterceptEventByKeyType( ) {
    std::set<int> keySet;
    if (interceptKeySet.size() > 0) {
        for (auto it = interceptKeySet.begin(); it != interceptKeySet.end();) {
            if( it->type == EV_KEY){
                keySet.insert( it->evCode );
            }
            ++it;
        }
    }
    return  keySet;
}

/* 查出类型是摇杆的集合 */
//std::set<int> MangmiIntercepter::getInterceptEventByAxisType() {
std::map<int,int> MangmiIntercepter::getInterceptEventByAxisType() {
    std::map<int, int> keySet;
    if (interceptKeySet.size() > 0) {
        for (auto it = interceptKeySet.begin(); it != interceptKeySet.end();) {
            if( it->type == EV_ABS){
                keySet.insert( {it->evCode, it->value });
            }
            ++it;
        }
    }
    return  keySet;
}

int MangmiIntercepter::insertInterceptKeyEventWithSerialNo(std::set<int> keySet, int serialNo) {

    ALOGD("%s ", __func__);
    if(keySet.size() > 0)
    {
        for(const auto& evCode : keySet)
        {
            InterceptKey key(serialNo, evCode, EV_KEY,0);
            interceptKeySet.insert(key);
        }
        return 1;
    }
    return 0;
}

int MangmiIntercepter::insertInterceptAxisEventWithSerialNo(std::set<int> keySet, int serialNo) {

    ALOGD("%s ", __func__);
    if(keySet.size() > 0)
    {
        for(const auto& evCode : keySet)
        {
            InterceptKey key(serialNo, evCode, EV_ABS,0);
            interceptKeySet.insert(key);
        }
        return 1;
    }
    return 0;
}
int MangmiIntercepter::clearInputFilter( ){
    interceptKeySet.clear();
    return 0;
}

//void MangmiIntercepter::enableInputsFilter( const std::set<int>& keyCodes, const std::set<int>& axisCodes ){
void MangmiIntercepter::enableInputsFilter( const std::set<int>& keyCodes, const std::map<int,int>& axisCodes ){
    InputFilter::getInstance()->setInputsFilter( keyCodes, axisCodes);
    return ;
}

/*  根据下发的策略设置拦截 */
void MangmiIntercepter::buildIntercepterByConfigs( GamepadConfig& gamepadConfig, int serialNo ){
    if( serialNo == 1880){
//       先除,再设置屏蔽
        eraseInterceptEventBySerialNo( 1880);
        //   屏蔽摇杆
        std::vector<JoystickConfig> joystickConfigs= gamepadConfig.joystickConfigs;
        for(auto& joystickConfig :joystickConfigs) {
            if (joystickConfig.id == INPUT_ID_LEFT_JOYSTICK) {
                if (JOYSTICK_TYPE_GAMEPAD_LEFT_JOYSTICK != joystickConfig.type) {
                    InterceptKey interceptKeyX = InterceptKey(serialNo, ABS_X, EV_ABS,0);
                    InterceptKey interceptKeyY = InterceptKey(serialNo, ABS_Y, EV_ABS,0);
                    insertInterceptEventKey(interceptKeyX);
                    insertInterceptEventKey(interceptKeyY);
                }
            } else if (joystickConfig.id == INPUT_ID_RIGHT_JOYSTICK) {
                if (JOYSTICK_TYPE_GAMEPAD_RIGHT_JOYSTICK != joystickConfig.type) { //
                    InterceptKey interceptKeyZ = InterceptKey(serialNo, ABS_Z, EV_ABS,0);
                    InterceptKey interceptKeyRZ = InterceptKey(serialNo, ABS_RZ, EV_ABS,0);
                    insertInterceptEventKey(interceptKeyZ);
                    insertInterceptEventKey(interceptKeyRZ);
                }

            }
        }
//      屏蔽按键
        std::vector<KeyConfig> keyConfigs = gamepadConfig.keyConfigs;
        for(auto& keyConfig :keyConfigs){
//          方向键应该 加到摇杆屏蔽集合里,   L2,R2 也是摇杆类型 ,左右 方向键的evCode相同, 但value 不相同, 上下方向键的evCode 相同
            if( keyConfig.id == INPUT_ID_LEFT  ){
                InterceptKey interceptArrowKey = InterceptKey( serialNo, ABS_HAT0X, EV_ABS,-1);
                insertInterceptEventKey( interceptArrowKey);
            }else if( keyConfig.id ==INPUT_ID_RIGHT){
                InterceptKey interceptArrowKey = InterceptKey( serialNo, ABS_HAT0X, EV_ABS,1);
                insertInterceptEventKey( interceptArrowKey);
            }else if( keyConfig.id == INPUT_ID_UP){
                InterceptKey interceptArrowKey = InterceptKey( serialNo, ABS_HAT0Y, EV_ABS,1);
                insertInterceptEventKey( interceptArrowKey);
            }else if( keyConfig.id ==INPUT_ID_DOWN) {
                InterceptKey interceptArrowKey = InterceptKey( serialNo, ABS_HAT0Y, EV_ABS,-1);
                insertInterceptEventKey( interceptArrowKey);
            }else if(keyConfig.id ==INPUT_ID_R2) {
                InterceptKey interceptArrowKey = InterceptKey( serialNo, ABS_GAS, EV_ABS,0);
                insertInterceptEventKey( interceptArrowKey);
            }else if(keyConfig.id==INPUT_ID_L2 ){
                InterceptKey interceptArrowKey = InterceptKey( serialNo, ABS_BRAKE, EV_ABS,0);
                insertInterceptEventKey( interceptArrowKey);
            }else{//
                int evCode = MangmiUtils::getEvcodeFromInputId(keyConfig.id);
                InterceptKey interceptKey = InterceptKey( serialNo, evCode, EV_KEY, 0);
                insertInterceptEventKey( interceptKey );
            }
        }

    }else {
        ALOGD("ERROR SERIAL NO %d",serialNo);
    }
    toString();
    return;
}
int MangmiIntercepter::updateInterceptPolicy() {
    ALOGD("updateInterceptPolicy");

//    更新拦截策略
    buildIntercepterByConfigs(MangmiConfig::getInstance()->getGamepadConfig(), 1880);
    std::set<int> keySet = MangmiIntercepter::getInterceptEventByKeyType();
//    std::set<int> axisSet =MangmiIntercepter::getInterceptEventByAxisType();
    std::map<int,int> axisSet =MangmiIntercepter::getInterceptEventByAxisType();
    enableInputsFilter(keySet, axisSet);
    return 0;
}

void MangmiIntercepter::toString() {
    ALOGD("toString size:%d\n", (int)interceptKeySet.size());
    if( interceptKeySet.size()>0 ){
        for( auto it =interceptKeySet.begin(); it!=interceptKeySet.end();){
            ALOGD("evCode:%d, serialNo:%d, type:%d , value:%d\n",it->evCode, it->serialNo, it->type, it->value);
            it++;
        }
    }
}


