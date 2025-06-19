/*
 *
 *Created by xshx on 2025/5/30.
 * 策略类
 * 职责是 处理事件的策略
*/


#include "MangmiPolicy.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <vector>
#include <set>
#include "json/json.h"
#include "utils/MangmiIntercepter.h"
#include "../ include/EventHub.h"
#include "utils/MangmiUtils.h"
#include "InputFilter.h"
#include "utils/MangmiSocketServer.h"

MangmiPolicy *MangmiPolicy::instance= nullptr;
std::vector<socketReply> MangmiPolicy::vectorReply;//定义
int MangmiPolicy::iSlotId=20;// iSlotId start from 20
std::vector<int> MangmiPolicy::jsLeftSlotId; //
std::vector<int> MangmiPolicy::jsRightSlotId;//
std::vector<IdConfig> MangmiPolicy::idConfigs;//
int MangmiPolicy::mWidth;
int MangmiPolicy::mHeight;
std::atomic<bool>  MangmiPolicy::AtomicComboThreadExit;//
MiThreadPool MangmiPolicy::mangmiPool;

socketReply::socketReply(int mRequestId,int mInputId){
    requestId = mRequestId;
    inputId =mInputId;
}

MangmiPolicy::MangmiPolicy() {
    ALOGD("init");
    mWidth = 800;
    mHeight =640;
    mangmiPool.setThreadCount(6); // set threadpool num is 6
    mangmiPool.createPool();
    AtomicComboThreadExit.store(false);
}

MangmiPolicy* MangmiPolicy::getInstance( ){
    if( instance == nullptr ){
        instance = new MangmiPolicy();
    }
    return instance;
}

pthread_t MangmiPolicy::startSocketServerThread( ){
    ALOGD("启动策略套接字线程 \n");
    pthread_t sThreadId;

    pthread_create(&sThreadId, nullptr, startMangmiSocket, this);
    return sThreadId;

}

void* MangmiPolicy::startMangmiSocket(void *args) {
    ALOGD("startMangmiSocket");
//    MangmiPolicy *self = static_cast<MangmiPolicy*>(args);
    int sockfd =MangmiSocketServer::getInstance()->startServer();
    if( sockfd ==-1){ALOGD("StartServer Failed");}
    return NULL;
}

int MangmiPolicy::updateIdConfigs( ){
    ALOGD("updateIdConfigs");
//
    GamepadConfig&  eventConfig= MangmiConfig::getInstance()->getGamepadConfig();
    int ilen = eventConfig.joystickConfigs.size();
    if( ilen >0 ){
        idConfigs.clear();
        jsLeftSlotId.clear();
        jsRightSlotId.clear();
        for(int i=0; i<ilen; i++){
            if( INPUT_ID_LEFT_JOYSTICK == eventConfig.joystickConfigs[i].id ){//left joystick
                if( JOYSTICK_TYPE_GAMEPAD_LEFT_JOYSTICK != eventConfig.joystickConfigs[i].type ){
                    assignIdConfig("joystickConfigs",   eventConfig.joystickConfigs[i].id + eventConfig.joystickConfigs[i].type);
                }

                if( JOYSTICK_TYPE_TOUCHSCREEN_ADJUST_VIEW != eventConfig.joystickConfigs[i].type ){
                    jsLeftSlotId.push_back( 80+i);
                }
            }else if( INPUT_ID_RIGHT_JOYSTICK == eventConfig.joystickConfigs[i].id ){// right joystick
                if( JOYSTICK_TYPE_GAMEPAD_RIGHT_JOYSTICK !=eventConfig.joystickConfigs[i].type ){
                    assignIdConfig("joystickConfigs", 8000+ eventConfig.joystickConfigs[i].id +eventConfig.joystickConfigs[i].type);

                }
                if( JOYSTICK_TYPE_TOUCHSCREEN_ADJUST_VIEW == eventConfig.joystickConfigs[i].type ){
                    jsRightSlotId.push_back( 90+i);
                }
            }

        }
    }

    ilen = eventConfig.keyConfigs.size();
    if( ilen >0 ){
        kSlotConfigs.clear();
        for(int i=0; i<ilen; i++){
            assignKeySlotConfig( eventConfig.keyConfigs[i].id, eventConfig.keyConfigs[i].type );
        }
    }
}

int MangmiPolicy::assignIdConfig(std::string str, int idAddType){
    IdConfig idconfig;
    iSlotId++;
    ALOGI("---assignIdConfig---idAddType = %d,  configStr = %s, iSlotId = %d", idAddType,  str.c_str(), iSlotId);

    idconfig.id = idAddType;
    idconfig.slot = iSlotId;
    idconfig.configStr = str;
    idConfigs.push_back(idconfig);
}

int MangmiPolicy::getSlotIdFromIdConfig(std::string str, int idAddType)
{
    int iSlot = 0;
    ALOGI("---getSlotIdFromIdConfig---str = %s, idAddType = %d", str.c_str(), idAddType);
    iSlot = -1;
    for (size_t i = 0; i < idConfigs.size(); ++i) {
        if ((idConfigs[i].id == idAddType) &&
            (idConfigs[i].configStr == str))
        {
            iSlot = idConfigs[i].slot;
            break;
        }
    }
    ALOGI("---getSlotIdFromIdConfig---iSlot = %d", iSlot);
    return iSlot;
}

void MangmiPolicy::assignKeySlotConfig(int inputId, int type)
{
    KeySlotConfig slotconfig;
    iSlotId++;
    ALOGI("---assignKeySlotConfig---inputId = %d, type = %d, iSlotId = %d", inputId, type, iSlotId);

    slotconfig.id = inputId;
    slotconfig.slot = iSlotId;
    slotconfig.type = type;

    kSlotConfigs.push_back(slotconfig);
}

std::vector<int> MangmiPolicy::getSlotIdFromKeySlotConfig(int inputId, int type) {
    ALOGD("getSlotIdFromKeySlotConfig %d, %d", inputId, type);
    std::vector<int> keySlots;
    for (size_t i = 0; i < kSlotConfigs.size(); ++i) {
        if ((kSlotConfigs[i].id == inputId) &&
            (kSlotConfigs[i].type == type))
        {
            keySlots.push_back(kSlotConfigs[i].slot);
        }
    }
    return keySlots;
}

int MangmiPolicy::replyApplication(int inputId, RawEvent event){
    ALOGD("回复客户端");
    int ilen = vectorReply.size();
    std::string sData;
    std::string sendData;
    int ret=0;
    for(int i=0; i<ilen; i++){//有 订阅才需要发socket
        if( inputId == vectorReply[i].inputId ){
            sData.clear();
            sData.append(std::to_string(inputId));
            sData.append("$");
            sData.append(std::to_string(event.value));
            sData.append("$");
            sData.append("0");

            sendData.clear();
            sendData.append(std::to_string(vectorReply[i].requestId));
            sendData.append("#");
            sendData.append("2881"); //接口ID
            sendData.append("#");
            sendData.append(std::to_string(sData.length()));
            sendData.append("#");
            sendData.append(sData);
            sendData.append("#@");
        }
        ret = replyClient( vectorReply[i].requestId, sendData);

    }
    return  ret;
}

int MangmiPolicy::replyClient(int requestId, std::string data){
    int iRet = 0;

//    RECONNECT:
//    if(false == key_socket_client.running)
//    {
//        // 客户端连接到服务器: 127.0.0.1 8088
//        key_socket_client.connectToServer("127.0.0.1");
//    }
//
//    // 发送消息到服务器
//    ALOGI("---%s---sendData = %s", __func__, data.c_str());
//    iRet = key_socket_client.sendMessage(data);
//    if(0 != iRet) {
//        ALOGI("---%s---iRet = %d, sendMessage error", __func__, iRet);
//        // 停止客户端
//        key_socket_client.stop();
//        goto RECONNECT;
//    }
//
//    iRet = key_socket_client.receiveMessage(requestId);
//    ALOGI("---%s---iRet = %d", __func__, iRet);
//    if(-1 == iRet)
//        ALOGI("---%s---iRet = %d, requestId error", __func__, iRet);
//
//    return iRet;
    MangmiSocketClient::getInstance()->connectToServer("127.0.0.1");
    MangmiSocketClient::getInstance()->sendMessage(data);
    iRet =MangmiSocketClient::getInstance()->receiveMessage( requestId);
    return iRet;
}

void MangmiPolicy::buildKeyEvent( RawEvent event){
    ALOGD("新生成一个事件 deviceId:%d, type:%d, code:%d, value:%d", event.deviceId, event.type, event.code, event.value);

    int inputId = MangmiUtils::getInputIdFromEvcode( event.code);
    std::map<int, std::vector<KeyConfig>> keyConfigMap =MangmiConfig::getInstance()->getKeyConfigsMapByInputId(inputId);

//  根据当前接到的配置 选择一个策略 来生成新的事件
    for( auto it=keyConfigMap.begin();it!=keyConfigMap.end();++it){
        //      每一个配置的类型对应一种 按键处理方式
        int type = it->first;
        std::vector<KeyConfig> keyConfigs = it->second;
        switch(type ){
            case KEY_CATEGORY_GAMEPAD://
                break;
            case KEY_TYPE_GAMEPAD_CLICK_STANDARD:// 处理成物理按键 按击事件
                convertToStandardKeyboardClick(event, KEY_TYPE_GAMEPAD_CLICK_STANDARD, keyConfigs);
                break;
            case KEY_TYPE_GAMEPAD_CLICK_TURBO: // 处理成物理按键 连续按击事件
                convertToStandardKeyboardComboClick( event, KEY_TYPE_GAMEPAD_CLICK_TURBO, keyConfigs);
                break;
            case KEY_CATEGORY_KEYBOARD://
                break;
            case KEY_CATEGORY_MOUSE:
                break;
            case KEY_CATEGORY_TOUCHSCREEN:
                break;
            case KEY_TYPE_TOUCHSCREEN_CLICK_STANDARD:// 屏幕点击
                convertToStandardTouchClick(event, KEY_TYPE_TOUCHSCREEN_CLICK_STANDARD,keyConfigs);
                break;
            case KEY_CATEGORY_MOBA:// 指向性技能
                break;
            case KEY_TYPE_MOBA_CANCEL_SKILL:// 取消技能
                break;
            case KEY_TYPE_MOBA_VIEW_MAP://看地图
                break;
            default:
                break;
        }
    }

    return;
}
void MangmiPolicy::buildAxisEvent( RawEvent event) {
    ALOGD("build new Event");
//  方向键， L2,R2，左右摇杆 都是abs事件
    switch( event.code ){
        case ABS_X:
        case ABS_Y:
            leftJoystick( event );
            break;
        case ABS_Z:
        case ABS_RZ:
            rightJoystick( event );
            break;
        case ABS_HAT0X:
        case ABS_HAT0Y:
            absHatXY( event);
            break;
        case ABS_GAS:
            absGas( event);
            break;
        case ABS_BREAK:
            absBreak( event);
            break;
        default:
            break;
    }
    return;

}

void MangmiPolicy::leftJoystick(RawEvent& event){
    ALOGD("leftJoystick");
    std::map<int, vector<JoystickConfig>> joystickMap = MangmiConfig::getInstance()->getJoystickConfigsMapByInputId( INPUT_ID_LEFT_JOYSTICK );// 一个左摇杆 可以配置多个策略
    for( auto it=joystickMap.begin();it!=joystickMap.end();++it){
        int type = it->first;
        std::vector<JoystickConfig> joystickConfigs = it->second;
        switch ( type){// 每一个类型 对应一种处理方式
            case JOYSTICK_CATEGORY_GAMEPAD:
                break;
            case JOYSTICK_TYPE_TOUCHSCREEN_JOYSTICK://虚拟摇杆
                gamePadLeftJoystick(event,JOYSTICK_TYPE_GAMEPAD_LEFT_JOYSTICK, joystickConfigs);
                break;
            case JOYSTICK_TYPE_GAMEPAD_RIGHT_JOYSTICK:
                break;
            case JOYSTICK_CATEGORY_KEYBOARD:
                break;
            case JOYSTICK_CATEGORY_MOUSE:
                break;
            case JOYSTICK_CATEGORY_TOUCHSCREEN:
                break;
            case JOYSTICK_TYPE_TOUCHSCREEN_SKILL_SHOT:// 指向性技能
                break;
            case JOYSTICK_TYPE_TOUCHSCREEN_ADJUST_VIEW://调整视角
                break;
            default:
                break;
        }
    }
    return;
}

void MangmiPolicy::rightJoystick(RawEvent event) {
    ALOGD("rightJoystick");
    std::map<int, vector<JoystickConfig>> joystickMap = MangmiConfig::getInstance()->getJoystickConfigsMapByInputId( INPUT_ID_RIGHT_JOYSTICK );// 一个左摇杆 可以配置多个策略
    for( auto it=joystickMap.begin();it!=joystickMap.end();++it){
        int type = it->first;
        std::vector<JoystickConfig> joystickConfigs = it->second;
        switch (type){// 每一个类型 对应一种处理方式
            case JOYSTICK_CATEGORY_GAMEPAD:
                break;
            case JOYSTICK_CATEGORY_KEYBOARD:
                break;
            case JOYSTICK_CATEGORY_MOUSE:
                break;
            case JOYSTICK_CATEGORY_TOUCHSCREEN:
                break;
            default:
                break;
        }
    }
    return;
}

bool isLeftDown =false;
#define DEADZONE_CENTERPOINT 5

void MangmiPolicy::gamePadLeftJoystick(RawEvent event, int type, std::vector<JoystickConfig> joystickConfigs){
    ALOGD("gamePadLeftJoystick");
    int leftOrigX, leftOrigY;
    int axisX, axisY;
    int leftAxisX, leftAxisY;
    if( event.code ==ABS_X){
        leftOrigY = event.value;
        axisY = mWidth - event.value; //swap x->y
        leftAxisY = axisY;
        axisX = leftAxisX;
    }else if( event.code ==ABS_Y){
        leftOrigX = event.value;
        axisX = event.value; //swap y->x
        leftAxisX = axisX;
        axisY = leftAxisY;
    }

    /*当摇杆从非死区移动到死区位置，或回到中心位置时，生成抬起事件*/
    if( (abs(leftOrigX)<DEADZONE_CENTERPOINT) && abs(leftOrigY)<DEADZONE_CENTERPOINT ){
        leftOrigX =0;
        leftOrigY =0;
//        isAxisLeftDown =false;
        isLeftDown =false;
        for(int i=0; i<joystickConfigs.size(); i++){
            int slotId = getSlotIdFromIdConfig("joystickConfigs", i + joystickConfigs[i].id+ joystickConfigs[i].type);
            InputFilter::getInstance()->pushSoftEvent(slotId, TOUCH_UP, 0, 0);
        }
    }else{
        if( isLeftDown ==false ){//
            isLeftDown =true;

            for(int i=0; i<joystickConfigs.size();i++){
                JoystickConfig joystickConfig = joystickConfigs[i];
                float fRadius = mWidth * joystickConfig.radius;
                float fSenX = joystickConfig.sensitivityX;
                float fSenY = joystickConfig.sensitivityY;
                float fCenterX = mWidth * joystickConfig.centerY;
                float fCenterY = mHeight - mHeight * joystickConfig.centerX;
                int x = (int)((double)axisX / 32768.0 * fRadius * fSenX + fCenterX);
                int y = (int)((double)axisY / 32768.0 * fRadius * fSenY + fCenterY);
                int slotId = getSlotIdFromIdConfig("joystickConfigs", i + joystickConfig.id+ joystickConfig.type);

                InputFilter::getInstance()->pushSoftEvent( slotId, TOUCH_DOWN, x, y);
            }
        }else{
            if( isLeftDown == true){
                for(int i=0; i<joystickConfigs.size();i++){
                    JoystickConfig joystickConfig = joystickConfigs[i];
                    int axisX = event.value;
                    int axisY = mWidth -event.value;
                    float fRadius = mWidth * joystickConfig.radius;
                    float fSenX = joystickConfig.sensitivityX;
                    float fSenY = joystickConfig.sensitivityY;
                    float fCenterX = mWidth * joystickConfig.centerY;
                    float fCenterY = mHeight - mHeight * joystickConfig.centerX;
                    int x = (int)((double)axisX / 32768.0 * fRadius * fSenX + fCenterX);
                    int y = (int)((double)axisY / 32768.0 * fRadius * fSenY + fCenterY);
                    int slotId = getSlotIdFromIdConfig("joystickConfigs", i + joystickConfig.id+ joystickConfig.type);

                    InputFilter::getInstance()->pushSoftEvent( slotId, TOUCH_MOVE, x, y);
                }
            }
        }
    }



}

void MangmiPolicy::absHatXY(RawEvent &event) {
    ALOGD("absHatXY deviceId:%d, type:%d, code:%d, value:%d", event.deviceId, event.type, event.code, event.value);
    int currentKey = 0;

    RawEvent newEvent;
    if (event.code == ABS_HAT0X && event.value == 1) {// 轴右键
        currentKey = DPAD_RIGHT;
        newEvent = generateEvent(event.deviceId, EV_KEY, DPAD_RIGHT, 1);
    } else if (event.code = ABS_HAT0X && event.value == -1) {//横轴左键
        currentKey = DPAD_LEFT;
        newEvent = generateEvent(event.deviceId, EV_KEY, DPAD_LEFT, 1);
    } else if (event.code == ABS_HAT0X && event.value == 0) {//
        if (currentKey == DPAD_LEFT) {
            newEvent = generateEvent(event.deviceId, EV_KEY, DPAD_LEFT, 0);
        } else if (currentKey == DPAD_RIGHT) {
            newEvent = generateEvent(event.deviceId, EV_KEY, DPAD_RIGHT, 0);
        }
        currentKey==0;
    } else if (event.code == ABS_HAT0Y && event.value == 1) {
        currentKey = DPAD_DOWN;
        newEvent = generateEvent(event.deviceId, EV_KEY, DPAD_DOWN, 1);
    } else if (event.code == ABS_HAT0Y && event.value == -1) {
        currentKey = DPAD_UP;
        newEvent = generateEvent(event.deviceId, EV_KEY, DPAD_UP, 1);
    } else if (event.code == ABS_HAT0Y && event.value == 0) {
        if (currentKey == DPAD_DOWN) {
            newEvent = generateEvent(event.deviceId, EV_KEY, DPAD_UP, 0);
        } else if (currentKey == DPAD_UP) {
            newEvent = generateEvent(event.deviceId, EV_KEY, DPAD_DOWN, 0);
        }
        currentKey=0;
    }
    buildKeyEvent(newEvent);
}

void MangmiPolicy::absGas(RawEvent &event){
    ALOGD("absGas ");
}

void MangmiPolicy::absBreak(RawEvent &event){
    ALOGD("absBreak");
}

RawEvent MangmiPolicy::generateEvent( int32_t deviceId, int type, int keyCode, int32_t value) {
    ALOGD("generateEvent");
    RawEvent event;
    event.deviceId = deviceId;
    event.type = type;
    int scanCode =MangmiUtils::getScanCodeFromKeyCode(keyCode);
    if( scanCode >0 ){
        keyCode = scanCode;
    }
    event.code = keyCode;
    event.value = value;
    return event;
}



void MangmiPolicy::convertToStandardTouchClick( RawEvent &event,int type, std::vector<KeyConfig> keyConfigs){
    ALOGD("convertToStandardTouchClick deviceId:%d, type:%d, code:%d, value:%d, sizeof keyConfig:%d", event.deviceId, event.type, event.code, event.value, keyConfigs.size());
    int tmpInputId= MangmiUtils::getInputIdFromEvcode( event.code);
    if( tmpInputId==0){return ;}
    std::vector<int>  slotIds =getSlotIdFromKeySlotConfig( tmpInputId, type );
    if( slotIds.size()==0){return;}
    for(int i=0; i<keyConfigs.size();i++){
        const auto &keyConfig = keyConfigs[i];
        float centerX = keyConfig.centerX;
        float centerY = keyConfig.centerY;
        if( event.value == 1){

            InputFilter::getInstance()->pushSoftEvent(slotIds[i], TOUCH_DOWN, centerX, centerY);
        }else if( event.value == 0){
            InputFilter::getInstance()->pushSoftEvent(slotIds[i], TOUCH_UP, centerX, centerY);
        }
    }
}


void MangmiPolicy::convertToStandardKeyboardClick( RawEvent &event,int type, std::vector<KeyConfig> keyConfigs) {
    ALOGD("convertToStandardKeyboardClick ");
    RawEvent  newEvent;

    for(const auto& keyconfig :keyConfigs){
        if( event.value ==1){
            newEvent = generateEvent(event.deviceId, EV_KEY, keyconfig.targetCode, event.value);
        }else if( event.value ==0){
            newEvent =generateEvent(event.deviceId, EV_KEY, keyconfig.targetCode, event.value);
        }
        InputFilter::getInstance()->pushSoftEvent( newEvent);
    }

    return;
}

void MangmiPolicy::convertToStandardKeyboardComboClick( RawEvent &event, int type, std::vector<KeyConfig> keyConfigs){
    ALOGD("convertToStandardKeyboardComboClick");
    for(auto keyConfig :keyConfigs ){
        if( event.value ==1 ){
            AtomicComboThreadExit.store(false);
            mangmiPool.push_task( keyBoardComboClick, event , keyConfig);
        }else if( event.value ==0){
            AtomicComboThreadExit.store(true);
        }
    }

    return;
}

void MangmiPolicy::keyBoardComboClick( RawEvent rawEvent, KeyConfig keyConfig){
    ALOGD("keyBoardComboClick");
    while(1){
        RawEvent downEvent =generateEvent( rawEvent.deviceId, rawEvent.type, keyConfig.targetCode, 1);
        InputFilter::getInstance()->pushSoftEvent( downEvent);
        std::this_thread::sleep_for( std::chrono::milliseconds(keyConfig.duration) );

        RawEvent  upEvent = generateEvent(rawEvent.deviceId, rawEvent.type, keyConfig.targetCode, 0);
        InputFilter::getInstance()->pushSoftEvent( upEvent);
        if( AtomicComboThreadExit.load() ){
            break;
        }
    }
}