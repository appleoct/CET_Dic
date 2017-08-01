/*
 * > File Name: net.h
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年06月08日 星期四 12时20分27秒
 */

#ifndef _NET_H
#define _NET_H
#include<cstdio>
#include<stdlib.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/resource.h>
#include<hiredis.h>
#include<string>
#include<string.h>
#include"./common.h"

class Server{
    public:
    Server(short port):_port(port) {}
    ~Server(){close(_socket_fd);redisFree(_context);}
    int init_db();
    int Listen();
    bool set_non_blocking(int fd);
    int  start_main_thread();
    int handle(int conn_fd);
    static int get_msg(int fd, char *buffer,std::string &job_type);
    static int send_msg(int fd,char *buffer);

    private:
    socklen_t sock_len;
    int _socket_fd,_conn_fd;
    short _port;
    struct epoll_event _ev;
    struct epoll_event _events[MAXPOLLSIZE];
    struct rlimit _rt;
    struct sockaddr_in _server_addr,_client_addr;
    int _kdpfd;
    int _nfds;

    redisContext *_context;
    redisReply   *_reply;
};


#endif
