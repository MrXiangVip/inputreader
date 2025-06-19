//
// Created by xshx on 2025/6/14.
//

#include "AndroidServer.h"
#include <cstring>
#include <iostream>
#include <csignal>

AndroidServer* AndroidServer::instance;
AndroidServer * AndroidServer::getInstance() {
    if( instance== nullptr){
        instance =new AndroidServer();
    }
}


AndroidServer::AndroidServer(){
    signal(SIGPIPE, SIG_IGN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr<<"socket failed"<<std::endl;
        return;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr<<"setsockopt failed"<<std::endl;
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
}

int AndroidServer::runServer( ){
    bool running = true;
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr<<"bind failed"<<std::endl;
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr<<"listen failed"<<std::endl;
        return -1;
    }
    while( running ){
       int  new_socket = accept(server_fd, nullptr, nullptr);
       char buffer[BUFFER_SIZE] = {0};

       int valread = read(new_socket, buffer, BUFFER_SIZE);
       std::cerr<<buffer<<std::endl;
        std::string message ="0#1#0#@";
       sendMessage(new_socket, message );

    }
}

int AndroidServer::sendMessage(int client_socket,std::string message) {
    // 要发送的字符串
    // 发送数据
    ssize_t bytes_sent = send(client_socket, message.c_str(), message.length(), 0);
    if (bytes_sent == -1) {
        std::cerr << "发送数据失败" << std::endl;
    } else {
        std::cout << "已发送数据: " << message << std::endl;
    }
    return 0;
}
