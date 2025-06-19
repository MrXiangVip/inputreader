//
// Created by xshx on 2025/5/30.
//

#ifndef UNTITLED_MANGMISOCKETCLIENT_H
#define UNTITLED_MANGMISOCKETCLIENT_H


#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../Macros.h"
class MangmiSocketClient {
public:
    static MangmiSocketClient *getInstance();
    void connectToServer(const std::string& ip);
    void stop();
    int sendMessage(const std::string& message);
    int receiveMessage(int requestId);
    int DealWith_DeceivedData(int requestid, std::string received_data);
    bool running;
private:

    MangmiSocketClient();
    ~MangmiSocketClient();

    static MangmiSocketClient *instance;

    int client_fd;
    std::thread client_thread;
    std::mutex mutex;
    std::condition_variable condition;
};


#endif //UNTITLED_MANGMISOCKETCLIENT_H
