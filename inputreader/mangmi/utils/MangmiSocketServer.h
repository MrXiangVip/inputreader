//
// Created by xshx on 2025/6/17.
//

#ifndef INPUTREADER_MANGMISOCKETSERVER_H
#define INPUTREADER_MANGMISOCKETSERVER_H
#include <sys/un.h>
#include <sys/socket.h>

#define SOCKET_PATH "/tmp/inputreader"
class MangmiSocketServer {

public:
    static MangmiSocketServer* getInstance();
    int runServer();

    bool running;

    int startServer();

private:
    int sockfd;
    sockaddr_un addr{};
    static MangmiSocketServer *instance;
    MangmiSocketServer();

    int createSocketServer();

    int dealReceivedData(std::string receivedData);

    int replyData(std::string requestId, std::string data);
};


#endif //INPUTREADER_MANGMISOCKETSERVER_H
