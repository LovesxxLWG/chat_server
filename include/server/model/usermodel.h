//
// Created by 86136 on 2025/3/28.
//

#ifndef CHAT_SEVER_USERMODEL_H
#define CHAT_SEVER_USERMODEL_H

#include "user.h"
#include "../db/db.h"

// 用户类
class UserModel {
public:
    // 添加用户信息
    bool insert(User &user);

    // 更新用户状态信息
    bool updateState(User user);

    // 重置用户状态信息
    void resetState();

    // 根据用户号码查询用户信息
    User query(int id);
};
#endif //CHAT_SEVER_USERMODEL_H
