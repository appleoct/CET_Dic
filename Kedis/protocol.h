//
// Created by kang on 17-10-1.
//

#ifndef KEDIS_PROTOCOL_H
#define KEDIS_PROTOCOL_H
const unsigned short MAX_BUFFER_SIZE = 2000;
const unsigned short HEADER_SIZE = 4;

struct MessageHeader{
    unsigned short messageSize;
    unsigned short typeCode;
};
struct Message{
    struct MessageHeader messageHeader;
    char buffer[MAX_BUFFER_SIZE];
};
enum messageType{
    TEST = 0,
    LOGIN = 1,
    REGISTER,
    QUERY_SINGLE,
    QUERY_MORE,
    ADD_WORD,
};
static const char *doJobProc[] = {"TEST", "LOGIN", "REGISTER", "QUERY_SINGLE", "QUERY_MORE", "ADD_WORD"};
#endif //KEDIS_PROTOCOL_H
