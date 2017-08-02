/*************************************************************************
	> File Name: protocol.h
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年08月01日 星期二 16时31分01秒
 ************************************************************************/

#ifndef _PROTOCOL_H
#define _PROTOCOL_H
const int MAXBODYSIZE = 1000;
struct Header{
    unsigned short packet_size;
    unsigned short JobTypeCode;
};

struct Packet{
    struct Header head;
    char body[MAXBODYSIZE];
};

enum JobType{
    TEST1 = 0,
    TEST2,
    TEST3,

};
const char *JobTypeFunc[3] = {"Test_Job","test2","test3"};
#endif
