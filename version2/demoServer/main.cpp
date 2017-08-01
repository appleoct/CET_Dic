/*************************************************************************
	> File Name: server.cpp
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年07月15日 星期六 15时53分14秒
 ************************************************************************/

#include "./common.h"

#include "./server.h"
const short PORT = 5473;
int main(void)
{
    Server server(PORT);
    server.init_db();
    server.Listen();
    server.start_main_thread();
}
