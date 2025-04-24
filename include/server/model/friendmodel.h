//
// Created by 86136 on 2025/3/30.
//

#ifndef CHAT_SEVER_FRIENDMODEL_H
#define CHAT_SEVER_FRIENDMODEL_H
#include "../db/db.h"
#include "user.h"
#include <vector>
#include <string>

using namespace std;

class FriendModel {
public:
    //添加好友关系
    void insert(int userid, int friendid);
    //查询好友列表
    vector<User> query(int userid);
    //删除好友
    void remove(int userid, int friendid);
};

#endif //CHAT_SEVER_FRIENDMODEL_H
