//
// Created by 86136 on 2025/3/30.
//

#ifndef CHAT_SEVER_OFFLINEMESSAGEMODEL_H
#define CHAT_SEVER_OFFLINEMESSAGEMODEL_H

#include "db/db.h"
#include "user.h"
#include "nlohmann/json.hpp"
#include <vector>

using json = nlohmann::json;

class OfflineMsgModel {
public:
    // 存储用户的离线消息
    void insert(int userid, string msg);
    // 查询用户的离线消息
    vector<string> query(int userid);
    // 删除用户的离线消息
    void remove(int userid);
};










#endif //CHAT_SEVER_OFFLINEMESSAGEMODEL_H
