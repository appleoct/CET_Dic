/*************************************************************************
	> File Name: Job_test.h
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年08月01日 星期二 16时48分35秒
 ************************************************************************/

#ifndef _JOB_TEST_H
#define _JOB_TEST_H
#include "./Job_base.hpp"
class Test_Job:public Job{
    public:
    Test_Job() { std::cout << "init Test_Job" << std::endl;}
    int dojob(char *buffer) { std::cout << "do test job" << std::endl;};
};
#endif
