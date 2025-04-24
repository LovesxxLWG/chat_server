//
// Created by 86136 on 2025/3/31.
//

#ifndef CHAT_SEVER_GROUPMODEL_H
#define CHAT_SEVER_GROUPMODEL_H
#include "group.h"
#include <string>
#include <vector>
using namespace std;
class GroupModel {
public:
    bool createGroup(Group &group);//创建群
    void addGroup(int userid, int groupid, string role);//加入群
    vector<Group> queryGroups(int userid);//查询群信息
    vector<int> queryGroupUsers(int userid, int groupid);//查询群用户id, 用于群聊
};
#endif //CHAT_SEVER_GROUPMODEL_H
