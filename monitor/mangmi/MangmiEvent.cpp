/*
 *
 * Created by xshx on 2025/5/30.
 * 抽象出投递者类
 * 职责是 负责事件的拉取和处理
*/

#include "MangmiEvent.h"
#include "MangmiPolicy.h"
#include "utils/MangmiUtils.h"
#include "InputFilter.h"


MangmiEvent *MangmiEvent::instance= nullptr;
std::vector<RawEvent> MangmiEvent::eventKeys; //
std::vector<RawEvent> MangmiEvent::eventAxis;//
bool MangmiEvent::running=false;

MangmiEvent* MangmiEvent::getInstance() {
    if( instance == nullptr ){
        instance = new MangmiEvent();
    }
    return  instance;
}
MangmiEvent::MangmiEvent( ){

    MangmiUtils::initInputIdMaps();
}

pthread_t MangmiEvent::startEventThread( ){
    ALOGD("启动 MangmiEvent 线程");

    pthread_t pthreadId;

    pthread_create(&pthreadId, nullptr, start, this);
    return  pthreadId;
}
void MangmiEvent::stop(){
    ALOGD("Stop MangmiEvent");
    running = false;
}

void* MangmiEvent::start(void *args) {
    ALOGD("启动 MangmiEvent");

//    updateInputIdMaps();
    MangmiEvent *self = static_cast<MangmiEvent*>(args);
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
}

void MangmiEvent::handleKeyEvents(std::vector<RawEvent>& events){
    ALOGD("处理按键事件 ");
    unsigned int size = events.size();
    for (unsigned int i = 0; i < size; i++)
    {
        RawEvent& event = events[i];
        handleKeyEvent(event);
    }
    return;
}
void MangmiEvent::handleKeyEvent(RawEvent event) {
    ALOGD("处理按键事件 deviceId:%d, type:%d,code:%d,value:%d ",event.deviceId, event.type, event.code,event.value);
    MangmiPolicy::getInstance()->buildKeyEvent( event);
    ALOGD("处理按键事件 over \n\n");
}

void MangmiEvent::handleAxisEvents(std::vector<RawEvent>& events){
    ALOGD("处理摇杆事件s");
    unsigned int size = events.size();
    for (unsigned int i = 0; i < size; i++)
    {
        RawEvent& event = events[i];
        handleAxisEvent(event);
    }
    return;
}

void MangmiEvent::handleAxisEvent(RawEvent event) {
    ALOGD("处理摇杆事件");
    MangmiPolicy::getInstance()->buildAxisEvent(event);
    ALOGD("处理摇杆事件 over \n\n");
}
