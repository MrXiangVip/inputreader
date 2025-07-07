//
// Created by xshx on 2025/5/30.
//

#ifndef UNTITLED_MANGMIPOLICY_H
#define UNTITLED_MANGMIPOLICY_H

#include <string>
#include <vector>
#include <atomic>
#include <pthread.h>
#include <set>
#include "Macros.h"
#include "../include/EventHub.h"
#include "utils/MangmiSocketClient.h"
#include "utils/MangmiConfig.h"
#include "utils/MiThreadPool.h"

#define BUFFER_SIZE 1024 * 64 //64k

#define KEY_CATEGORY_GAMEPAD 1000
#define KEY_TYPE_GAMEPAD_CLICK_STANDARD (KEY_CATEGORY_GAMEPAD + 1)


#define LEFT_ASSOCIATE_JOYSTICK 1
#define RIGHT_ASSOCIATE_JOYSTICK 2
#define SMART_ASSOCIATE_JOYSTICK 3

#define LEFT_MAP_VIEW_MODE 4
#define RIGHT_MAP_VIEW_MODE 5

struct socketReply {
    socketReply(int i, int i1);

    socketReply(int);

    int requestId;
    int inputId;
};

class MangmiPolicy {

public:
    pthread_t startSocketServerThread( );  //创程
    static int replyApplication(int inputId, RawEvent event);// 回复客户端socket
    void buildKeyEvent(RawEvent event); //根据 下发的策略生成新的事件
    void buildAxisEvent(RawEvent event);
    static MangmiPolicy* getInstance();
    static std::vector<socketReply> vectorReply;
    void stop();
protected:
    static void* startMangmiSocket(void *args);

    static int replyClient(int requestId, std::string data);

    static RawEvent generateEvent( int32_t deviceId, int i, int code, int32_t value);
    //

private:
    MangmiPolicy( );
    static MangmiPolicy *instance;
    static MiThreadPool mangmiPool;
    bool iCancelSkill;// 取消技能
    std::map<int, std::set<JoystickConfig>>  associateJoystickMap;//指向性技能
    bool mapViewMode;//调整视角模式
    static int mWidth;
    static int mHeight;

    static std::atomic<bool>  AtomicComboThreadExit;// 按键连击开关
    static std::atomic<bool> AtomicScreenComboThreadExit;//触屏连击开关

    void leftJoystick(RawEvent& event);

    void rightJoystick(RawEvent event);

    void absHatXY(RawEvent &event);

    void absGas(RawEvent &event);

    void absBreak(RawEvent &event);

    void touchScreensStandardClickWhenPress(RawEvent &event, std::vector<KeyConfig> keyConfigs);
    void touchScreenComboClickWhenPress(RawEvent &event, std::vector<KeyConfig> keyConfigs);
    static void screenComboClick( RawEvent rawEvent, KeyConfig keyConfig);

    void standardKeyClick(RawEvent &event,  std::vector<KeyConfig> keyConfigs);

    void standardKeyComboClick(RawEvent &event,  std::vector<KeyConfig> keyConfigs);

    static void keyBoardComboClick(RawEvent rawEvent, KeyConfig keyConfig);

    void leftVirtualJoystick(RawEvent event,  vector<JoystickConfig> joystickConfigs);

    void touchScreenFastComboClickWhenPress(RawEvent rawEvent, vector<KeyConfig> keyConfigs);

    void touchScreenClickWhenRelease(RawEvent rawEvent, vector<KeyConfig> keyConfigs);

    void mobaAssociateLeftJoystickWhenPress(RawEvent rawEvent, vector<KeyConfig> keyConfigs);

    void mobaAssociateRightJoystickWhenPress(RawEvent event, vector<KeyConfig> keyConfigs);

    void mobaAssociateSmartJoystickWhenPress(RawEvent event, vector<KeyConfig> keyConfigs);

    void mobaCancelSkillWhenPress(RawEvent event, vector<KeyConfig> keyConfigs);

    void mobaViewMapWhenPress(RawEvent event, vector<KeyConfig> keyConfigs);

    void rightVirtualJoystick(RawEvent event, vector<JoystickConfig> joystickConfigs);

    void addJoyStickMapFromKeyConfig(int associateType, KeyConfig keyConfig);

    void removeJoyStickMapByKeyConfig(int associateType, KeyConfig keyConfig);
};


#endif //UNTITLED_MANGMIPOLICY_H
