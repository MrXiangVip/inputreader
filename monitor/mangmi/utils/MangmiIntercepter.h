//
// Created by xshx on 2025/5/29.
//

#ifndef UNTITLED_MANGMIINTERCEPTER_H
#define UNTITLED_MANGMIINTERCEPTER_H


#include <set>

#include "../MangmiEventPlus.h"
#include "MangmiConfig.h"

//#define EV_KEY 1
//#define EV_ABS 2
/*
 *  拦截 事件的类
 * */
struct InterceptKey {
    InterceptKey(int mSerialNo, int mEvCode, int mType, int mValue) {
        serialNo =mSerialNo;
        evCode = mEvCode;
        type = mType;
        value =mValue;
    }

    int serialNo;// 1880,1882
    int evCode;  //
    int type;    //EV_KEY, EV_ABS
    int value;   //EV_ABS 还需要 value, 用来区分方向键
    bool operator<(const InterceptKey& other) const {
        if (serialNo != other.serialNo){
            return serialNo < other.serialNo;
        }
        if( type !=other.type){
            return type < other.type;
        }
        if( evCode !=other.evCode){
            return evCode <other.evCode;
        }
        return value < other.value;
    }
};

class MangmiIntercepter{

public:
    static MangmiIntercepter *getInstance();
    static int eraseInterceptEventBySerialNo(int serialNo );
    static int eraseInterceptEventKeyByInterceptKey( InterceptKey key );
    static int  insertInterceptEventKey( InterceptKey key);

    int insertInterceptKeyEventWithSerialNo(std::set<int> keySet, int serialNo);
    int insertInterceptAxisEventWithSerialNo(std::set<int> keySet, int serialNo);
    static void buildIntercepterByConfigs(GamepadConfig& gamepadConfig, int serialNo);//
	
    static std::set<int> getInterceptEventByType(int type);
    static std::set<int> getInterceptEventByKeyType();
//    static std::set<int> getInterceptEventByAxisType();
    static std::map<int,int> getInterceptEventByAxisType();


    static int getSize();
    static void toString();
    static int clearInputFilter();
//    static void enableInputsFilter( const std::set<int>& keyCodes, const std::set<int>& axisCodes);
    static void enableInputsFilter( const std::set<int>& keyCodes, const std::map<int, int>& axisCodes);

    int updateInterceptPolicy();

private:
    static MangmiIntercepter *instance;
    static std::set<InterceptKey> interceptKeySet;


//    static std::set<int> keySet;
//    static std::set<int> axisSet;

};

#endif //UNTITLED_MANGMIINTERCEPTER_H
