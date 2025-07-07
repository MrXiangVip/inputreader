//
// Created by xshx on 2025/5/30.
//

#ifndef UNTITLED_EVENTHUB_H
#define UNTITLED_EVENTHUB_H

#include <cstdint>

//#define  EV_KEY  1
//#define  EV_ABS  3
//#define  EV_SYN  4

struct RawEvent {
    // Time when the event happened
    int64_t when;
    // Time when the event was read by EventHub. Only populated for input events.
    // For other events (device added/removed/etc), this value is undefined and should not be read.
    int64_t readTime;
    int32_t deviceId;
    int32_t type;
    int32_t code;
    int32_t value;
};



#endif //UNTITLED_EVENTHUB_H
