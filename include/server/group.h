//
// Created by 86136 on 2025/3/31.
//

#ifndef CHAT_SEVER_GROUP_H
#define CHAT_SEVER_GROUP_H
#include "groupuser.h"
#include <string>
#include <vector>
using namespace std;

// Group表的ORM类
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setDesc(string desc) { this->desc = desc; }

    int getId() { return this->id; }
    string getName() { return this->name; }
    string getDesc() { return this->desc; }
    vector<GroupUser> &getUsers() { return this->users; }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};

#endif //CHAT_SEVER_GROUP_H
