//
// Created by xshx on 2025/6/1.
//

#include <vector>
#include <sys/types.h>
#include <pthread.h>
#include <thread>
#include <csignal>
#include <linux/input.h>

#include "InputFilter.h"
#include "../include/EventHub.h"
#include "Macros.h"
#include "utils/MangmiUtils.h"
#include <sys/time.h>

InputFilter* InputFilter::instance;

int InputFilter::mWidth=1080;
int InputFilter::mHeight=1920;

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
        RawEvent event;
        event.deviceId =1;
        event.type=EV_KEY;
        event.code=BTN_A;
        event.value=1;
        sleep(10);
        self->inputRawEvent( event);
/* left yaogan */
        RawEvent event2;
        event2.deviceId =1;
        event2.type=EV_ABS;
        event2.code=ABS_X;
        event2.value=100;
        self->inputRawEvent( event2);
        sleep(10);// 每隔5秒钟发送一次事件
    }

}

std::vector<RawEvent> InputFilter::inputRawEvent(RawEvent& event)
{
    ALOGD("thread 投递一个事件 deviceId:%d, type:%d, code:%d, value:%d", event.deviceId, event.type, event.code, event.value);
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
    bool  filtered = (keyFilter.find(event.code)!=keyFilter.end());
    if( filtered ){ return;}
    pushEvent( event);
    return ;
}


void InputFilter::handleAbsEvent(RawEvent& event){
    switch (event.code)
    {
        case ABS_MT_SLOT:
        case ABS_MT_TRACKING_ID:
        case ABS_MT_TOUCH_MAJOR:
        case ABS_MT_WIDTH_MAJOR:
        case ABS_MT_POSITION_X:
        case ABS_MT_POSITION_Y:
        case ABS_MT_PRESSURE:
            handleTpEvent(event);
            break;
        case ABS_X:
        case ABS_Y:
        case ABS_Z:
        case ABS_RX:
        case ABS_RY:
        case ABS_RZ:
        case ABS_HAT0X:
        case ABS_HAT0Y:
        case ABS_BRAKE:
        case ABS_GAS:
            handleAxisEvent(event);
            break;
        default:
            pushEvent(event);
    }
    return;
}



void InputFilter::handleTpEvent(RawEvent& event){
    ALOGI("%s",__func__);
    return;
}
void InputFilter::handleAxisEvent(RawEvent& event){
    ALOGI("%s",__func__);
    axisEvents.push_back( event);
    pthread_cond_signal(&condEvent);
//    bool filtered = (axisFilter.find(event.code) != axisFilter.end());
    bool filtered=false;
    auto it =axisFilter.find(event.code);
    if( it!= axisFilter.end()){
        if( it->second==0){
            filtered=true;
        }else if( it->second == event.value ){
            filtered =true;
        }
    }
    if( filtered ){ return;}
    pushEvent(event);
    return;
}



void InputFilter::handleSynEvent(RawEvent& event){
    ALOGD("%s",__func__);
    return;
}

void InputFilter::pushEvent(const RawEvent& event){
    ALOGD("%s",__func__);
    return;
}

void InputFilter::pushSoftEvent(int id, int action, int x, int y)
{
    ALOGD("pushSoftEvent id:%d, action:%d, x:%d, y:%d", id, action, x, y);
//    filter.postSoftEvent(id, action, x, y);
    return;
}

void InputFilter::pushSoftEvent(RawEvent event) {
    ALOGD("pushSoftEvent id:%d,  type:%d, code:%d,value:%d", event.deviceId, event.type, event.code, event.value);
    return;
}
void InputFilter::pullInputEvents(std::vector<RawEvent>& keys, std::vector<RawEvent>& axes, int millisecond)
{
    pullEvents(keys, axes, millisecond);
}
/* 从输入事件队列中拉取按键和轴事件 */
void InputFilter::pullEvents(std::vector<RawEvent>& keys, std::vector<RawEvent>& axes, int millisecond)
{
    ALOGD("thread :拉取事件");
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
    return;
}

//void InputFilter::setInputsFilter(const std::set<int>& keyCodes, const std::set<int>& axisCodes){
void InputFilter::setInputsFilter(const std::set<int>& keyCodes, const std::map<int,int>& axisCodes){
    ALOGD("setInputsFilter ");
    setFilter( keyCodes, axisCodes);
    return;
}

//void InputFilter::setFilter(const std::set<int> &keyCodes, const std::set<int> &axisCodes){
void InputFilter::setFilter(const std::set<int> &keyCodes, const std::map<int, int> &axisCodes){
    ALOGI("%s",__func__);
    keyFilter = keyCodes;
    axisFilter = axisCodes;
    return;
}