//
// Created by 86136 on 2025/3/24.
//

#ifndef CHAT_SEVER_PUBLIC_H
#define CHAT_SEVER_PUBLIC_H


//server和client的公共文件
enum MsgType {
    LOGIN_MSG = 1,//登录消息
    LOGIN_MSG_ACK,//登录响应消息
    LOGOUT_MSG,//登出消息
    REG_MSG,//注册消息
    REG_MSG_ACK,//注册响应消息
    ONE_CHAT_MSG,//私聊消息
    ADD_FRIEND_MSG,//添加好友消息

    CREATE_GROUP_MSG,//创建群组
    ADD_GROUP_MSG,//加入群组
    GROUP_CHAT_MSG,//群聊消息


    CLOSE_FRIEND_MSG,
    CLOSE_GROUP_MSG,
};
#endif //CHAT_SEVER_PUBLIC_H
