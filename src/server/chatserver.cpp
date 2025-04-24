//
// Created by 86136 on 2025/3/23.
//
#include "chatserver.h"
#include "chatservice.h"
#include <iostream>
#include <functional>
#include <nlohmann/json.hpp>
#include <muduo/net/TcpServer.h>

using namespace std;
using namespace placeholders;
using json = nlohmann :: json;

ChatServer::ChatServer( EventLoop *loop,
                        const InetAddress &listenAddr,
                        const string &nameArg)
                    :server_(loop, listenAddr, nameArg),loop_(loop)
{
    //设置连接回调函数
    server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    //设置消息回调函数
    server_.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    //设置线程数量
    server_.setThreadNum(4);

}
// 启动服务器程序
void ChatServer::start() {
    server_.start();
}

// 连接回调函数
void ChatServer :: onConnection(const TcpConnectionPtr &conn) {
    //客户端连接断开
    if(!conn->connected()){
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }

}

// 消息回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time) {
    string msg = buffer->retrieveAllAsString();
    json js = json::parse(msg);
    //目的：将网络模块和业务模块进行解耦
    //通过js["msgid"]获取消息id，然后找到对应的业务处理
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());

    msgHandler(conn, js, time);
}