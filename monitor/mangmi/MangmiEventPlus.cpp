/*
 *
 * Created by xshx on 2025/5/30.
 * 抽象出投递者类
 * 职责是 负责事件的拉取和处理
*/

#include "MangmiEventPlus.h"
#include "MangmiPolicy.h"
#include "utils/MangmiUtils.h"
#include "InputFilter.h"


MangmiEventPlus *MangmiEventPlus::instance= nullptr;
std::vector<RawEvent> MangmiEventPlus::eventKeys; //
std::vector<RawEvent> MangmiEventPlus::eventAxis;//
bool MangmiEventPlus::running=false;

MangmiEventPlus* MangmiEventPlus::getInstance() {
    if( instance == nullptr ){
        instance = new MangmiEventPlus();
    }
    return  instance;
}
MangmiEventPlus::MangmiEventPlus( ){

    MangmiUtils::initInputIdMaps();
}

pthread_t MangmiEventPlus::startEventThread( ){
    ALOGD("启动 MangmiEventPlus 线程");

    pthread_t pthreadId;

    pthread_create(&pthreadId, nullptr, start, this);
    return  pthreadId;
}
void MangmiEventPlus::stop(){
    ALOGD("Stop MangmiEventPlus");
    running = false;
}

void* MangmiEventPlus::start(void *args) {
    ALOGD("启动 MangmiEventPlus");

//    updateInputIdMaps();
    MangmiEventPlus *self = static_cast<MangmiEventPlus*>(args);
    ////  先创建策略线程 收来自客户端的配置投递
    try {
        pthread_t policyThread;
        policyThread= MangmiPolicy::getInstance()->startSocketServerThread();
    }catch(const std::exception& e){
        std::cerr<<e.what()<<std::endl;
    }

    running = true;
    while(running) {
        InputFilter::getInstance()->pullInputEvents(eventKeys, eventAxis, 2000);

        if(!eventKeys.empty())
        {
            self->handleKeyEvents(eventKeys);
        }
        if(!eventAxis.empty())
        {
            self->handleAxisEvents( eventAxis);
        }
    }
    return nullptr;
}

void MangmiEventPlus::handleKeyEvents(std::vector<RawEvent>& events){
    ALOGD("keyEvents ");
    unsigned int size = events.size();
    for (unsigned int i = 0; i < size; i++)
    {
        RawEvent& event = events[i];
        handleKeyEvent(event);
    }
    return;
}
void MangmiEventPlus::handleKeyEvent(RawEvent event) {
    ALOGD("keyEvent deviceId:%d, type:%d,code:%d,value:%d ",event.deviceId, event.type, event.code,event.value);
    MangmiPolicy::getInstance()->handleKeyPlus( event);
    ALOGD("keyEvent over \n\n");
}

void MangmiEventPlus::handleAxisEvents(std::vector<RawEvent>& events){
    ALOGD("axisEvents");
    unsigned int size = events.size();
    for (unsigned int i = 0; i < size; i++)
    {
        RawEvent& event = events[i];
        handleAxisEvent(event);
    }
    return;
}

void MangmiEventPlus::handleAxisEvent(RawEvent event) {
    ALOGD("axisEvent");
    MangmiPolicy::getInstance()->handleAxisPlus(event);
    ALOGD("axisEvent over \n\n");
}
