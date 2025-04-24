//
// Created by 86136 on 2025/3/24.
//


#include "chatservice.h"
#include <vector>
#include <string>
#include <map>
#include <muduo/base/Logging.h>
#include "public.h"
#include <mutex>




// 初始化静态成员
//ChatService* ChatService::_instance = nullptr;

// 类外定义时无需重复 static 关键字
/*ChatService& ChatService::instance() {
    static ChatService _instance; // 线程安全的局部静态变量
    return _instance;
}*/
ChatService :: ChatService()
{   //
    _msgHandlerMap.insert(std::make_pair(LOGIN_MSG, std::bind(&ChatService::login, this, std :: placeholders::_1, std :: placeholders::_2, std :: placeholders::_3)));
    _msgHandlerMap.insert(std::make_pair(LOGOUT_MSG, std :: bind(&ChatService::logout, this, std :: placeholders::_1, std :: placeholders::_2, std :: placeholders::_3)));
    _msgHandlerMap.insert(std::make_pair(REG_MSG, std::bind(&ChatService::reg, this, std :: placeholders::_1, std :: placeholders::_2, std :: placeholders::_3)));
    _msgHandlerMap.insert(std::make_pair(ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, std :: placeholders::_1, std :: placeholders::_2, std :: placeholders::_3)));
    _msgHandlerMap.insert(std::make_pair(ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, std :: placeholders::_1, std :: placeholders::_2, std :: placeholders::_3)));
    //群组业务回调
    _msgHandlerMap.insert(std::make_pair(CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, std :: placeholders::_1, std :: placeholders::_2, std :: placeholders::_3)));
    _msgHandlerMap.insert(std::make_pair(ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, std :: placeholders::_1, std :: placeholders::_2, std :: placeholders::_3)));
    _msgHandlerMap.insert(std::make_pair(GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, std :: placeholders::_1, std :: placeholders::_2, std :: placeholders::_3)));

    //redis订阅回调
    if(_redis.connect()){
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, std :: placeholders::_1, std :: placeholders::_2));
    }


}
//服务器异常，重置用户状态信息
void ChatService::reset()
{
    _userModel.resetState();
}


// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid){
    //记录错误日志，msgid没有对应的事件处理程序
    auto it = _msgHandlerMap.find(msgid);
    if(it == _msgHandlerMap.end()){
        //返回空处理程序
        return [=] (const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time){
            //记录错误日志
            LOG_ERROR << "msgid:" << msgid << "can not find handler!";
        };



    }else{

        return _msgHandlerMap[msgid];//返回对应的回调函数
    }

}


void ChatService::login(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time)
{
     int id = js["id"].get<int>();
     string pwd = js["password"];
     User user = _userModel.query(id);
     if(user.getId() == id && user.getPwd() == pwd){
         if(user.getState() == "online"){// 用户已经登录，不允许重复登录
             json response;
             response["msgid"] = LOGIN_MSG_ACK;
             response["errno"] = 2;
             response["errmsg"] = "this account has logged in, please logout first!";
             conn->send(response.dump());
             return;
         }
         else {
             // 登录成功
             // 记录用户连接
             //注意线程安全
             {
                 lock_guard<mutex> lock(_connMutex);
                 _userConnMap.insert({id, conn});
             }
             //id登录成功后，向redis订阅channel(id)
             _redis.subscribe(id);

             user.setState("online");
             _userModel.updateState(user);
             json response;
             response["msgid"] = LOGIN_MSG_ACK;
             response["errno"] = 0;
             response["id"] = user.getId();
             response["name"] = user.getName();
             response["state"] = user.getState();
             //查询该用户的离线消息
             vector<string> vec = _offlineMsgModel.query(id);
             if (!vec.empty()) {
                 response["offlinemsg"]= vec;
                 //读取完之后再删除
                 _offlineMsgModel.remove(id);
             }
             //显示好友列表
             vector<User> userVec = _friendModel.query(id);
             if (!userVec.empty()) {
                 vector<string>vec2;
                 for (auto &user : userVec) {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                 }
                 response["friends"] = vec2;
             }
             conn->send(response.dump());
         }
     }
     else{
         if (user.getId() == -1) {// 用户不存在
             json response;
             response["msgid"] = LOGIN_MSG_ACK;
             response["errno"] = 1;
             response["errmsg"] = "用户不存在!";
             conn->send(response.dump());

         }
         else{// 密码错误
             json response;
             response["msgid"] = LOGIN_MSG_ACK;
             response["errno"] = 1;
             response["errmsg"] = "password is error!";
             conn->send(response.dump());

         }
     }
}
//登出
void ChatService::logout(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time)
{
     int userid = js["id"].get<int>();
     {
         lock_guard<mutex> lock(_connMutex);
         auto it = _userConnMap.find(userid);
         if (it != _userConnMap.end()) {
             _userConnMap.erase(it);
         }
     }
     // 用户注销，相当于下线，在redis中取消订阅通道
     _redis.unsubscribe(userid);
     // 更新用户的状态信息
     User user(userid,"","","offline");
     _userModel.updateState(user);
}
// 注册业务
void ChatService::reg(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time)
{
     string name = js["name"];
     string pwd = js["password"];
     User user;
     user.setName(name);
     user.setPwd(pwd);
     bool state = _userModel.insert(user);
     if(state){
         //注册成功
         json response;
         response["msgid"] = REG_MSG_ACK;
         response["errno"] = 0;
         response["id"] = user.getId();
         conn->send(response.dump());
     }else{
         //注册失败
         json response;
         response["msgid"] = REG_MSG_ACK;
         response["errno"] = 1;
         conn->send(response.dump());
     }
}

// 处理客户端异常退出 clientCloseException
void ChatService::clientCloseException(const muduo :: net :: TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        {
            for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++) {
                if (it->second == conn) {
                    user.setId(it->first);
                    _userConnMap.erase(it);//删除连接信息
                    break;
                }
            }
        }
    }
    // 通知redis
    _redis.unsubscribe(user.getId());
    // 更新用户状态信息
    if(user.getId() != -1){
        //更新用户状态信息
        user.setState("offline");
        _userModel.updateState(user);
    }


}

// 处理消息
void ChatService::oneChat(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time)
{
    int toid = js["toid"].get<int>();
    {
        // 查找toid用户是否在线
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it != _userConnMap.end()){//在线,但是在同一台服务器
            //转发消息
            it->second->send(js.dump());
            return;
        }
    }
    //查询toid是否在其他服务器上在线，通过redis转发
    User user = _userModel.query(toid);
    if(user.getState() == "online"){
        _redis.publish(toid, js.dump());
        return;
    }
   //不在线，存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

//添加好友
void ChatService::addFriend(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    _friendModel.insert(userid, friendid);
}
//创建群
void ChatService::createGroup(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];
    Group group(-1, name, desc);
    if(_groupModel.createGroup(group)) {
        //存储创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }

}
// 加入群
void ChatService :: addGroup(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

//群聊天
void ChatService::groupChat(const muduo :: net :: TcpConnectionPtr &conn, json &js, muduo :: Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex);
    for (auto id : useridVec) {
        auto it = _userConnMap.find(id);
        if(it != _userConnMap.end()){
            //转发消息
            it->second->send(js.dump());
        }
        else{
            //查询toid是否在其他服务器上在线，通过redis转发
            User user = _userModel.query(id);
            if(user.getState() == "online"){
                _redis.publish(id, js.dump());

            } else{
                //存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }

        }
    }
}
// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}