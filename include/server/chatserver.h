//
// Created by 86136 on 2025/3/23.
//

#ifndef CHAT_SEVER_CHATSERVER_H
#define CHAT_SEVER_CHATSERVER_H


#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <string>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>


using namespace std;
using namespace muduo;
using namespace muduo::net;

//聊天服务器的主类
class ChatServer
{
public:
    //初始化聊天服务器对象
    ChatServer(EventLoop* loop,
               const InetAddress& listenAddr,
               const string& nameArg);
    // 启动服务
    void start();

private:
    // 有客户端连接的时候，会执行回调
    void onConnection(const TcpConnectionPtr&);
    // 有客户端发送消息的时候，会执行回调
    void onMessage(const TcpConnectionPtr&,
                   Buffer*,
                   Timestamp);

    TcpServer server_;// 组合的muduo库，实现tcp服务器功能的类对象
    EventLoop* loop_; // 指向事件循环对象的指针

};

#endif //CHAT_SEVER_CHATSERVER_H