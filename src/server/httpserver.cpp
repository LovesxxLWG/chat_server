//
// Created by 86136 on 2025/4/22.
//
// httpserver.cpp
#include "httpserver.h"
#include <fstream>

HttpServer::HttpServer(EventLoop* loop, const InetAddress& listenAddr)
        : server_(loop, listenAddr, "HttpServer") {
    server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::start() {
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    // 连接管理（可选）
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp) {
    string request(buf->retrieveAllAsString());
    // 简单路由
    if (request.find("GET / ") != string::npos) {
        std::ifstream file("../web/index.html");
        string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        sendResponse(conn, content);
    }
}

void HttpServer::sendResponse(const TcpConnectionPtr& conn, const string& content) {
    string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: " + std::to_string(content.size()) + "\r\n"
                                                                            "\r\n" + content;
    conn->send(response);
}