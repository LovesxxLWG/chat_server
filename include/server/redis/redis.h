//
// Created by 86136 on 2025/4/2.
//

#ifndef CHAT_SEVER_REDIS_H
#define CHAT_SEVER_REDIS_H
#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
class Redis
{
public:
    Redis();
    ~Redis();
    //连接redis服务器
    bool connect();
    //向redis通道发布消息
    bool publish(int channel,std::string message);
    //订阅消息
    bool subscribe(int channel);
    //取消订阅
    bool unsubscribe(int channel);
    //在独立线程中接收订阅通道中的消息
    void observer_channel_message();
    //初始化向业务层上报通道消息的回调对象
    void init_notify_handler(std::function<void(int,std::string)> fn);
private:
    //redis上下文对象, 负责publish消息
    redisContext* _publish_context;
    //redis上下文对象, 负责subscribe消息
    redisContext* _subscribe_context;
    //回调函数, 用于上报通道上消息的回调函数
    std::function<void(int,std::string)> _notify_message_handler;
};
#endif //CHAT_SEVER_REDIS_H
