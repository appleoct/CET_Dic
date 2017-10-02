#include <iostream>
#include <string>
#include "Network.h"
#include "TheadPool.h"
const short PORT = 5473;
int main(int argc, char *argv[]) {
    int thread_nums;
    if(!argv[1])
        thread_nums = 0;
    else
        thread_nums = atoi(argv[1]);
    ThreadPool::getInstance().startThreadPool(thread_nums);
    Network server(PORT);
    server.initDb();
    server.Listen();
    server.startMainLoop();
    return 0;
}