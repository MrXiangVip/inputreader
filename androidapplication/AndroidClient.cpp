//
// Created by xshx on 2025/6/1.
//

#include <netdb.h>
#include <iostream>
#include <csignal>
#include <cstring>
#include "AndroidClient.h"


AndroidClient* AndroidClient::client= nullptr;
AndroidClient *AndroidClient::getInstance( ){
    if(client == nullptr){
        client = new AndroidClient();
    }
    return client;
}

AndroidClient::AndroidClient( ){
//    client_socket = socket(AF_INET, SOCK_STREAM, 0);
//    if (client_socket == -1) {
//        std::cerr << "创建 socket 失败" << std::endl;
//    }

    // 设置服务端地址
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_port = htons(PORT); // 服务端端口

    // 将 IPv4 地址从点分十进制转换为二进制形式
//    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
//        std::cerr << "地址无效或不支持" << std::endl;
//        close(client_socket);
//    }

    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "创建 socket 失败" << std::endl;
    }

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof (addr.sun_path));

}
int AndroidClient::connectClientSocket( ) {

    if( client_socket ==-1){return -1;}
    // 连接服务端
    if (connect(client_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "连接服务端失败" << std::endl;
        close(client_socket);
        return -1;
    }
    std::cout << "已连接到服务端" << std::endl;
    return 0;
}

int AndroidClient::sendMessage(std::string message) {
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


int AndroidClient::sendMessageThenRelay(std::string send, std::string reply){

    return 0;
}