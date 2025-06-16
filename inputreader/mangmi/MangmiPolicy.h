//
// Created by xshx on 2025/5/30.
//

#ifndef UNTITLED_MANGMIPOLICY_H
#define UNTITLED_MANGMIPOLICY_H

#include <string>
#include <vector>
#include "Macro.h"
#include "../ include/EventHub.h"
#include "utils/MangmiSocketClient.h"
#include "utils/MangmiConfig.h"

#define PORT 8080
#define BUFFER_SIZE 1024 * 64 //64k

#define KEY_CATEGORY_GAMEPAD 1000
#define KEY_TYPE_GAMEPAD_CLICK_STANDARD (KEY_CATEGORY_GAMEPAD + 1)

struct KeySlotConfig {
    int id;
    int slot;
    int type;
};
struct IdConfig {
    int id;
    int slot;
    std::string configStr;
};




struct socketReply {
    socketReply(int i, int i1);

    socketReply(int);

    int requestId;
    int inputId;
};

class MangmiPolicy {

public:
    pthread_t startSocketServerThread( );  //创程
    static int replyPolicy(int inputId, RawEvent event);// 回复客户端socket
    void buildKeyEvent(RawEvent event); //根据 下发的策略生成新的事件
    void buildAxisEvent(RawEvent event);
    static MangmiPolicy* getInstance();
protected:
    void runServer();
    static void* startMangmiSocket(void *args);
    int dealReceivedData(std::string receivedData );
    int replyData(std::string requestId, std::string data);

    int updateInterceptPolicy( );
    static int replyClient(int requestId, std::string data);

    static void key_CG_Standard( RawEvent event, std::vector<KeyConfig> ctsKConfigs );
    static RawEvent& generateEvent( int32_t deviceId, int i, int code, int32_t value);

    int updateIdConfigs( );
private:
    MangmiPolicy( );
    static MangmiPolicy *instance;
    int server_fd;// 套接字fd
    static bool running;//

    static MangmiSocketClient key_socket_client;//
    static std::vector<socketReply> vectorReply;//
    std::vector<KeySlotConfig> kSlotConfigs; //?
    static std::vector<IdConfig> idConfigs;//

    static int iSlotId;//
    static std::vector<int> jsLeftSlotId;
    static std::vector<int> jsRightSlotId;
    static int mWidth;
    static int mHeight;

    int assignIdConfig(std::string str, int idAddType);
    void assignKeySlotConfig(int inputId, int type);

    void leftJoystick(RawEvent& event);

    static std::vector<JoystickConfig> getJoystickConfigsFromId(int inputId);

    static int getSlotIdFromIdConfig(std::string str, int idAddType);

    void rightJoystick(RawEvent event);

    void absHatXY(RawEvent &event);

    void absGas(RawEvent &event);

    void absBreak(RawEvent &event);

    void handleKey(RawEvent &event);

    void convertToStandardClick(RawEvent &event, std::vector<KeyConfig> keyConfig);

    std::vector<int> getSlotIdFromKeySlotConfig(int id, int type);
};


#endif //UNTITLED_MANGMIPOLICY_H
