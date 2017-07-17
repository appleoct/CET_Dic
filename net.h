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
#include<cstring>
#include<string>
#include<iostream>
#include<hiredis.h>
const char *DB_HOST = "127.0.0.1";
const short DB_PORT = 6379;
const int LEN = 5000;
const int MAXPOLLSIZE = 10000;
const int MAXSIZE = 1000;
const int MAXLINE = 100;

class Server{
    public:
    Server(short port):_port(port) {}
    ~Server(){close(_socket_fd);redisFree(_context);}
    int init_db();
    int Listen();
    bool set_non_blocking(int fd);
    int  start_main_thread();
    int handle(int conn_fd);
    //int no_copy_echo(int conn_fd);

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
int Server::init_db()
{
    static struct timeval _timeout = {1, 500000};
    _context = redisConnectWithTimeout(DB_HOST, DB_PORT, _timeout);
    if(_context == NULL || _context->err){
        if(_context){
            printf("Redis Connection error : %s\n",_context->errstr);
            redisFree(_context);
        }else{
            printf("Redis Connection error : can't allocate redis context\n");
        }
        return -1;
    }
}
int Server::start_main_thread()
{
    _kdpfd = epoll_create(MAXPOLLSIZE);

    _ev.events = EPOLLIN | EPOLLET;
    _ev.data.fd = _socket_fd;
    if(epoll_ctl(_kdpfd, EPOLL_CTL_ADD, _socket_fd, &_ev) < 0){
        perror("epoll_ctl");
        return -1;
    }
    int curfds = 1;
    int acceptCount = 0;
    while(1){
        _nfds = epoll_wait(_kdpfd, _events, curfds, -1);
        if(_nfds == -1){
            perror("epoll_wait");
            continue;
        }
        for(int n = 0; n < _nfds; n++){
            if(_events[n].data.fd == _socket_fd){
                _conn_fd = accept(_socket_fd, (struct sockaddr *)&_client_addr, &sock_len);
                if(_conn_fd < 0){
                    perror("accept error");
                    continue;
                }
                acceptCount++;

                if(curfds >= MAXPOLLSIZE){
                    close(_conn_fd);
                    continue;
                }
                if(this->set_non_blocking(_conn_fd) < 0){
                    perror("set_non_blocking error");
                }

                _ev.events = EPOLLIN | EPOLLET;
                _ev.data.fd = _conn_fd;

                if(epoll_ctl(_kdpfd, EPOLL_CTL_ADD, _conn_fd, &_ev) < 0){
                    return -1;
                }

                curfds++;
                continue;
            }
            if(this->handle(_events[n].data.fd) < 0){
                epoll_ctl(_kdpfd, EPOLL_CTL_DEL, _events[n].data.fd, &_ev);
                curfds--;
            }
        }
    }
}
/*
int Server::no_copy_echo(int conn_fd)
{
    int pipefd[2];
    pipe(pipefd);
    while(1){

    splice(conn_fd, NULL, pipefd[1],NULL, 32768,SPLICE_F_MORE|SPLICE_F_MOVE);
    splice(pipefd[0], NULL, conn_fd,NULL, 32768,SPLICE_F_MORE|SPLICE_F_MOVE);
    }
    close(conn_fd);

}
*/
int Server::handle(int conn_fd)
{
    int nread;
    char buf[MAXLINE];

    nread = read(conn_fd, buf, MAXLINE);
    if(nread == 0){
        printf("client close connection\n");
        close(conn_fd);
        return -1;
    }
    if(nread < 0){
        perror("read error");
        close(conn_fd);
        return -1;
    }
    char cmd[MAXSIZE];
    sprintf(cmd,"%s %s","GET",buf);
    _reply = (redisReply *)redisCommand(_context, cmd);
    printf("get %s:%s\n",buf,_reply->str);
    //strcpy(buf, _reply->str);
    memcpy(buf,_reply->str,strlen(_reply->str));
    buf[strlen(_reply->str)] = '\0';
    freeReplyObject(_reply);
    /*
     * To do :
     * query from Redis
     * then pack with json
     */

    write(conn_fd, buf, nread);
    return 0;
}
int Server::Listen()
{
    int optrval = 1;
    sock_len = sizeof(struct sockaddr_in);

    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optrval,sizeof(int));
    
    memset(&_server_addr, 0, sizeof(struct sockaddr_in));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(_port);
    _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    this->set_non_blocking(_socket_fd);
   
    bind(_socket_fd, (struct sockaddr *)&_server_addr, sizeof(struct sockaddr_in));
    listen(_socket_fd, LEN);
    return 1;
}
bool Server::set_non_blocking(int fd)
{
    if(fcntl(fd, F_SETFL, fcntl(_socket_fd, F_GETFD, 0)|O_NONBLOCK) == -1){
        return false;
    }else
        return true;
}
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
        puts(read_buffer);
    }
}
#endif
