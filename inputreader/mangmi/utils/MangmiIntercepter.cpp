/*
 *
 * Created by xshx on 2025/5/29.
 * 职责是处理拦截逻辑
*/




#include <cstdio>
#include <vector>
#include "MangmiIntercepter.h"
#include "../Macro.h"
#include "../InputFilter.h"
#include "../MangmiPolicy.h"

std::set<InterceptKey> MangmiIntercepter::interceptKeySet; // 定义
std::set<int> MangmiIntercepter::keySet;
std::set<int> MangmiIntercepter::axisSet;

int MangmiIntercepter::insertInterceptEventKey(InterceptKey key) {
    ALOGD("%s, evCode:%d, serialNo:%d, type:%d\n", __func__, key.evCode, key.serialNo, key.type);
    interceptKeySet.insert( key );
    return  0;
}


int MangmiIntercepter::eraseInterceptEventBySerialNo(int serialNo) {
    ALOGD("%s, %d, size:%d\n", __func__, serialNo, interceptKeySet.size());
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
    ALOGD("%s, %d,%d,%d size:%d\n", __func__, key.serialNo, key.type, key.evCode, interceptKeySet.size());
    if( interceptKeySet.size() >0){
        for( auto it = interceptKeySet.begin(); it!=interceptKeySet.end();){
            if( it->serialNo == key.serialNo && it->evCode==key.evCode && it->type==key.type){
                it = interceptKeySet.erase( it);
            }else{
                ++it;
            }
        }
        return 1;
    }
    return 0;
}

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

std::set<int> MangmiIntercepter::getInterceptEventByAxisType() {
    std::set<int> keySet;
    if (interceptKeySet.size() > 0) {
        for (auto it = interceptKeySet.begin(); it != interceptKeySet.end();) {
            if( it->type == EV_ABS){
                keySet.insert( it->evCode );
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
            InterceptKey key(serialNo, evCode, EV_KEY);
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
            InterceptKey key(serialNo, evCode, EV_ABS);
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

void MangmiIntercepter::enableInputFilter( const std::set<int>& keyCodes, const std::set<int>& axisCodes ){
    InputFilter::getInstance()->setInputsFilter( keySet, axisSet);
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
                    InterceptKey interceptKeyX = InterceptKey(serialNo, ABS_X, EV_ABS);
                    InterceptKey interceptKeyY = InterceptKey(serialNo, ABS_Y, EV_ABS);
                    insertInterceptEventKey(interceptKeyX);
                    insertInterceptEventKey(interceptKeyY);
                }
            } else if (joystickConfig.id == INPUT_ID_RIGHT_JOYSTICK) {
                if (JOYSTICK_TYPE_GAMEPAD_RIGHT_JOYSTICK != joystickConfig.type) { //
                    InterceptKey interceptKeyZ = InterceptKey(serialNo, ABS_Z, EV_ABS);
                    InterceptKey interceptKeyRZ = InterceptKey(serialNo, ABS_RZ, EV_ABS);
                    insertInterceptEventKey(interceptKeyZ);
                    insertInterceptEventKey(interceptKeyRZ);
                }

            }
        }
//      屏蔽按键
        std::vector<KeyConfig> keyConfigs = gamepadConfig.keyConfigs;
        for(auto& keyConfig :keyConfigs){
            InterceptKey interceptKey = InterceptKey( serialNo, keyConfig.id, EV_KEY);
            insertInterceptEventKey( interceptKey );
        }

    }else {
        ALOGD("ERROR SERIAL NO %d",serialNo);
    }
}


void MangmiIntercepter::toString() {
    ALOGD("toString size:%d\n", interceptKeySet.size());
    if( interceptKeySet.size()>0 ){
        for( auto it =interceptKeySet.begin(); it!=interceptKeySet.end();){
            ALOGD("evCode:%d, serialNo:%d, type:%d \n",it->evCode, it->serialNo, it->type);
            it++;
        }
    }
}


