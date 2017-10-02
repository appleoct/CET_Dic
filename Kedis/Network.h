//
// Created by kang on 17-10-1.
//

#ifndef KEDIS_NETWORK_H
#define KEDIS_NETWORK_H
#include <cstdio>
#include <cstdlib>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/resource.h>
#include <hiredis.h>
#include <string>
#include <cstring>
#include <iostream>
#include <csignal>
static const unsigned int MAX_POLL_SIZE = 10000;
static const char *DB_HOST  = "123.206.89.123";
static const short DB_PORT = 6379;
static const int LISTEN_LEN  = 1000;
class Network{
public:
    Network(short port):_port(port) {}
    ~Network(){
        close(_socket_fd);
        redisFree(_context);}

    int initDb();
    int Listen();
    bool setNonBlocking(int fd);
    int  startMainLoop();
    static int recvMessage(int fd, char *buffer,std::string &job_type);
    static int sendMessage(int fd,char *buffer);
    int delFd(int fd);

private:
    socklen_t sock_len;
    int _socket_fd,_conn_fd;
    short _port;
    struct epoll_event _ev;
    struct epoll_event _events[MAX_POLL_SIZE];
    struct rlimit _rt;
    struct sockaddr_in _server_addr,_client_addr;
    int _kdpfd;
    int _nfds;

    redisContext *_context;
    redisReply   *_reply;
};


#endif //KEDIS_NETWORK_H
