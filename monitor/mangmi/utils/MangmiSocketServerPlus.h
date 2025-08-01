//
// Created by xshx on 2025/6/17.
//

#ifndef INPUTREADER_MANGMISOCKETSERVER_H
#define INPUTREADER_MANGMISOCKETSERVER_H
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SOCKET_PATH "/tmp/inputreader"
class MangmiSocketServerPlus {

public:
    static MangmiSocketServerPlus* getInstance();

    bool running;

    int startServer();
    void stop();

private:
    int sockfd;
    sockaddr_un addr{};
    sockaddr_in addrIn{};
    static MangmiSocketServerPlus *instance;
    MangmiSocketServerPlus();

    int createLocalSocketServer();
    int createSocketServer();
    int dealReceivedData(std::string receivedData);

    int replyData(std::string requestId, std::string data);

};


#endif //INPUTREADER_MANGMISOCKETSERVER_H
