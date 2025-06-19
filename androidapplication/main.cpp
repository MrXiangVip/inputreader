#include <iostream>
#include <vector>
#include "AndroidClient.h"
#include "AndroidServer.h"
#include <string>


std::string splite="#";

std::string  info="{\"alpha\":1.0,\"createdTime\":0,\"desc\":\"\",\"id\":1748570482171,\"joystickConfigs\":[],\"keyConfigs\":[{\"duration\":0,\"exclusiveJoystick\":false,\"targetCode\":0,\"triggerType\":0,\"width\":0,\"centerX\":0.7609375,\"centerY\":0.6111111,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":22,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":5005},{\"duration\":0,\"exclusiveJoystick\":false,\"targetCode\":0,\"triggerType\":0,\"width\":0,\"centerX\":0.81822914,\"centerY\":0.41666666,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":21,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":5,\"type\":5006}],\"packageName\":\"com.android.gallery3d\",\"title\":\"图库\",\"toolbarLocation\":0,\"type\":0,\"updatedTime\":1748752631731,\"version\":0}";

std::string  tuku="{\"alpha\":1.0,\"createdTime\":0,\"desc\":\"\",\"id\":1748570482171,\"joystickConfigs\":[],\"keyConfigs\":[{\"duration\":0,\"exclusiveJoystick\":false,\"targetCode\":0,\"triggerType\":0,\"width\":0,\"centerX\":0.7609375,\"centerY\":0.6111111,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":22,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":5005},{\"duration\":0,\"exclusiveJoystick\":false,\"targetCode\":0,\"triggerType\":0,\"width\":0,\"centerX\":0.81822914,\"centerY\":0.41666666,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":21,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":5,\"type\":4001}],\"packageName\":\"com.android.gallery3d\",\"title\":\"图库\",\"toolbarLocation\":0,\"type\":0,\"updatedTime\":1748927111168,\"version\":0}";

std::string info1182="0#1882#2#62#@";


/*  横轴方向键映射为点击事件的JSON配置 */
std::string leftArrow="0#1880#915#{\"alpha\":1.0,\"createdTime\":1749780629870,\"desc\":\"\",\"id\":1749780629870,\"joystickConfigs\":[],\"keyConfigs\":[{\"duration\":0,\"exclusiveJoystick\":false,\"targetCode\":0,\"triggerType\":0,\"width\":0,\"centerX\":0.2640625,\"centerY\":0.25185186,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":14,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":4001},{\"duration\":0,\"exclusiveJoystick\":false,\"targetCode\":0,\"triggerType\":0,\"width\":0,\"centerX\":0.34322917,\"centerY\":0.26018518,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":13,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":4001}],\"packageName\":\"com.example.myapplication\",\"title\":\"My Application\",\"toolbarLocation\":0,\"type\":0,\"updatedTime\":1750065682578,\"version\":0}#@";
/* */
//std::string originalMode="0#1880#2024#{\"alpha\":1.0,\"createdTime\":1749630268592,\"desc\":\"Empty config\",\"id\":1749630268592,\"joystickConfigs\":[{\"centerX\":0.0,\"centerY\":0.0,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":1,\"invertJoystickX\":true,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.0,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":1001},{\"centerX\":0.0,\"centerY\":0.0,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":2,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.0,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":1002}],\"keyConfigs\":[{\"duration\":0,\"exclusiveJoystick\":false,\"targetCode\":97,\"triggerType\":0,\"width\":0,\"centerX\":0.0,\"centerY\":0.0,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":21,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.0,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":1001}],\"packageName\":\"com.example.myapplication\",\"title\":\"My Application\",\"toolbarLocation\":0,\"type\":0,\"updatedTime\":1750065682578,\"version\":0}#@";
/* 配置A键连击 */
std::string  comboClick="0#1880#2026#{\"alpha\":1.0,\"createdTime\":1749630268592,\"desc\":\"Empty config\",\"id\":1749630268592,\"joystickConfigs\":[{\"centerX\":0.0,\"centerY\":0.0,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":1,\"invertJoystickX\":true,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.0,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":1001},{\"centerX\":0.0,\"centerY\":0.0,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":2,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.0,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":1002}],\"keyConfigs\":[{\"duration\":166,\"exclusiveJoystick\":false,\"targetCode\":96,\"triggerType\":0,\"width\":0,\"centerX\":0.0,\"centerY\":0.0,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":21,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.0,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":1002}],\"packageName\":\"com.example.myapplication\",\"title\":\"My Application\",\"toolbarLocation\":0,\"type\":0,\"updatedTime\":1750065682578,\"version\":0}#@";

/* 托出摇杆 */
std::string yaogan="0#1880#752#{\"alpha\":1.0,\"createdTime\":1749780629870,\"desc\":\"\",\"id\":1749780629870,\"joystickConfigs\":[{\"centerX\":0.12135417,\"centerY\":0.57685184,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":1,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":4001},{\"centerX\":0.828125,\"centerY\":0.59074074,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":2,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":4001}],\"keyConfigs\":[],\"packageName\":\"com.example.myapplication\",\"title\":\"My Application\",\"toolbarLocation\":0,\"type\":0,\"updatedTime\":1750143034915,\"version\":0}#@";

/* two right 摇杆 */
std::string towRightYaoGan="0#1880#740#{\"alpha\":1.0,\"createdTime\":0,\"desc\":\"\",\"id\":1750163596205,\"joystickConfigs\":[{\"centerX\":0.8286458,\"centerY\":0.6259259,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":1,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":4001},{\"centerX\":0.64739585,\"centerY\":0.63148147,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":1,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":4001}],\"keyConfigs\":[],\"packageName\":\"com.example.myapplication\",\"title\":\"My Application\",\"toolbarLocation\":0,\"type\":0,\"updatedTime\":1750214252833,\"version\":0}#@";

/* 摇杆设置为 指向性技能 */
std::string pointSkill="0#1880#479#{\"alpha\":1.0,\"createdTime\":0,\"desc\":\"\",\"id\":1750163596205,\"joystickConfigs\":[{\"centerX\":0.2,\"centerY\":0.64537036,\"followPointerPosition\":false,\"hideFromUi\":false,\"id\":1,\"invertJoystickX\":false,\"invertJoystickY\":false,\"minEffectiveRadiusPercentage\":0.0,\"radius\":0.1,\"sensitivityX\":1.0,\"sensitivityY\":1.0,\"speed\":0,\"type\":4003}],\"keyConfigs\":[],\"packageName\":\"com.example.myapplication\",\"title\":\"My Application\",\"toolbarLocation\":0,\"type\":0,\"updatedTime\":1750239776770,\"version\":0}#@";

std::string  buildMessage( int requestId, int serialNo, std::string info ){
    std::string  retString;
    retString=std::to_string(requestId);//
    retString+=splite;// 分隔符
    retString+=std::to_string(serialNo);
    retString+=splite;
    retString+=std::to_string(info.length());
    retString+=splite;
    retString+=info;
    std::cout<<info.c_str()<<std::endl;
    return  retString;
}


std::string buildErrorMessage( int requestId, int serialNo, std::string info ){
    std::string  retString;
    retString=std::to_string(requestId);//
    retString+=splite;// 分隔符
//    retString+=std::to_string(serialNo);
    retString+="";
    retString+=splite;
    retString+=std::to_string(info.length());
    retString+=splite;
    retString+=info;
    std::cout<<info.c_str()<<std::endl;
    return  retString;
}
int main() {
    std::cout << "Hello, World!" << std::endl;


//    std::string message =buildMessage(  0,1880,info);
    AndroidClient* client = AndroidClient::getInstance( );
//    client->connectClientSocket();
    client->sendMessage( leftArrow );
//    client->sendMessage( originalMode );
//    client->sendMessage( towRightYaoGan );
//    client->sendMessage( yaogan );
//    AndroidServer* server = AndroidServer::getInstance();
//    server->createServer();
//    server->runServer();

}





//#include <string>
//#include <stdexcept>
//#include <cctype>



//void MangmiSocketClient::DealWith_DeceivedData(int fd, std::string data) {
//    try {
//        if (isNumeric(data)) {
//            int value = std::stoi(data);
//            // 继续处理转换后的值
//        } else {
//            ALOGE("Invalid numeric data: %s", data.c_str());
//            return; // 或处理错误情况
//        }
//    } catch (const std::invalid_argument& e) {
//        ALOGE("stoi conversion failed: %s", e.what());
//        // 错误处理逻辑
//    } catch (const std::out_of_range& e) {
//        ALOGE("stoi out of range: %s", e.what());
//        // 范围错误处理
//    }
//}
