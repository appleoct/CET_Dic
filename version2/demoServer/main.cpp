/*************************************************************************
	> File Name: server.cpp
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年07月15日 星期六 15时53分14秒
 ************************************************************************/

#include "./server.h"
#include "./thread_pool.h"
const short PORT = 5473;
int main(int argc, char **argv)
{
    int thread_nums;
    if(!argv[1])
        thread_nums = 0;
    else
        thread_nums = atoi(argv[1]);
    ThreadPool::get_instance().Start(thread_nums);
    Server server(PORT);
    server.init_db();
    server.Listen();
    server.start_main_thread();
}
