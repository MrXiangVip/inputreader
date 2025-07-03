//
// Created by xshx on 2025/6/1.
//

#ifndef UNTITLED_INPUTFILTER_H
#define UNTITLED_INPUTFILTER_H

#include <pthread.h>

#include <set>
#include <map>
#include "../include/EventHub.h"

class InputFilter {

public:
    pthread_t startInputMonitor();
    void pullInputEvents(std::vector<RawEvent>& keys, std::vector<RawEvent>& axes, int millisecond);
    void pushSoftEvent(int id, int action, int x, int y);
    void pushSoftEvent(RawEvent event);

    /**
     * 设置需要过滤(屏蔽)的输入
     * @param keyCodes  设置被过滤的按键键值
     * @param axisCodes 设置被过滤的摇杆键值: ABS_X:0x00, ABS_Y:0x01, ABS_Z:0x02, ABS_RZ:0x05
     */
//    void setInputsFilter(const std::set<int>& keyCodes, const std::set<int>& axisCodes);
    void setInputsFilter(const std::set<int>& keyCodes, const std::map<int, int>& axisCodes);

    static InputFilter *getInstance( );
    static int mWidth, mHeight;
    std::vector<RawEvent> inputRawEvent(RawEvent &events);
//    void setFilter(const std::set<int> &keyCodes, const std::set<int> &axisCodes);
    void setFilter(const std::set<int> &keyCodes, const std::map<int,int> &axisCodes);
protected:
    std::vector<RawEvent> handleRawEvents(std::vector<RawEvent> &events);
    void handleKeyEvent(RawEvent &event);
    void handleAbsEvent(RawEvent &event);
    void handleSynEvent(RawEvent &event);
    void pushEvent(const RawEvent &event);
    static void *inputEventMonitor(void *args);
    void pullEvents(std::vector<RawEvent> &keys, std::vector<RawEvent> &axes, int millisecond);

    void handleTpEvent(RawEvent& event);
    void handleAxisEvent(RawEvent& event);

private:
    InputFilter();
    static InputFilter *instance;
    pthread_mutex_t       mutex;
    pthread_cond_t        condEvent;//
    //    Signal inputSignal;
    std::vector<RawEvent> keyEvents;
    std::vector<RawEvent> axisEvents;

    std::set<int> keyFilter;
//    std::set<int> axisFilter;
    std::map<int, int> axisFilter;
};


#endif //UNTITLED_INPUTFILTER_H
