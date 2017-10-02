//
// Created by kang on 17-10-2.
//

#ifndef KEDIS_BASEJOB_H
#define KEDIS_BASEJOB_H
#include <iostream>
#include <map>
#include <functional>
#include <cstdio>

class BaseJob {
public:
    BaseJob(){std::cout << "Action" << std::endl;}
    virtual int doJob(char *buffer) = 0;
};


#endif //KEDIS_BASEJOB_H
