//
// Created by xshx on 2025/6/17.
//

#include <string>
#include <csignal>
#include <vector>
#include "MangmiSocketServerPlus.h"
#include "../Macros.h"
#include "MangmiConfig.h"
#include "../MangmiPolicy.h"
#include "MangmiIntercepter.h"
#define BUFFER_SIZE 1024 * 64 //64k
#define PORT 8080

MangmiSocketServerPlus* MangmiSocketServerPlus::instance;
MangmiSocketServerPlus * MangmiSocketServerPlus::getInstance() {
    if( instance== nullptr){
        instance =new MangmiSocketServerPlus();
    }
    return instance;
}

MangmiSocketServerPlus::MangmiSocketServerPlus() {
    ALOGD("MangmiSocketServerPlus");

    createSocketServer();
}

int MangmiSocketServerPlus::createSocketServer(){
    ALOGD("createSocketServer");
    std::string received_data;


    signal(SIGPIPE, SIG_IGN);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0) {
        ALOGI("socket failed");
        return -1;
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        ALOGI("setsockopt failed");
        return -1;
    }

    addrIn.sin_family = AF_INET;
    addrIn.sin_addr.s_addr = INADDR_ANY;
    addrIn.sin_port = htons(PORT);

    if (::bind(sockfd, (struct sockaddr*)&addrIn, sizeof(addrIn)) < 0) {
        ALOGI("bind failed");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 3) < 0) {
        ALOGI("listen failed");
        close(sockfd);
        return -1;
    }
    running = true;//
    return 0;
}

int MangmiSocketServerPlus::createLocalSocketServer(){

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if( sockfd==-1){
        ALOGD("ERROR Create SOCKET");
        return -1;
    }
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path));

    unlink(addr.sun_path);
    if( ::bind(sockfd, (struct sockaddr*)&addr, sizeof(addr))==-1){
        ALOGD("BIND ERROR");
        return -1;
    }
    if( listen(sockfd,5)==-1){ALOGD("LISTEN ERROR");}

    running = true;
    return 0;
}
void MangmiSocketServerPlus::stop( ){
    running = false;
    close( sockfd);
    return;
}
int MangmiSocketServerPlus::startServer( ){
    char buffer[BUFFER_SIZE] = {0};
    while (running) {
        ALOGD("waiting client");
        int clientfd = accept(sockfd, nullptr, nullptr);
        if (clientfd < 0) {
            ALOGI("accept failed");
            break;
        }
        std::string received_data;

        // 不断接收数据直到遇到以“@”结尾的数据包
        received_data.clear();
        memset(buffer, 0, sizeof(buffer));
        while (true) {
            int valread = read(clientfd, buffer, BUFFER_SIZE);
            if (valread <= 0) {
                break; // 连接关闭
            }

            received_data += std::string(buffer, valread);

            // 检查是否以“@”结尾
            if (!received_data.empty() && received_data.back() == '@') {
                break;
            }

            // 清空缓冲区以便下一次读取
            memset(buffer, 0, sizeof(buffer));
//
        } //while (true)
        dealReceivedData( received_data );

    }
    close(sockfd);
    sockfd = -1;
    return sockfd;
}
int MangmiSocketServerPlus::dealReceivedData(std::string receivedData) {
    ALOGD("dealReceivedData %s", receivedData.c_str());
    int iRet=0;
    size_t start_pos = 0;
    std::vector<std::string> data_segments;

    size_t end_pos = receivedData.find('#');

    while (end_pos != std::string::npos) {
        data_segments.push_back(receivedData.substr(start_pos, end_pos - start_pos));
        start_pos = end_pos + 1;
        end_pos = receivedData.find('#', start_pos);
    }

    // 处理最后一段数据（如果有的话）
    if (start_pos < receivedData.length() - 1) {
        data_segments.push_back(receivedData.substr(start_pos));
    }

    std::string cRequestId;
    std::string cSerialNo;
    std::string cDataLen;
    std::string cData;

    for (size_t i = 0; i < data_segments.size(); ++i) {
//        ALOGI("---data_segments(%d):len = %d, data: %s", i, data_segments[i].length(), data_segments[i].c_str());
        if (0 == i) {
            cRequestId = data_segments[i].c_str();
        } else if (1 == i) {
            cSerialNo = data_segments[i].c_str();
        } else if (2 == i) {
            cDataLen = data_segments[i].c_str();
        } else if (3 == i) {
            cData = data_segments[i].c_str();
        }
    }

    ALOGI("---data_segments: cRequestId = %s, cSerialNo = %s, cDataLen = %s, cData: %s", cRequestId.c_str(),
          cSerialNo.c_str(), cDataLen.c_str(), cData.c_str());
    if ("1880" == cSerialNo) { //设置当前映射配置
        MangmiConfig::getInstance()->releaseAllSlots();
        iRet =MangmiConfig::getInstance()->parseJson( cData);

//        iRet =MangmiPolicy::getInstance()->updateIdConfigs( );
    }else if( "1881" == cSerialNo){
        iRet = stoi(cRequestId);
    }else if("1882" == cSerialNo ){
//
        iRet = replyData( cRequestId, cData);
        if( 0== stoi(cDataLen) ){

        }
    }else{
        ALOGD("unkown serialNo %s", cSerialNo.c_str());
    }
    ALOGI("---DealWith_Deceiveddata---success");

//    更新拦截策略
    MangmiIntercepter::getInstance()->updateInterceptPolicy();
    return iRet;
}

int MangmiSocketServerPlus::replyData(std::string requestId, std::string data){
    ALOGD("replyData ");
    std::string token;
    char delimiter = '$';
    int ilen =0;
    for (char c : data) {
        if (c == delimiter) {
            if (!token.empty()) {
                ilen++;
                MangmiPolicy::vectorReply.emplace_back(socketReply(std::stoi(requestId), std::stoi(token))); // 将字符串转换为整数
                token.clear();
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        ilen++;
        MangmiPolicy::vectorReply.emplace_back(std::stoi(requestId), std::stoi(token)); // 将最后一个标记转换为整数
    }
    return ilen;
}