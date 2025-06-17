//
// Created by xshx on 2025/6/1.
//

#ifndef UNTITLED_INPUTFILTER_H
#define UNTITLED_INPUTFILTER_H

#include <pthread.h>

#include <set>
#include "../ include/EventHub.h"

class InputFilter {

public:
    pthread_t startInputMonitor();
    void pullInputEvents(std::vector<RawEvent>& keys, std::vector<RawEvent>& axes, int millisecond);
    void pushSoftEvent(int id, int action, int x, int y);
    void pushSoftEvent(RawEvent event);
    void setInputsFilter(const std::set<int>& keyCodes, const std::set<int>& axisCodes);
    static InputFilter *getInstance( );
protected:
    std::vector<RawEvent> inputRawEvent(RawEvent &events);
    std::vector<RawEvent> handleRawEvents(std::vector<RawEvent> &events);
    void handleKeyEvent(RawEvent &event);
    void handleAbsEvent(RawEvent &event);
    void handleSynEvent(RawEvent &event);
    void pushEvent(const RawEvent &event);
    static void *inputEventMonitor(void *args);
    void pullEvents(std::vector<RawEvent> &keys, std::vector<RawEvent> &axes, int millisecond);

private:
    InputFilter();
    static InputFilter *instance;
    pthread_mutex_t       mutex;
    pthread_cond_t        condEvent;//
    //    Signal inputSignal;
    std::vector<RawEvent> keyEvents;
    std::vector<RawEvent> axisEvents;


};


#endif //UNTITLED_INPUTFILTER_H
