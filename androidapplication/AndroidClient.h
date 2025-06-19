//
// Created by xshx on 2025/6/1.
//

#ifndef ANDROIDAPPLICATION_ANDROIDCLIENT_H
#define ANDROIDAPPLICATION_ANDROIDCLIENT_H
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define PORT 8080

#define SOCKET_PATH "/tmp/inputreader"
class AndroidClient {

public:
    static AndroidClient *getInstance();
    int connectClientSocket(  );
    int connectLocalClientSocket();

    int closeClientSocket();
    int sendMessage( std::string message);

private:
    int client_socket;
    struct sockaddr_in  addrIn;
    struct sockaddr_un  addr;

    static AndroidClient *client;
    AndroidClient();

    int sendMessageThenRelay(std::string send, std::string reply);
};


#endif //ANDROIDAPPLICATION_ANDROIDCLIENT_H
