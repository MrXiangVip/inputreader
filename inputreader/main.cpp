#include <iostream>
#include "mangmi/utils/MangmiIntercepter.h"
#include "mangmi/Macro.h"
#include "mangmi/MangmiPolicy.h"
#include "mangmi/MangmiFlinger.h"
#include "mangmi/InputFilter.h"

int main() {
    InterceptKey interceptKey(1880,100, EV_ABS);
    InterceptKey interceptKey2(1880,100, EV_KEY);
    InterceptKey interceptKey3(1882,100, EV_ABS);
    InterceptKey interceptKey4(1882,100, EV_KEY);
    MangmiIntercepter::insertInterceptEventKey(interceptKey);
    MangmiIntercepter::insertInterceptEventKey(interceptKey);
    MangmiIntercepter::insertInterceptEventKey(interceptKey2);
    MangmiIntercepter::insertInterceptEventKey(interceptKey3);
    MangmiIntercepter::insertInterceptEventKey(interceptKey4);
    MangmiIntercepter::toString();
    std::set<int> axisSet=MangmiIntercepter::getInterceptEventByAxisType();
    std::set<int> keySet =MangmiIntercepter::getInterceptEventByKeyType();
    MangmiIntercepter::eraseInterceptEventBySerialNo(1880);
    MangmiIntercepter::toString();
    MangmiIntercepter::eraseInterceptEventKeyByInterceptKey(interceptKey2);
    MangmiIntercepter::toString();
    std::cout << "Hello, World!" << std::endl;

    bool flag = MangmiUtils::isNumeric("22");

////  先创建策略线程 收来自客户端的配置投递
    pthread_t policyThread;
    try {
        policyThread= MangmiPolicy::getInstance()->startSocketServerThread();
    }catch(const std::exception& e){
        std::cerr<<e.what()<<std::endl;
    }
//
////  再启动接收投递 flinger 线程
    pthread_t flingerThread;
    try{
        flingerThread= MangmiFlinger::getInstance()->startFlingerThread();
    }catch (const std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
//
////  创建 filter 线程作为 输入事件的来源
    pthread_t inputThread;
    try{
        inputThread = InputFilter::getInstance( )->startInputMonitor();
    }catch (const std::exception  &e){
        std::cerr<<e.what()<<std::endl;
    }

//
    pthread_join( policyThread, NULL);
    pthread_join( flingerThread, NULL);
    pthread_join( inputThread, NULL);
    ALOGD("all over \n");
//    return 0;
}




