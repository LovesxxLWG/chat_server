//
// Created by 86136 on 2025/4/22.
//

#ifndef CHAT_SEVER_WEBSOCKETSERVER_H
#define CHAT_SEVER_WEBSOCKETSERVER_H
// websocketserver.h
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <unordered_map>

using namespace muduo::net;

class WebSocketServer {
public:
    WebSocketServer(EventLoop* loop, const InetAddress& listenAddr);
    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);
    bool handleHandshake(const TcpConnectionPtr& conn, const string& request);
    void handleWebSocketFrame(const TcpConnectionPtr& conn, const string& frame);

    TcpServer server_;
    std::unordered_map<TcpConnectionPtr, bool> wsConnections_; // 标记是否为WebSocket连接
};
#endif //CHAT_SEVER_WEBSOCKETSERVER_H
