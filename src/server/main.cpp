//
// Created by 86136 on 2025/3/23.
//
#include "chatserver.h"
#include "chatservice.h"
#include <iostream>
#include <signal.h>
using namespace std;

//处理服务器ctrl+c 信号，退出之前，重置用户状态信息
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
   /* if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 8000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    */
    char *ip = const_cast<char*>("0.0.0.0");
    uint16_t port = 8000;

    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");



    server.start();

    loop.loop();

    return 0;
}