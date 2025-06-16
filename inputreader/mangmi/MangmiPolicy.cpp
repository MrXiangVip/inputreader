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

MangmiPolicy *MangmiPolicy::instance= nullptr;
std::vector<socketReply> MangmiPolicy::vectorReply;//定义
bool MangmiPolicy::running = false;
MangmiSocketClient MangmiPolicy::key_socket_client;//
int MangmiPolicy::iSlotId=20;// iSlotId start from 20
std::vector<int> MangmiPolicy::jsLeftSlotId; //
std::vector<int> MangmiPolicy::jsRightSlotId;//
std::vector<IdConfig> MangmiPolicy::idConfigs;//
int MangmiPolicy::mWidth;
int MangmiPolicy::mHeight;
//bool MangmiPolicy::isLeftDown = false;
//int MangmiPolicy::leftJoysStatus =TOUCH_NONE;

socketReply::socketReply(int mRequestId,int mInputId){
    requestId = mRequestId;
    inputId =mInputId;
}

MangmiPolicy::MangmiPolicy() {
    ALOGD("init");
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
    MangmiPolicy *self = static_cast<MangmiPolicy*>(args);
    running = true;
    self->runServer();
}

void MangmiPolicy::runServer() {
    ALOGD("runServer");
    char buffer[BUFFER_SIZE] = {0};
    int new_socket = -1;
    std::string received_data;

    signal(SIGPIPE, SIG_IGN);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        ALOGI("socket failed");
        return;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        ALOGI("setsockopt failed");
        return;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        ALOGI("bind failed");
        return;
    }

    if (listen(server_fd, 3) < 0) {
        ALOGI("listen failed");
        return;
    }

    ALOGI("Server is listening on port %d", PORT);

    while (running) {
        new_socket = accept(server_fd, nullptr, nullptr);
        if (new_socket < 0) {
            ALOGI("accept failed");
            break;
        }

        // 不断接收数据直到遇到以“@”结尾的数据包
        received_data.clear();
        memset(buffer, 0, sizeof(buffer));
        while (true) {
            int valread = read(new_socket, buffer, BUFFER_SIZE);
            if (valread <= 0) {
                break; // 连接关闭
            }

            received_data += std::string(buffer, valread);

            // 检查是否以“@”结尾
            if (!received_data.empty() && received_data.back() == '@') {
                break;
            }

            // 清空缓冲区以便下一次读取
            memset(buffer, 0, sizeof(buffer));
//
        } //while (true)
        dealReceivedData( received_data );

    }
    close(server_fd);
    server_fd = -1;
}


int MangmiPolicy::dealReceivedData(std::string receivedData) {
    ALOGD("dealReceivedData %s", receivedData.c_str());
    int iRet;
    size_t start_pos = 0;
    std::vector<std::string> data_segments;

    size_t end_pos = receivedData.find('#');

    while (end_pos != std::string::npos) {
        data_segments.push_back(receivedData.substr(start_pos, end_pos - start_pos));
        start_pos = end_pos + 1;
        end_pos = receivedData.find('#', start_pos);
    }

    // 处理最后一段数据（如果有的话）
    if (start_pos < receivedData.length() - 1) {
        data_segments.push_back(receivedData.substr(start_pos));
    }

    std::string cRequestId;
    std::string cSerialNo;
    std::string cDataLen;
    std::string cData;

    for (size_t i = 0; i < data_segments.size(); ++i) {
//        ALOGI("---data_segments(%d):len = %d, data: %s", i, data_segments[i].length(), data_segments[i].c_str());
        if (0 == i) {
            cRequestId = data_segments[i].c_str();
        } else if (1 == i) {
            cSerialNo = data_segments[i].c_str();
        } else if (2 == i) {
            cDataLen = data_segments[i].c_str();
        } else if (3 == i) {
            cData = data_segments[i].c_str();
        }
    }

    ALOGI("---data_segments: cRequestId = %s, cSerialNo = %s, cDataLen = %s, cData: %s", cRequestId.c_str(),
          cSerialNo.c_str(), cDataLen.c_str(), cData.c_str());
    if ("1880" == cSerialNo) { //设置当前映射配置
        iRet =MangmiConfig::getInstance()->parseJson( cData);

        iRet =updateIdConfigs( );
    }else if( "1881" == cSerialNo){
        iRet = stoi(cRequestId);
    }else if("1882" == cSerialNo ){
//
        iRet = replyData( cRequestId, cData);
        if( 0== stoi(cDataLen) ){

        }
    }else{
        ALOGD("unkown serialNo %d", cSerialNo.c_str());
    }
    ALOGI("---DealWith_Deceiveddata---success");

//    更新拦截策略
    updateInterceptPolicy();
    return iRet;
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
            if( KEY_TYPE_GAMEPAD_CLICK_STANDARD != eventConfig.keyConfigs[i].type ){
                ALOGD("标准点击事件不拦截");
            }
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


int MangmiPolicy::replyData( std::string requestId, std::string data){
    ALOGD("replyData ");
    std::string token;
    char delimiter = '$';
    int ilen =0;
    for (char c : data) {
        if (c == delimiter) {
            if (!token.empty()) {
                ilen++;
                vectorReply.emplace_back(socketReply(std::stoi(requestId), std::stoi(token))); // 将字符串转换为整数
                token.clear();
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        ilen++;
        vectorReply.emplace_back(std::stoi(requestId), std::stoi(token)); // 将最后一个标记转换为整数
    }
    return ilen;
}

int MangmiPolicy::updateInterceptPolicy() {
    ALOGD("updateInterceptPolicy");
    std::set<int> keySet;
    std::set<int> axisSet;

    MangmiIntercepter::eraseInterceptEventBySerialNo(1180);

//    更新拦截策略
    //    mangmi::InputFilter::setInputsFilter(keySet, axisSet);

    return 0;
}



int MangmiPolicy::replyPolicy(int inputId, RawEvent event){
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

    RECONNECT:
    if(false == key_socket_client.running)
    {
        // 客户端连接到服务器: 127.0.0.1 8088
        key_socket_client.connectToServer("127.0.0.1");
    }

    // 发送消息到服务器
    ALOGI("---%s---sendData = %s", __func__, data.c_str());
    iRet = key_socket_client.sendMessage(data);
    if(0 != iRet) {
        ALOGI("---%s---iRet = %d, sendMessage error", __func__, iRet);
        // 停止客户端
        key_socket_client.stop();
        goto RECONNECT;
    }

    iRet = key_socket_client.runClient(requestId);
    ALOGI("---%s---iRet = %d", __func__, iRet);
    if(-1 == iRet)
        ALOGI("---%s---iRet = %d, requestId error", __func__, iRet);

    return iRet;
}




void MangmiPolicy::buildKeyEvent( RawEvent event){
    ALOGD("新生成一个事件 deviceId:%d, type:%d, code:%d, value:%d", event.deviceId, event.type, event.code, event.value);

    std::map<int, std::vector<KeyConfig>> keyConfigMap =MangmiConfig::getInstance()->getKeyConfigsMap();

//  根据当前接到的配置 选择一个策略 来生成新的事件
    for( auto it=keyConfigMap.begin();it!=keyConfigMap.end();++it){
        //      每一个配置的类型对应一种 按键处理方式
        int type = it->first;
        std::vector<KeyConfig> keyConfigs = it->second;
        switch(type ){
            case KEY_CATEGORY_GAMEPAD://
                break;
            case KEY_CATEGORY_KEYBOARD://
                break;
            case KEY_CATEGORY_MOUSE:
                break;
            case KEY_CATEGORY_TOUCHSCREEN:
                break;
            case KEY_TYPE_TOUCHSCREEN_CLICK_STANDARD:
                convertToStandardClick(event, keyConfigs);
                break;
            case KEY_CATEGORY_MOBA://
                break;
            default:
                break;
        }
    }

    return;
}
void MangmiPolicy::buildAxisEvent( RawEvent event) {
    ALOGD("build new Event");
//  摇杆就只有左右摇杆的 event.code
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
    std::vector<JoystickConfig> jConfigs = MangmiConfig::getInstance()->getJoystickConfigsFromId( INPUT_ID_LEFT_JOYSTICK );// 一个左摇杆 可以配置多个策略
    for( int i; i<jConfigs.size(); i++){
        JoystickConfig config= jConfigs[i];
        int axisX =0;
        int axisY =0;
        float fRadius = mWidth *config.radius;
        float fSenX = config.sensitivityX;
        float fSenY = config.sensitivityY;
        float fCenterX = mWidth *config.centerY;
        float fCenterY = mHeight - mHeight*config.centerX;
        int x = (int)((double)axisX / 32768.0 * fRadius * fSenX + fCenterX);
        int y = (int)((double)axisY / 32768.0 * fRadius * fSenY + fCenterY);
        int slotId = getSlotIdFromIdConfig("joystickConfigs", i + config.id + config.type);

        switch (config.type){// 每一个类型 对应一种处理方式
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

void MangmiPolicy::rightJoystick(RawEvent event) {
    ALOGD("rightJoystick");
    std::vector<JoystickConfig> jConfigs = MangmiConfig::getInstance()->getJoystickConfigsFromId( INPUT_ID_RIGHT_JOYSTICK );// 一个左摇杆 可以配置多个策略
    for( int i; i<jConfigs.size(); i++){
        JoystickConfig config= jConfigs[i];
        int axisX =0;
        int axisY =0;
        float fRadius = mWidth *config.radius;
        float fSenX = config.sensitivityX;
        float fSenY = config.sensitivityY;
        float fCenterX = mWidth *config.centerY;
        float fCenterY = mHeight - mHeight*config.centerX;
        int x = (int)((double)axisX / 32768.0 * fRadius * fSenX + fCenterX);
        int y = (int)((double)axisY / 32768.0 * fRadius * fSenY + fCenterY);
        int slotId = getSlotIdFromIdConfig("joystickConfigs", i + config.id + config.type);

        switch (config.type){// 每一个类型 对应一种处理方式
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

void MangmiPolicy::absHatXY(RawEvent &event){
    ALOGD("absHatXY deviceId:%d, type:%d, code:%d, value:%d", event.deviceId, event.type, event.code, event.value);
    std::vector<int> inputId = MangmiUtils::getInputIdFromEvcode( event.code);// 这里有问题, 一个inputID 对应一个evCode, 但 一个evCode 可能有多个 inputID
    int inputId0 = inputId[0];
    std::vector<KeyConfig> keyConfigs= MangmiConfig::getInstance()->getKeyConfigsByInputId( inputId0 );
    if( keyConfigs.empty() ){return;}
    for(auto keyConfig:keyConfigs){
        RawEvent event;
        if( event.code==ABS_HAT0X && event.value==1 ){// 轴右键
            event =generateEvent( event.deviceId, EV_KEY, KEY_RIGHT,1);
        }else if( event.code = ABS_HAT0X && event.value ==-1 ){//横轴左键
            event =generateEvent(  event.deviceId, EV_KEY, KEY_LEFT, 1);
        }else if( event.code ==ABS_HAT0X && event.value ==0 ){//
            if( inputId0 == INPUT_ID_LEFT){
                event= generateEvent(  event.deviceId, EV_KEY, KEY_LEFT, 0);
            }else if( inputId0 == INPUT_ID_RIGHT ){
                event =generateEvent(  event.deviceId, EV_KEY, KEY_RIGHT, 0);
            }
        }else if( event.code ==ABS_HAT0Y && event.value ==1 ){
            event =generateEvent(  event.deviceId, EV_KEY, KEY_DOWN, 1);
        }else if( event.code ==ABS_HAT0Y && event.value ==-1){
            event =generateEvent(  event.deviceId, EV_KEY, KEY_UP, 1);
        }else if( event.code ==ABS_HAT0Y && event.value ==0){
            if( inputId0 == INPUT_ID_UP){
                event =generateEvent(  event.deviceId, EV_KEY, KEY_UP, 0);
            }else if( inputId0 == INPUT_ID_DOWN){
                event =generateEvent(  event.deviceId, EV_KEY, KEY_DOWN, 0);
            }
        }
        buildKeyEvent( event);
    }
}

void MangmiPolicy::absGas(RawEvent &event){

}

void MangmiPolicy::absBreak(RawEvent &event){

}

RawEvent& MangmiPolicy::generateEvent( int32_t deviceId, int type, int keyCode, int32_t value) {
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



void MangmiPolicy::convertToStandardClick( RawEvent &event, std::vector<KeyConfig> keyConfig){

//    int tmpInputId= MangmiUtils::getInputIdFromEvcode( event.code);
//    float centerX = keyConfig.centerX;
//    float centerY = keyConfig.centerY;
//    std::vector<int>  slotIds =getSlotIdFromKeySlotConfig( tmpInputId, keyConfig.type );
//    if( event.value == 1){
//        InputFilter::getInstance()->pushSoftEvent(id, TOUCH_DOWN, centerX, centerY);
//    }else if( event.value == 0){
//        InputFilter::getInstance()->pushSoftEvent(id, TOUCH_UP, centerX, centerY);
//    }

}

std::vector<int> MangmiPolicy::getSlotIdFromKeySlotConfig(int id, int type) {
    return std::vector<int>();
}

