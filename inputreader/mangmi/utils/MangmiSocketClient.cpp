//
// Created by xshx on 2025/5/30.
//

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include "MangmiSocketClient.h"

#define PORT 8088

#define BUFFER_SIZE 1024 * 64 //64k

MangmiSocketClient* MangmiSocketClient::instance=NULL;
MangmiSocketClient::MangmiSocketClient() : client_fd(-1), running(false) {}


MangmiSocketClient::~MangmiSocketClient() {
    //stop();
}

MangmiSocketClient *MangmiSocketClient::getInstance(){
    if( instance ==NULL ){
        instance = new MangmiSocketClient();
    }
    return instance;
}

void MangmiSocketClient::connectToServer(const std::string& ip) {
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        ALOGI("socket creation failed");
        return;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        ALOGI("Invalid address/ Address not supported");
        close(client_fd);
        client_fd = -1;
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        ALOGI("Connection Failed");
        close(client_fd);
        client_fd = -1;
        return;
    }

    //ALOGI("---MangmiSocketClient---connectToServer success");
    running = true;
    //client_thread = std::thread(&MangmiSocketClient::runClient, this);
}

void MangmiSocketClient::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        running = false;
    }
    condition.notify_all();

    //if (client_thread.joinable()) {
    //client_thread.join();
    //}

    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
    }
}

int MangmiSocketClient::sendMessage(const std::string& message) {
    int iRet = 0;

    iRet = -1;
    if (client_fd != -1) {
        //ALOGI("---MangmiSocketClient---sendMessage(%d): %s", message.size(), message.c_str());
        iRet = send(client_fd, message.c_str(), message.size(), 0);
        if(iRet == (int)message.size())
            iRet = 0;
        else {
            ALOGI("---MangmiSocketClient---sendMessage---error");
            iRet = -1;
        }
    }

    return iRet;
}

int MangmiSocketClient::receiveMessage(int requestId) {
    int iRet = 0;
    std::string received_data;
    char buffer[BUFFER_SIZE] = {0};

    // 不断接收数据直到遇到以“@”结尾的数据包
    received_data.clear();
    while (running) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(client_fd, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            break;
        }

        ALOGI("---MangmiSocketClient---Message from server: %s", buffer);

        received_data += std::string(buffer, valread);

        // 检查是否以“@”结尾
        if (!received_data.empty() && received_data.back() == '@') {
            break;
        }
    }

    iRet = DealWith_DeceivedData(requestId, received_data);

    return iRet;
}
int MangmiSocketClient::DealWith_DeceivedData(int requestId, std::string received_data) {
    int iRet;
    std::string cRequestId;
    std::string cDataLen;
    std::string cData;
    std::vector <std::string> data_segments;
    size_t start_pos = 0;

    //请求
    //请求ID#接口ID（1880）#数据长度#配置参数(json格式)#@

    //回复
    //请求ID#数据长度#回复数据区#@

    // 以“#”划分接收到的数据
    size_t end_pos = received_data.find('#');

    while (end_pos != std::string::npos) {
        data_segments.push_back(received_data.substr(start_pos, end_pos - start_pos));
        start_pos = end_pos + 1;
        end_pos = received_data.find('#', start_pos);
    }

    // 处最后一理段数据（如果有的话）
    if (start_pos < received_data.length() - 1) {
        data_segments.push_back(received_data.substr(start_pos));
    }

    //ALOGI("---Received data segments:");
    //for (const auto& segment : data_segments) {
    //  ALOGI("---data segment(%d): %s", segment.length(), segment.c_str());
    //}

    for (size_t i = 0; i < data_segments.size(); ++i) {
        //ALOGI("---data_segments(%d):len = %d, data: %s", i, data_segments[i].length(), data_segments[i].c_str());
        if(0 == i) {
            cRequestId = data_segments[i].c_str();
        }else if(1 == i) {
            cDataLen = data_segments[i].c_str();
        }else if(2 == i) {
            cData = data_segments[i].c_str();
        }
    }

    ALOGI("---data_segments: cRequestId = %s, cDataLen = %s, cData: %s", cRequestId.c_str(), cDataLen.c_str(), cData.c_str());

    if(requestId == stoi(cRequestId))
        iRet = stoi(cData);
    else
        iRet = -1;

    return iRet;
}