//
// Created by 86136 on 2025/3/23.
//

#ifndef CHAT_SEVER_CHATSERVICE_H
#define CHAT_SEVER_CHATSERVICE_H
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <memory>
#include <redis/redis.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include "model/friendmodel.h"
#include "model/usermodel.h"
#include "model/offlinemessagemodel.h"
#include "model/groupmodel.h"

using json = nlohmann::json;
using MsgHandler = std :: function<void (const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp)>;
// 聊天服务器类
class ChatService {
public:
    // 获取单例对象的接口函数
    static ChatService* instance(){
        static ChatService service;
        return &service;
    }
    //处理登录业务
    void login(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time);
    //处理登出业务
    void logout(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time);
    //处理注册业务
    void reg(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time);
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    //处理客户端的连接关闭事件回调
    void clientCloseException(const muduo :: net :: TcpConnectionPtr &conn);
    //聊天业务
    void oneChat(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time);
    //添加好友业务
    void addFriend(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time);
    //创建群组业务
    void createGroup(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time);
    //加入群组业务
    void addGroup(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time);
    //群组聊天业务
    void groupChat(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time);
    //服务器端异常退出回调
    void reset();
    //redis消息队列回调
    void handleRedisSubscribeMessage(int userid, std :: string msg);
private:
    ChatService();
    ChatService(const ChatService&) = delete;
    ChatService& operator=(const ChatService&) = delete;

    // 存储消息id和其对应的业务处理方法
    std :: unordered_map<int, MsgHandler> _msgHandlerMap;
    // 存储在线用户的通信连接
    std :: unordered_map<int, muduo :: net :: TcpConnectionPtr> _userConnMap;
    //定义互斥锁
    std :: mutex _connMutex;

    //数据库user表操作对象
    UserModel _userModel;
    //数据库offlinemessage表操作对象
    OfflineMsgModel _offlineMsgModel;
    //数据库friend表操作对象
    FriendModel _friendModel;
    //数据库group表操作对象
    GroupModel _groupModel;
    //redis操作对象
    Redis _redis;
};

#endif //CHAT_SEVER_CHATSERVICE_H
