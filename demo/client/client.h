/*
 * > File Name: client.h
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年07月18日 星期二 16时18分43秒
 */

#ifndef _CLIENT_H
#define _CLIENT_H
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
#include<cstring>
#include<string>
#include<iostream>
#include"str_json.h"
const char *DB_HOST = "127.0.0.1";
const short DB_PORT = 6379;
const int LEN = 5000;
const int MAXPOLLSIZE = 10000;
const int MAXSIZE = 1000;
const int MAXLINE = 100;
class Client{
    public:
    Client(std::string ip_addr, short port): _ip_addr(ip_addr),_port(port){};
    ~Client(){close(_socket_fd);}
    int Connect();
    int handle();

    private:
    std::string _ip_addr;
    short _port;
    struct sockaddr_in _server_addr;
    int _socket_fd;
};
int Client::Connect()
{
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&_server_addr, 0, sizeof(struct sockaddr_in));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(_port);
    inet_aton(_ip_addr.c_str(),&_server_addr.sin_addr);
    
    connect(_socket_fd, (struct sockaddr *)&_server_addr, sizeof(struct sockaddr_in));
}
int Client::handle()
{
    char send_buffer[MAXSIZE];
    char read_buffer[MAXSIZE];
    int n;
    while(1){
        memset(&send_buffer, 0, sizeof(send_buffer));
        memset(&read_buffer, 0, sizeof(read_buffer));
        scanf("%s",send_buffer);
        if(strcmp("exit",send_buffer) == 0)
            break;
        if(write(_socket_fd, send_buffer, strlen(send_buffer)+1) < 0){
            continue;
        }
        n = read(_socket_fd, read_buffer, sizeof(read_buffer));
        if(n < 0)
            continue;

        std::string str(read_buffer);
        std::cout << str << std::endl;
        json2str(str);
    }
}
#endif
