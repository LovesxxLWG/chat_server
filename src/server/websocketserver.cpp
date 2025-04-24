//
// Created by 86136 on 2025/4/22.
//
// websocketserver.cpp
#include "websocketserver.h"
#include <algorithm>
#include <cstring>
#include "chatservice.h"
#include <openssl/sha.h>
#include <base64.h>

WebSocketServer::WebSocketServer(EventLoop* loop, const InetAddress& listenAddr)
        : server_(loop, listenAddr, "WebSocketServer") {
    server_.setConnectionCallback(std::bind(&WebSocketServer::onConnection, this, _1));
    server_.setMessageCallback(std::bind(&WebSocketServer::onMessage, this, _1, _2, _3));
}

void WebSocketServer::start() {
    server_.start();
}

void WebSocketServer::onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        wsConnections_[conn] = false; // 初始状态为未握手
    } else {
        wsConnections_.erase(conn);
        ChatService::instance()->clientCloseException(conn);
    }
}

void WebSocketServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
    if (!wsConnections_[conn]) {
        string request(buf->retrieveAllAsString());
        if (handleHandshake(conn, request)) {
            wsConnections_[conn] = true;
        }
    } else {
        string frame(buf->retrieveAllAsString());
        handleWebSocketFrame(conn, frame);
    }
}

// WebSocket握手处理
bool WebSocketServer::handleHandshake(const TcpConnectionPtr& conn, const string& request) {
    // 解析Sec-WebSocket-Key并生成响应
    // 示例代码，需完善头解析逻辑
    string keyHeader = "Sec-WebSocket-Key: ";
    size_t start = request.find(keyHeader);
    if (start == string::npos) return false;
    start += keyHeader.length();
    size_t end = request.find("\r\n", start);
    string key = request.substr(start, end - start);

    // 生成accept
    string accept = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    unsigned char sha1[20];
    SHA1(reinterpret_cast<const unsigned char*>(accept.data()), accept.length(), sha1);
    string acceptKey = base64_encode(sha1, 20);

    // 构造响应
    string response = "HTTP/1.1 101 Switching Protocols\r\n"
                      "Upgrade: websocket\r\n"
                      "Connection: Upgrade\r\n"
                      "Sec-WebSocket-Accept: " + acceptKey + "\r\n\r\n";
    conn->send(response);
    return true;
}

// WebSocket帧处理
void WebSocketServer::handleWebSocketFrame(const TcpConnectionPtr& conn, const string& frame) {
    // 简单处理文本帧（需完善）
    if ((frame[0] & 0x0F) == 0x01) { // 文本帧
        size_t len = frame[1] & 0x7F;
        size_t maskOffset = 2;
        if (len == 126) {
            len = (static_cast<unsigned char>(frame[2]) << 8) | static_cast<unsigned char>(frame[3]);
            maskOffset = 4;
        } else if (len == 127) {
            // 处理64位长度（略）
        }

        string payload = frame.substr(maskOffset + 4, len);
        // 解码payload（需处理掩码）
        // 此处简化处理，实际应应用掩码
        json js = json::parse(payload);
        auto handler = ChatService::instance()->getHandler(js["msgid"].get<int>());
        // 适配发送函数
        auto sendFunc = [conn](const string& msg) {
            // 构造WebSocket帧
            string frame;
            frame.push_back(0x81); // FIN + text frame
            if (msg.size() <= 125) {
                frame.push_back(static_cast<char>(msg.size()));
            } else if (msg.size() <= 65535) {
                frame.push_back(126);
                frame.push_back(static_cast<char>((msg.size() >> 8) & 0xFF));
                frame.push_back(static_cast<char>(msg.size() & 0xFF));
            }
            frame += msg;
            conn->send(frame);
        };
        handler(sendFunc, js, time);
    }
}
