//
// Created by 86136 on 2025/3/30.
//
#include "../../../include/server/model/offlinemessagemodel.h"
#include "../../../include/server/db/db.h"
// 存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg)
{
    //组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into OfflineMessage values(%d,'%s')", userid, msg.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
// 删除离线消息
void OfflineMsgModel::remove(int userid)
{
    //组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from OfflineMessage where userid=%d", userid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    //组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from OfflineMessage where userid=%d", userid);

    vector<string> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            //查询成功
            MYSQL_ROW row;
            //把每一行内容都放到vec中
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);

        }
    }
    return vec;
}



