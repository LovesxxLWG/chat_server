//
// Created by 86136 on 2025/3/31.
//

#ifndef CHAT_SEVER_GROUPUSER_H
#define CHAT_SEVER_GROUPUSER_H

#include "user.h"
class GroupUser : public User {
public:
     void setRole(string role) { this->role = role;}
     string getRole() { return this->role;}
private:
     string role;

};

#endif //CHAT_SEVER_GROUPUSER_H
