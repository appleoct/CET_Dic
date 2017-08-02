/*************************************************************************
	> File Name: server.cpp
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年07月15日 星期六 15时53分14秒
 ************************************************************************/

#include "./server.h"
#include "./thread_pool.h"
const short PORT = 5473;
int main(void)
{
    ThreadPool::get_instance().Start(4);
    Server server(PORT);
    server.init_db();
    server.Listen();
    server.start_main_thread();
}
