//
// Created by xshx on 2025/6/1.
//

#include <vector>
#include <sys/types.h>
#include <pthread.h>
#include <thread>
#include <csignal>
#include "InputFilter.h"
#include "../ include/EventHub.h"
#include "Macro.h"
#include "utils/MangmiUtils.h"
#include <sys/time.h>

InputFilter* InputFilter::instance;

InputFilter::InputFilter(){
//    pthread_mutex_init(&mutex, NULL);
//    pthread_cond_init(&condEvent, NULL);//

    mutex =PTHREAD_MUTEX_INITIALIZER;
    condEvent = PTHREAD_COND_INITIALIZER;
}

InputFilter* InputFilter::getInstance( ){
    if( instance == nullptr ){
        instance = new InputFilter();
    }
    return instance;
}

pthread_t InputFilter::startInputMonitor( ){
    ALOGD("创建 输入事件线程");
    pthread_t threadId;
    threadId =pthread_create( &threadId, nullptr, inputEventMonitor, this);
    return  threadId;
}

void *InputFilter::inputEventMonitor( void *args){
    ALOGD("inputEventMonitor ");
    InputFilter *self = static_cast<InputFilter*>(args);
    int value=0;
    while( true){
/* 按下横轴右键
        RawEvent event;
        event.deviceId=1;
        event.type=EV_ABS;
        event.code = ABS_HAT0X;
        event.value =1;
*/

/*  A键*/
//        RawEvent event;
//        event.deviceId =1;
//        event.type=EV_KEY;
//        event.code=BUTTON_A;
//        event.value=1;
        RawEvent event;
        event.deviceId =2;
        event.type=EV_ABS;
        event.code=ABS_X;
        event.value=100;
        self->inputRawEvent( event);
        sleep(10);// 每隔5秒钟发送一次事件
    }

}

std::vector<RawEvent> InputFilter::inputRawEvent(RawEvent& event)
{
    ALOGD("thread:%d 投递一个事件 deviceId:%d, type:%d, code:%d, value:%d",std::this_thread::get_id(), event.deviceId, event.type, event.code, event.value);
    std::vector<RawEvent> rawEvents;
    rawEvents.push_back(event);
    return handleRawEvents( rawEvents );
}

std::vector<RawEvent> InputFilter::handleRawEvents(std::vector<RawEvent>& events){

    unsigned int size = events.size();
    for (unsigned int i = 0; i < size; i++)
    {
        RawEvent& event = events[i];
        switch (event.type)
        {
            case EV_KEY:
                handleKeyEvent(event);
                break;
            case EV_ABS:
                handleAbsEvent(event);
                break;
            case EV_SYN:
                handleSynEvent(event);
                break;
            default:
                pushEvent(event);
        }
    }
    return  events;
}

void InputFilter::handleKeyEvent(RawEvent& event){
    keyEvents.push_back( event);
    pthread_cond_signal(&condEvent);
    return ;
}


void InputFilter::handleAbsEvent(RawEvent& event){
    axisEvents.push_back( event);
    pthread_cond_signal(&condEvent);
    return;
}

void InputFilter::handleSynEvent(RawEvent& event){

}

void InputFilter::pushEvent(const RawEvent& event){

}

void InputFilter::pushSoftEvent(int id, int action, int x, int y)
{
    ALOGD("pushSoftEvent id:%d, action:%d, x:%d, y:%d", id, action, x, y);
//    filter.postSoftEvent(id, action, x, y);
}

void InputFilter::pushSoftEvent(RawEvent event) {
    ALOGD("pushSoftEvent id:%d,  type:%d, code:%d,value:%d", event.deviceId, event.type, event.code, event.value);
}
void InputFilter::pullInputEvents(std::vector<RawEvent>& keys, std::vector<RawEvent>& axes, int millisecond)
{
    pullEvents(keys, axes, millisecond);
}
/* 从输入事件队列中拉取按键和轴事件 */
void InputFilter::pullEvents(std::vector<RawEvent>& keys, std::vector<RawEvent>& axes, int millisecond)
{
    ALOGD("thread:%d :拉取事件", std::this_thread::get_id());
    unsigned long remain = (0 > millisecond) ? (unsigned long)(-1) : millisecond;
    do
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait( &condEvent, &mutex);//
        useconds_t begin = MangmiUtils::getSystemTimePosix();

        keys = keyEvents;
        keyEvents.clear();
        axes = axisEvents;
        axisEvents.clear();
        pthread_mutex_unlock( &mutex);

        if (!keys.empty() || !axes.empty())
        {
            break;
        }

        useconds_t end = MangmiUtils::getSystemTimePosix();
        unsigned long elapse = (end - begin) / 1000000;
        if (elapse >= remain)
        {
            break;
        }
        remain -= elapse;
    } while (0 < remain);
}

void InputFilter::setInputsFilter(const std::set<int>& keyCodes, const std::set<int>& axisCodes){
    return;
}