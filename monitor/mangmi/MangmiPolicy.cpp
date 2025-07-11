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
#include "../include/EventHub.h"
#include "utils/MangmiUtils.h"
#include "InputFilter.h"
#include "utils/MangmiSocketServer.h"

MangmiPolicy *MangmiPolicy::instance= nullptr;
std::vector<socketReply> MangmiPolicy::vectorReply;//定义
int MangmiPolicy::mWidth;
int MangmiPolicy::mHeight;
std::atomic<bool>  MangmiPolicy::AtomicComboThreadExit;//
std::atomic<bool>  MangmiPolicy::AtomicScreenComboThreadExit;//
MiThreadPool MangmiPolicy::mangmiPool;

socketReply::socketReply(int mRequestId,int mInputId){
    requestId = mRequestId;
    inputId =mInputId;
}

MangmiPolicy::MangmiPolicy() {
    ALOGD("init");
    mWidth = InputFilter::mWidth;
    mHeight =InputFilter::mHeight;
    mangmiPool.setThreadCount(6); // set threadpool num is 6
    mangmiPool.createPool();
    AtomicComboThreadExit.store(false);
    AtomicScreenComboThreadExit.store(false);
    iCancelSkill = false;//
}

MangmiPolicy* MangmiPolicy::getInstance( ){
    if( instance == nullptr ){
        instance = new MangmiPolicy();
    }
    return instance;
}

void MangmiPolicy::stop() {
    MangmiSocketServer::getInstance()->stop();
    ALOGD("Stop MangmiPolicy");
    return;
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

    MangmiSocketClient::getInstance()->connectToServer("127.0.0.1");
    MangmiSocketClient::getInstance()->sendMessage(data);
    iRet =MangmiSocketClient::getInstance()->receiveMessage( requestId);
    return iRet;
}

/* */
RawEvent MangmiPolicy::generateEvent( int32_t deviceId, int type, int keyCode, int32_t value) {
    ALOGD("generateEvent");
    RawEvent event;
    event.deviceId = deviceId;
    event.type = type;
//    int scanCode =MangmiUtils::getScanCodeFromKeyCode(keyCode);
//    if( scanCode >0 ){
//        keyCode = scanCode;
//    }
    event.code = keyCode;
    event.value = value;
    return event;
}

void MangmiPolicy::handleKeyPlus( RawEvent event){
    ALOGD("新生成一个事件 deviceId:%d, type:%d, code:%d, value:%d", event.deviceId, event.type, event.code, event.value);

    int inputId = MangmiUtils::getInputIdFromEvcode( event.code);
    std::map<int, std::vector<KeyConfig>> keyConfigMap =MangmiConfig::getInstance()->getKeyConfigsMapByInputId(inputId);
    if(keyConfigMap.size()==0){ return;}
//  根据当前接到的配置 选择一个策略 来生成新的事件
    for( auto it=keyConfigMap.begin();it!=keyConfigMap.end();++it){
        //      每一个配置的类型对应一种 按键处理方式
        int type = it->first;
        std::vector<KeyConfig> keyConfigs = it->second;
        switch(type ){
            case KEY_CATEGORY_GAMEPAD://
                break;
            case KEY_TYPE_GAMEPAD_CLICK_STANDARD:// 处理成物理按键 按击事件
                standardKeyClick(event,  keyConfigs);
                break;
            case KEY_TYPE_GAMEPAD_CLICK_TURBO: // 处理成物理按键 连续按击事件
                standardKeyComboClick( event,  keyConfigs);
                break;
            case KEY_CATEGORY_KEYBOARD://
                break;
            case KEY_CATEGORY_MOUSE:
                break;
            case KEY_CATEGORY_TOUCHSCREEN:
                break;
            case KEY_TYPE_TOUCHSCREEN_CLICK_STANDARD:// 屏幕点击--标准点击,
                touchScreensStandardClickWhenPress(event, keyConfigs);
                break;
            case KEY_TYPE_TOUCHSCREEN_CLICK_TURBO:// 屏幕点击--连发
                touchScreenComboClickWhenPress( event, keyConfigs);
                break;
            case KEY_TYPE_TOUCHSCREEN_CLICK_WHEN_PRESS:// 屏幕点击--极速点击
                touchScreenFastComboClickWhenPress( event, keyConfigs);
                break;
            case KEY_TYPE_TOUCHSCREEN_CLICK_WHEN_RELEASE://屏幕点击--松开点击
                touchScreenClickWhenRelease( event, keyConfigs);
            case KEY_CATEGORY_MOBA:// 指向性技能
                break;
            case KEY_TYPE_MOBA_LEFT_JOYSTICK_ASSOCIATION:// 指向性技能 --摇杆关联--左摇杆
                mobaAssociateLeftJoystickWhenPress( event, keyConfigs);
                break;
            case KEY_TYPE_MOBA_RIGHT_JOYSTICK_ASSOCIATION://指向性技能--摇杆关联--右摇杆
                mobaAssociateRightJoystickWhenPress( event, keyConfigs);
                break;
            case KEY_TYPE_MOBA_SMART_JOYSTICK_ASSOCIATION://指向性技能--摇杆关联--智能
                mobaAssociateSmartJoystickWhenPress( event, keyConfigs);
                break;
            case KEY_TYPE_MOBA_CANCEL_SKILL:// 取消技能
                mobaCancelSkillWhenPress( event, keyConfigs);//
                break;
            case KEY_TYPE_MOBA_VIEW_MAP://看地图
                mobaViewMapWhenPress( event, keyConfigs);
                break;
            default:
                break;
        }
    }

    return;
}

/* 标准按键点击在原生模式下触发 , xbox模式和switch模式 需要根据配置下发的targetCode找到 对应的scancode */
void MangmiPolicy::standardKeyClick( RawEvent &event, std::vector<KeyConfig> keyConfigs) {
    ALOGD("standardKeyClick ");
    RawEvent  newEvent;
    for(const auto& keyconfig :keyConfigs){
        int scanCode = MangmiUtils::getScanCodeFromKeyCode( keyconfig.targetCode);
        newEvent = generateEvent(event.deviceId, EV_KEY, scanCode, event.value);
        InputFilter::getInstance()->pushSoftEvent( newEvent);
    }

    return;
}
/* 原生模式 --连发*/
void MangmiPolicy::standardKeyComboClick( RawEvent &event,  std::vector<KeyConfig> keyConfigs){
    ALOGD("standardKeyComboClick");
    if( event.value ==1 ){//按下
        AtomicComboThreadExit.store(false);
        for(auto keyConfig :keyConfigs ){
            mangmiPool.push_task( keyBoardComboClick, event , keyConfig);
        }
    }else if( event.value ==0){// 抬起
        AtomicComboThreadExit.store(true);
    }
    return;
}

void MangmiPolicy::keyBoardComboClick( RawEvent rawEvent, KeyConfig keyConfig){
    ALOGD("keyBoardComboClick");
    while(1){
        int scanCode = MangmiUtils::getScanCodeFromKeyCode( keyConfig.targetCode);
        RawEvent downEvent =generateEvent( rawEvent.deviceId, rawEvent.type, scanCode, 1);
        InputFilter::getInstance()->pushSoftEvent( downEvent);
        std::this_thread::sleep_for( std::chrono::milliseconds(keyConfig.duration) );

        RawEvent  upEvent = generateEvent(rawEvent.deviceId, rawEvent.type, scanCode, 0);
        InputFilter::getInstance()->pushSoftEvent( upEvent);
        if( AtomicComboThreadExit.load() ){
            break;
        }
    }
}
/* 屏幕映射模式 -- 屏幕点击-- 标准点击 */
void MangmiPolicy::touchScreensStandardClickWhenPress( RawEvent &event, std::vector<KeyConfig> keyConfigs){
    ALOGD("touchScreensStandardClickWhenPress deviceId:%d, type:%d, code:%d, value:%d, sizeof keyConfig:%d", event.deviceId, event.type, event.code, event.value, (int)keyConfigs.size());
    int tmpInputId= MangmiUtils::getInputIdFromEvcode( event.code);
    if( tmpInputId==0){return ;}
    if( keyConfigs.size()==0){return;}
    int mWidth = InputFilter::mWidth;
    int mHeight  =InputFilter::mHeight;
    for(size_t i=0; i<keyConfigs.size();i++){
        const auto keyConfig = keyConfigs[i];
        ALOGD("%s, width:%d, height:%d", keyConfig.toString().c_str(), mWidth, mHeight);
        float fCenterX =  mWidth* keyConfig.centerY;
        float fCenterY = mHeight - mHeight * keyConfig.centerX;
        if( event.value == 1){
//            InputFilter::getInstance()->pushSoftEvent(slotIds[i], TOUCH_DOWN, centerX, centerY);
            ALOGD("TOUCH_DOWN slotId:%d, x:%f, y:%f", keyConfig.slotId, fCenterX, fCenterY);
            InputFilter::getInstance()->pushSoftEvent(keyConfig.slotId, TOUCH_DOWN, fCenterX, fCenterY);
        }else if( event.value == 0){
//            InputFilter::getInstance()->pushSoftEvent(slotIds[i], TOUCH_UP, centerX, centerY);
            ALOGD("TOUCH_UP slotId:%d, x:%f, y:%f", keyConfig.slotId, fCenterX, fCenterY);
            InputFilter::getInstance()->pushSoftEvent(keyConfig.slotId, TOUCH_UP, fCenterX, fCenterY);
        }
    }
    return;
}
/* 屏幕映射模式 -- 屏幕点击 -- 连击 */
void MangmiPolicy::touchScreenComboClickWhenPress(RawEvent &event, std::vector<KeyConfig> keyConfigs){
    ALOGD("touchScreenComboClickWhenPress ");
    if( event.value ==1 ){// 按下
        AtomicScreenComboThreadExit.store(false);
        for( auto config:keyConfigs ){
            mangmiPool.push_task( screenComboClick, event, config);
        }
    }else if( event.value ==0){// 抬起
        AtomicScreenComboThreadExit.store(true);

    }
    return;
}
/*  屏幕映射模式 -- 屏幕点击 -- 连击  的实现*/
void MangmiPolicy::screenComboClick( RawEvent rawEvent, KeyConfig keyConfig){
    ALOGD("keyBoardComboClick");
    while(1){
        float fCenterX = mWidth*keyConfig.centerY;
        float fCenterY = mHeight - mHeight*keyConfig.centerX;
        InputFilter::getInstance()->pushSoftEvent( keyConfig.slotId, TOUCH_DOWN,  fCenterX, fCenterY );
        std::this_thread::sleep_for(std::chrono::milliseconds( keyConfig.duration));
        InputFilter::getInstance()->pushSoftEvent(keyConfig.slotId, TOUCH_UP, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds( keyConfig.duration));
        if( AtomicScreenComboThreadExit.load()){
            break;
        }
    }
    return;
}
/* 屏幕映射模式 -- 屏幕点击-- 极速点击 */
void MangmiPolicy::touchScreenFastComboClickWhenPress( RawEvent rawEvent, std::vector<KeyConfig> keyConfigs){
    ALOGD("touchScreenFastComboClickWhenPress");
    if( rawEvent.value == 1){// key down
        for( auto &keyConfig: keyConfigs){
            int centerX = keyConfig.centerX*mWidth;
            int centerY = keyConfig.centerY*mHeight;
            InputFilter::getInstance()->pushSoftEvent( keyConfig.slotId, TOUCH_DOWN, centerX, centerY);
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            InputFilter::getInstance()->pushSoftEvent( keyConfig.slotId, TOUCH_UP, centerX, centerY);

        }
    }else if( rawEvent.value ==0){//key up

    }
    return;
}


/* 屏幕映射模式--屏幕点击--松开点击 */
void MangmiPolicy::touchScreenClickWhenRelease(RawEvent rawEvent, std::vector<KeyConfig> keyConfigs){
    ALOGD("%s",__func__);
    if( rawEvent.value == 0){// 抬起
        for(auto &config:keyConfigs){
            int centerX = config.centerX*mWidth;
            int centerY = config.centerY*mHeight;
            InputFilter::getInstance()->pushSoftEvent( config.slotId, TOUCH_DOWN, centerX, centerY);
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            InputFilter::getInstance()->pushSoftEvent( config.slotId, TOUCH_UP, centerX, centerY);
        }
    }else if( rawEvent.value ==1){// 按下

    }
}
/* 屏幕映射模式--拖出按键-- 指向性技能-- 摇杆关联选左*/
void MangmiPolicy::mobaAssociateLeftJoystickWhenPress(RawEvent event, std::vector<KeyConfig> keyConfigs){
    ALOGD("%s",__func__);


    if( event.value == 1){// 按下
        for(const auto &config:keyConfigs){
            int centerX = mWidth*config.centerX;
            int centerY = mHeight*config.centerY;
            InputFilter::getInstance()->pushSoftEvent( config.slotId, TOUCH_DOWN, centerX, centerY);
//            同时要保存数据, 当左摇杆 摇动要用到
            addJoyStickMapFromKeyConfig( LEFT_ASSOCIATE_JOYSTICK, config);
        }
    }else if( event.value==0){//抬起
        for( const auto &config:keyConfigs){
            InputFilter::getInstance()->pushSoftEvent( config.slotId, TOUCH_UP, 0,0);
//        将元素删除
            removeJoyStickMapByKeyConfig(LEFT_ASSOCIATE_JOYSTICK, config);
        }

    }
    return;
}
/* 将指向性技能的按键数据保存 */
void MangmiPolicy::addJoyStickMapFromKeyConfig(int associateType, KeyConfig keyConfig){
    ALOGD("%s", __func__);
    JoystickConfig  joystickConfig;
    joystickConfig.centerX =  keyConfig.centerX;
    joystickConfig.centerY =  keyConfig.centerY;
    joystickConfig.sensitivityX =  keyConfig.sensitivityX;
    joystickConfig.sensitivityY = keyConfig.sensitivityY;
    joystickConfig.reverseJoystickX = keyConfig.reverseJoystickX;
    joystickConfig.reverseJoystickY = keyConfig.reverseJoystickY;
    joystickConfig.radius = mWidth *keyConfig.radius;
    joystickConfig.minEffectiveRadiusPercent  = keyConfig.minEffectiveRadiusPercent;

    auto it = associateJoystickMap.find(associateType);
    if( it != associateJoystickMap.end() ){
        std::set<JoystickConfig> associateJoystickConfig = it->second;
        associateJoystickConfig.insert( joystickConfig);
    }else{
        std::set<JoystickConfig> associateJoystickConfig ;
        associateJoystickConfig.insert( joystickConfig);
        associateJoystickMap.insert({associateType, associateJoystickConfig});
    }
    return ;
}
void MangmiPolicy::removeJoyStickMapByKeyConfig(int associateType, KeyConfig keyConfig) {
    ALOGD("%s", __func__);
    JoystickConfig  joystickConfig;
    joystickConfig.centerX =  keyConfig.centerX;
    joystickConfig.centerY =  keyConfig.centerY;
    joystickConfig.sensitivityX =  keyConfig.sensitivityX;
    joystickConfig.sensitivityY = keyConfig.sensitivityY;
    joystickConfig.reverseJoystickX = keyConfig.reverseJoystickX;
    joystickConfig.reverseJoystickY = keyConfig.reverseJoystickY;
    joystickConfig.radius = mWidth *keyConfig.radius;
    joystickConfig.minEffectiveRadiusPercent  = keyConfig.minEffectiveRadiusPercent;
    std::set<JoystickConfig> associateJoystickConfig = associateJoystickMap.at( associateType);
    for( auto it =associateJoystickConfig.begin(); it != associateJoystickConfig.end();){
        if(  *it ==joystickConfig ){
            it = associateJoystickConfig.erase(it);
        } else{
            ++it;
        }
    }
    return;
}


/* 屏幕映射模式--拖出按键--指向性技能-- 摇杆关联选右 */
void MangmiPolicy::mobaAssociateRightJoystickWhenPress(RawEvent event, std::vector<KeyConfig> keyConfigs){
    ALOGD("%s", __func__);
    if( event.value == 1){// 按下
        for(const auto &config:keyConfigs) {
            int centerX = mWidth * config.centerX;
            int centerY = mHeight * config.centerY;
            InputFilter::getInstance()->pushSoftEvent(config.slotId, TOUCH_DOWN, centerX, centerY);
//            同时要保存数据, 当左摇杆 摇动要用到
            addJoyStickMapFromKeyConfig(RIGHT_ASSOCIATE_JOYSTICK, config);
        }
    }else if( event.value ==0){//抬起
        for( const auto &config:keyConfigs){
            InputFilter::getInstance()->pushSoftEvent( config.slotId, TOUCH_UP, 0,0);
            // 将元素删除
            removeJoyStickMapByKeyConfig(RIGHT_ASSOCIATE_JOYSTICK, config);
        }
    }
    return;
}

/* 屏幕映射模式--拖出按键--指向性技能--摇杆关联选智能*/
void MangmiPolicy::mobaAssociateSmartJoystickWhenPress(RawEvent event, std::vector<KeyConfig> keyConfigs){
    ALOGD("%s",__func__);
    if( event.value==1){//按下
        for(const auto &config:keyConfigs) {
            int centerX = mWidth * config.centerX;
            int centerY = mHeight * config.centerY;
            InputFilter::getInstance()->pushSoftEvent(config.slotId, TOUCH_DOWN, centerX, centerY);
//            同时要保存数据, 当左摇杆 摇动要用到
            addJoyStickMapFromKeyConfig(SMART_ASSOCIATE_JOYSTICK, config);
        }
    }else if( event.value ==0){//抬起
        for( const auto &config:keyConfigs){
            InputFilter::getInstance()->pushSoftEvent( config.slotId, TOUCH_UP, 0,0);
//          删除元素
            removeJoyStickMapByKeyConfig( SMART_ASSOCIATE_JOYSTICK, config);
        }
    }
    return;
}

/* 屏幕映射模式--取消技能,   有按下和抬起的动作 同时将 按键的指向性技能取消 */
void MangmiPolicy::mobaCancelSkillWhenPress(RawEvent event, std::vector<KeyConfig> keyConfigs){
    ALOGD("%s",__func__);

    if( event.value ==1){//按下
        iCancelSkill = true;
        for( const auto &config: keyConfigs){
            int centerX = mWidth * config.centerX;
            int centerY = mHeight * config.centerY;
            InputFilter::getInstance()->pushSoftEvent(config.slotId, TOUCH_DOWN, centerX, centerY);
        }
//      同时将指向性技能的键抬起
        for(auto it =associateJoystickMap.begin(); it!=associateJoystickMap.end(); it++){
            int type = it->first;
            std::set<JoystickConfig> joystickConfigs = it->second;
            for(auto config:joystickConfigs){
                InputFilter::getInstance()->pushSoftEvent( config.slotId, TOUCH_UP, 0,0);
            }
        }
        associateJoystickMap.clear();//同时将map清除
    }else if(event.value ==0){//抬起取消技能按键
        iCancelSkill = false;
        for(const auto config:keyConfigs){
            InputFilter::getInstance()->pushSoftEvent(config.slotId, TOUCH_UP, 0, 0);
        }
    }
    return;
}
/* 屏幕映射模式--查看地图 */
void MangmiPolicy::mobaViewMapWhenPress(RawEvent event, std::vector<KeyConfig> keyConfigs){
    ALOGD("%s",__func__);
    if( event.value ==1){ //按下
        mapViewMode = 1;
    }else if(event.value ==0){//抬起
        mapViewMode = 0;
    }
    return;
}

/* */
void MangmiPolicy::handleAxisPlus( RawEvent event) {
    ALOGD("build new Event");
//  十字键， L2,R2，左右摇杆 都是abs事件
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
        case ABS_BRAKE:
            absBrake( event);
            break;
        default:
            break;
    }
    return;

}
/* 左摇杆 */
void MangmiPolicy::leftJoystick(RawEvent& event){
    ALOGD("leftJoystick");
    std::map<int, vector<JoystickConfig>> joystickMap = MangmiConfig::getInstance()->getJoystickConfigsMapByInputId( INPUT_ID_LEFT_JOYSTICK );// 一个左摇杆 可以配置多个策略
    for( auto it=joystickMap.begin();it!=joystickMap.end();++it){
        int type = it->first;
        std::vector<JoystickConfig> joystickConfigs = it->second;
        switch ( type){// 每一个类型 对应一种处理方式
            case JOYSTICK_CATEGORY_GAMEPAD:
                break;
            case JOYSTICK_CATEGORY_KEYBOARD:
                break;
            case JOYSTICK_CATEGORY_MOUSE:
                break;
            case JOYSTICK_CATEGORY_TOUCHSCREEN:
                break;
            case JOYSTICK_TYPE_TOUCHSCREEN_JOYSTICK://虚拟摇杆
            case JOYSTICK_TYPE_TOUCHSCREEN_SKILL_SHOT:// 指向性技能
                leftVirtualJoystick(event, joystickConfigs);
                break;
            case JOYSTICK_TYPE_TOUCHSCREEN_ADJUST_VIEW://调整视角
                break;
            default:
                break;
        }
    }
    return;
}
/* 右摇杆 */
void MangmiPolicy::rightJoystick(RawEvent event) {
    ALOGD("rightJoystick");
    std::map<int, std::vector<JoystickConfig>> joystickMap = MangmiConfig::getInstance()->getJoystickConfigsMapByInputId( INPUT_ID_RIGHT_JOYSTICK );// 一个左摇杆 可以配置多个策略
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
            case JOYSTICK_TYPE_TOUCHSCREEN_JOYSTICK://虚拟摇杆
            case JOYSTICK_TYPE_TOUCHSCREEN_SKILL_SHOT:// 指向性技能
                rightVirtualJoystick(event, joystickConfigs);
                break;
            case JOYSTICK_TYPE_TOUCHSCREEN_ADJUST_VIEW://调整视角
                break;
            default:
                break;
        }
    }
    return;
}



#define DEADZONE_CENTERPOINT 5
/* 左虚拟摇杆 */
bool isLeftDown =false;
bool isAxisLeftDown = false;
int leftOrigX=0, leftOrigY=0;
int leftAxisX=0, leftAxisY=0;
void MangmiPolicy::leftVirtualJoystick(RawEvent event,  std::vector<JoystickConfig> joystickConfigs){
    ALOGD("%s, deviceId:%d, type:%d, code:%d, value:%d",__func__, event.deviceId, event.type, event.code, event.value);
    int axisX=0, axisY=0;
    mWidth =InputFilter::mWidth;
    mHeight = InputFilter::mHeight;
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

//   当没有按下取消技能时, 从指向性技能里查出 关联左摇杆和智能摇杆 的按键配置. 如果按下了取消技能按键,抬起关联了左摇杆和智能摇杆的按键
    if( iCancelSkill ==false ){
        if( associateJoystickMap.count(LEFT_ASSOCIATE_JOYSTICK) >0){
            std::set<JoystickConfig> leftAssociateJoystickConfig =associateJoystickMap.at(LEFT_ASSOCIATE_JOYSTICK);
            joystickConfigs.insert( joystickConfigs.end(),leftAssociateJoystickConfig.begin(), leftAssociateJoystickConfig.end());

        }else if( associateJoystickMap.count(SMART_ASSOCIATE_JOYSTICK) >0){
            std::set<JoystickConfig> smartAssociateJoystickConfig =associateJoystickMap.at( SMART_ASSOCIATE_JOYSTICK);
            joystickConfigs.insert( joystickConfigs.end(), smartAssociateJoystickConfig.begin(), smartAssociateJoystickConfig.end());
        }

    }

    /*当摇杆从非死区移动到死区位置，或回到中心位置时，生成抬起事件*/
    if( (abs(leftOrigX)<DEADZONE_CENTERPOINT) && abs(leftOrigY)<DEADZONE_CENTERPOINT ){
        if( isAxisLeftDown ==true){
            if( (abs(leftOrigX) < DEADZONE_CENTERPOINT) && (abs(leftOrigY) < DEADZONE_CENTERPOINT) ){
                leftOrigX =0;
                leftOrigY =0;
                isAxisLeftDown =false;
                isLeftDown =false;
                for(int i=0; i<joystickConfigs.size(); i++){
                    int slotId = joystickConfigs[i].slotId;
                    ALOGD("slotId:%d, TOUCH_UP", slotId);
                    InputFilter::getInstance()->pushSoftEvent(slotId, TOUCH_UP, 0, 0);
                }
            }
        }

    }else{
        if( isAxisLeftDown ==false ){//
            isLeftDown =true;
            isAxisLeftDown = true;

            for(int i=0; i<joystickConfigs.size();i++){
                JoystickConfig joystickConfig = joystickConfigs[i];
                float fRadius = mWidth * joystickConfig.radius;
                float fSenX = joystickConfig.sensitivityX;
                float fSenY = joystickConfig.sensitivityY;
                float fCenterX = mWidth * joystickConfig.centerY;
                float fCenterY = mHeight - mHeight * joystickConfig.centerX;
                float fMinRadius = fRadius * joystickConfig.minEffectiveRadiusPercent;
                float xMinRadius = fMinRadius*fSenX;
                float yMinRadius = fMinRadius*fSenY;

                int x = (int)((double)axisX / 32768.0 * fRadius * fSenX + fCenterX);
                int y = (int)((double)axisY / 32768.0 * fRadius * fSenY + fCenterY);
                int slotId = joystickConfigs[i].slotId;
                ALOGD("slotId:%d, TOUCH_DOWN x:%d, y:%d", slotId, x, y);
                InputFilter::getInstance()->pushSoftEvent( slotId, TOUCH_DOWN, x, y);
            }
        }else{
            if( isLeftDown == true){
                for(int i=0; i<joystickConfigs.size();i++){
                    JoystickConfig joystickConfig = joystickConfigs[i];
                    float fRadius = mWidth * joystickConfig.radius;
                    float fSenX = joystickConfig.sensitivityX;
                    float fSenY = joystickConfig.sensitivityY;
                    float fCenterX = mWidth * joystickConfig.centerY;
                    float fCenterY = mHeight - mHeight * joystickConfig.centerX;
                    int x = (int)((double)axisX / 32768.0 * fRadius * fSenX + fCenterX);
                    int y = (int)((double)axisY / 32768.0 * fRadius * fSenY + fCenterY);
                    int slotId = joystickConfigs[i].slotId;
                    ALOGD("slotId:%d, TOUCH_MOVE x:%d, y:%d", slotId, x, y);
                    InputFilter::getInstance()->pushSoftEvent( slotId, TOUCH_MOVE, x, y);
                }
            }
        }
    }

}

/* 右虚拟摇杆 */
bool isRightDown =false;
bool isAxisRightDown = false;
int rightOrigX=0, rightOrigY=0;
int rightAxisX=0, rightAxisY=0;
void MangmiPolicy::rightVirtualJoystick(RawEvent event, std::vector<JoystickConfig> joystickConfigs){
    ALOGD("%s, deviceId:%d, type:%d, code:%d, value:%d",__func__, event.deviceId, event.type, event.code, event.value);

    int axisX=0, axisY=0;
    mWidth =InputFilter::mWidth;
    mHeight = InputFilter::mHeight;
    if( event.code ==ABS_X){
        rightOrigX = event.value;
        axisY = mWidth - event.value; //swap x->y
        rightAxisY = axisY;
        axisX = rightAxisX;
    }else if( event.code ==ABS_Y){
        rightOrigX = event.value;
        axisX = event.value; //swap y->x
        rightAxisX = axisX;
        axisY = rightAxisY;
    }
//   当没有按下取消技能时, 从指向性技能里查出 关联右摇杆和智能摇杆 的按键配置. 如果按下了取消技能,抬起关联了右摇杆和智能摇杆的按键
    if( iCancelSkill ==false ){
        std::set<JoystickConfig> rightAssociateJoystickConfig =associateJoystickMap.at( RIGHT_ASSOCIATE_JOYSTICK);
        std::set<JoystickConfig> smartAssociateJoystickConfig =associateJoystickMap.at( SMART_ASSOCIATE_JOYSTICK);
        joystickConfigs.insert( joystickConfigs.end(),rightAssociateJoystickConfig.begin(), rightAssociateJoystickConfig.end());
        joystickConfigs.insert( joystickConfigs.end(), smartAssociateJoystickConfig.begin(), smartAssociateJoystickConfig.end());
    }

    /*当摇杆从非死区移动到死区位置，或回到中心位置时，生成抬起事件*/
    if( (abs(rightOrigX)<DEADZONE_CENTERPOINT) && abs(rightOrigY)<DEADZONE_CENTERPOINT ){
        if( isAxisRightDown ==true){
            if( (abs(rightOrigX) < DEADZONE_CENTERPOINT) && (abs(rightOrigY) < DEADZONE_CENTERPOINT) ){
                rightOrigX =0;
                rightOrigY =0;
                isAxisRightDown =false;
                isRightDown =false;
                for(int i=0; i<joystickConfigs.size(); i++){
                    int slotId = joystickConfigs[i].slotId;
                    ALOGD("slotId:%d, TOUCH_UP", slotId);
                    InputFilter::getInstance()->pushSoftEvent(slotId, TOUCH_UP, 0, 0);
                }
            }
        }

    }else{
        if( isAxisRightDown ==false ){//
            isRightDown =true;
            isAxisRightDown = true;

            for(int i=0; i<joystickConfigs.size();i++){
                JoystickConfig joystickConfig = joystickConfigs[i];
                float fRadius = mWidth * joystickConfig.radius;
                float fSenX = joystickConfig.sensitivityX;
                float fSenY = joystickConfig.sensitivityY;
                float fCenterX = mWidth * joystickConfig.centerY;
                float fCenterY = mHeight - mHeight * joystickConfig.centerX;
                float fMinRadius = fRadius * joystickConfig.minEffectiveRadiusPercent;
                float xMinRadius = fMinRadius*fSenX;
                float yMinRadius = fMinRadius*fSenY;

                int x = (int)((double)axisX / 32768.0 * fRadius * fSenX + fCenterX);
                int y = (int)((double)axisY / 32768.0 * fRadius * fSenY + fCenterY);
                int slotId = joystickConfigs[i].slotId;
                ALOGD("slotId:%d, TOUCH_DOWN x:%d, y:%d", slotId, x, y);
                InputFilter::getInstance()->pushSoftEvent( slotId, TOUCH_DOWN, x, y);
            }
        }else{
            if( isRightDown == true){
                for(int i=0; i<joystickConfigs.size();i++){
                    JoystickConfig joystickConfig = joystickConfigs[i];
                    float fRadius = mWidth * joystickConfig.radius;
                    float fSenX = joystickConfig.sensitivityX;
                    float fSenY = joystickConfig.sensitivityY;
                    float fCenterX = mWidth * joystickConfig.centerY;
                    float fCenterY = mHeight - mHeight * joystickConfig.centerX;
                    int x = (int)((double)axisX / 32768.0 * fRadius * fSenX + fCenterX);
                    int y = (int)((double)axisY / 32768.0 * fRadius * fSenY + fCenterY);
                    int slotId = joystickConfigs[i].slotId;
                    ALOGD("slotId:%d, TOUCH_MOVE x:%d, y:%d", slotId, x, y);
                    InputFilter::getInstance()->pushSoftEvent( slotId, TOUCH_MOVE, x, y);
                }
            }
        }
    }
    return;
}

/* 十字轴 方向键 */
int lastKey = 0;
void MangmiPolicy::absHatXY(RawEvent &event) {
    ALOGD("absHatXY deviceId:%d, type:%d, code:%d, value:%d", event.deviceId, event.type, event.code, event.value);

    RawEvent newEvent;
    if (event.code == ABS_HAT0X && event.value == 1) {// 横轴右键下
        lastKey = KEY_RIGHT;
        newEvent = generateEvent(event.deviceId, EV_KEY, KEY_RIGHT, 1);
    } else if (event.code == ABS_HAT0X && event.value == -1) {//横轴左键按下
        lastKey = KEY_LEFT;
        newEvent = generateEvent(event.deviceId, EV_KEY, KEY_LEFT, 1);
    } else if (event.code == ABS_HAT0X && event.value == 0) {//横轴 抬起
        if (lastKey == KEY_LEFT) {
            newEvent = generateEvent(event.deviceId, EV_KEY, KEY_LEFT, 0);
        } else if (lastKey == KEY_RIGHT) {
            newEvent = generateEvent(event.deviceId, EV_KEY, KEY_RIGHT, 0);
        }
        lastKey=0;
    } else if (event.code == ABS_HAT0Y && event.value == 1) {// 纵轴上键 按下
        lastKey = KEY_DOWN;
        newEvent = generateEvent(event.deviceId, EV_KEY, KEY_DOWN, 1);
    } else if (event.code == ABS_HAT0Y && event.value == -1) {// 纵轴下键 按下
        lastKey = KEY_UP;
        newEvent = generateEvent(event.deviceId, EV_KEY, KEY_UP, 1);
    } else if (event.code == ABS_HAT0Y && event.value == 0) {//纵轴抬起
        if (lastKey == KEY_DOWN) {
            newEvent = generateEvent(event.deviceId, EV_KEY, KEY_UP, 0);
        } else if (lastKey == KEY_UP) {
            newEvent = generateEvent(event.deviceId, EV_KEY, KEY_DOWN, 0);
        }
        lastKey=0;
    }
    handleKeyPlus(newEvent);//根据配置决定动作
}

/* L2 键 */
void MangmiPolicy::absGas(RawEvent &event){
    ALOGD("absGas ");
    RawEvent newEvent;
    if( event.code ==ABS_GAS && event.value==1){
        newEvent =generateEvent(event.deviceId, EV_KEY, BTN_TL2, 1);

    }else if( event.code ==ABS_GAS && event.value ==0){
        newEvent =generateEvent( event.deviceId, EV_KEY, BTN_TL2, 0);
    }
//    InputFilter::getInstance()->pushSoftEvent( newEvent);
    handleKeyPlus( newEvent);
}

/* R2 键 */
void MangmiPolicy::absBrake(RawEvent &event){
    ALOGD("absBrake");
    RawEvent newEvent;
    if( event.code ==ABS_BRAKE && event.value==1){
        newEvent = generateEvent( event.deviceId, EV_KEY, BTN_TR2,1);

    }else if( event.code ==ABS_BRAKE && event.value ==0){
        newEvent = generateEvent( event.deviceId, EV_KEY, BTN_TR2, 0);
    }
//    InputFilter::getInstance()->pushSoftEvent( newEvent);
    handleKeyPlus( newEvent);
}




