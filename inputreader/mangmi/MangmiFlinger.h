//
// Created by xshx on 2025/5/30.
//

#ifndef UNTITLED_MANGMIFLINGER_H
#define UNTITLED_MANGMIFLINGER_H

#include <map>
#include <vector>
#include "utils/MangmiUtils.h"
#include "Macros.h"
#include "../ include/EventHub.h"
#include "utils/MiThreadPool.h"
#include "MangmiPolicy.h"

class MangmiFlinger {

public:
    pthread_t startFlingerThread();
    static void* start(void *args);
    static MangmiFlinger* getInstance();
protected:
    void handleKeyEvents(std::vector<RawEvent>& events );
    void handleAxisEvents(std::vector<RawEvent>& events );
    void handleKeyEvent(RawEvent events );
    void handleAxisEvent(RawEvent events );


private:
    MangmiFlinger( );
    static MangmiFlinger *instance;//
    static std::vector<RawEvent> eventKeys; //
    static std::vector<RawEvent> eventAxis;//

};


#endif //UNTITLED_MANGMIFLINGER_H
