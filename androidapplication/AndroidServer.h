//
// Created by xshx on 2025/6/14.
//

#ifndef ANDROIDAPPLICATION_ANDROIDSERVER_H
#define ANDROIDAPPLICATION_ANDROIDSERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sys/un.h>

#define PORT 8088
#define BUFFER_SIZE 1024 * 64 //64k

class AndroidServer {
public:
    static AndroidServer* getInstance();
    int createServer();
    int runServer();

protected:
    int sendMessage(int client_socket,std::string message);

private:
    int server_fd;
    sockaddr_in address{};
    sockaddr_un addr{};
    static AndroidServer *instance;
    AndroidServer();
};


#endif //ANDROIDAPPLICATION_ANDROIDSERVER_H
