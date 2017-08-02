/*************************************************************************
	> File Name: server.cpp
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年08月01日 星期二 21时15分11秒
 ************************************************************************/
 #include"./server.h"
 #include"./protocol.h"
 #include"./thread_pool.h"

 #include "./factory.h"
const char *DB_HOST = "127.0.0.1";
const short DB_PORT = 6379;
const int LEN = 5000;
const int MAXSIZE = 1000;
const int MAXLINE = 100;

int Server::get_msg(int fd, char *buffer, std::string& job_type)
{
    struct Header myhead;
    if(read(fd, &myhead, 4) < 4)
        return -1;
    
    job_type = std::string(JobTypeFunc[myhead.JobTypeCode]);

    if(read(fd,buffer, myhead.packet_size) < myhead.packet_size)
        return -1;
    else
        return 0;

}
int Server::send_msg(int fd, char *buffer)
{
    int len  = strlen(buffer);
    if(write(fd, buffer, len) < len)
        return -1;
    else 
        return 0;
}

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
            ThreadPool::get_instance().AddTask(_events[n].data.fd);
        }
    }
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
