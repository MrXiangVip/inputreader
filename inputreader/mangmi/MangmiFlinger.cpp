/*
 *
 * Created by xshx on 2025/5/30.
 * 抽象出投递者类
 * 职责是 负责事件的拉取和处理
*/

#include "MangmiFlinger.h"
#include "MangmiPolicy.h"
#include "utils/MangmiUtils.h"
#include "InputFilter.h"


MangmiFlinger *MangmiFlinger::instance= nullptr;
std::vector<RawEvent> MangmiFlinger::eventKeys; //
std::vector<RawEvent> MangmiFlinger::eventAxis;//


MangmiFlinger* MangmiFlinger::getInstance() {
    if( instance == nullptr ){
        instance = new MangmiFlinger();
    }
    return  instance;
}
MangmiFlinger::MangmiFlinger( ){

    MangmiUtils::initInputIdMaps();
}

pthread_t MangmiFlinger::startFlingerThread( ){
    ALOGD("启动 mangmi flinger 线程");

    pthread_t pthreadId;

    pthread_create(&pthreadId, nullptr, start, this);
    return  pthreadId;
}


void* MangmiFlinger::start(void *args) {
    ALOGD("启动 mangmi flinger");

//    updateInputIdMaps();
    MangmiFlinger *self = static_cast<MangmiFlinger*>(args);


    while(1) {
        InputFilter::getInstance()->pullInputEvents(eventKeys, eventAxis, 2000);

        if(!eventKeys.empty())
        {
//            handleKeyEvent();
            self->handleKeyEvents(eventKeys);
        }
        if(!eventAxis.empty())
        {
//            handleAxisEvent(eventAxis);
            self->handleAxisEvents( eventAxis);
        }
    }
}

void MangmiFlinger::handleKeyEvents(std::vector<RawEvent>& events){
    ALOGD("处理按键事件 ");
    unsigned int size = events.size();
    for (unsigned int i = 0; i < size; i++)
    {
        RawEvent& event = events[i];
        handleKeyEvent(event);
    }
    return;
}
void MangmiFlinger::handleKeyEvent(RawEvent event) {
    ALOGD("处理按键事件 deviceId:%d, type:%d,code:%d,value:%d ",event.deviceId, event.type, event.code,event.value);
    int inputId = MangmiUtils::getInputIdFromEvcode(event.code);
    MangmiPolicy::getInstance()->replyPolicy(inputId, event );
    MangmiPolicy::getInstance()->buildKeyEvent( event);
    ALOGD("处理按键事件 over \n\n");
}

void MangmiFlinger::handleAxisEvents(std::vector<RawEvent>& events){
    ALOGD("处理摇杆事件s");
    unsigned int size = events.size();
    for (unsigned int i = 0; i < size; i++)
    {
        RawEvent& event = events[i];
        handleAxisEvent(event);
    }
    return;
}

void MangmiFlinger::handleAxisEvent(RawEvent event) {
    ALOGD("处理摇杆事件");
//    std::vector<int> inputId = MangmiUtils::getInputIdFromEvcode( event.code);
//    if( inputId.size()==0){
//        return;
//    }
//    MangmiPolicy::getInstance()->replyPolicy(inputId[0], event);
    MangmiPolicy::getInstance()->buildAxisEvent(event);
    ALOGD("处理摇杆事件 over \n\n");
}
