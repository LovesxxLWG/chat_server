//
// Created by 86136 on 2025/4/22.
//

#ifndef CHAT_SEVER_HTTPSERVER_H
#define CHAT_SEVER_HTTPSERVER_H

// httpserver.h
#include <muduo/net/TcpServer.h>

class HttpServer {
public:
    HttpServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr);
    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);
    void sendResponse(const muduo::net::TcpConnectionPtr& conn, const std::string& content);

    muduo::net::TcpServer server_;
};
#endif //CHAT_SEVER_HTTPSERVER_H
