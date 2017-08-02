/*************************************************************************
	> File Name: Job_base.h
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年08月02日 星期三 11时52分41秒
 ************************************************************************/

#ifndef _JOB_BASE_H
#define _JOB_BASE_H
#include <iostream>
#include <map>
#include <functional>
#include <cstdio>
class Job{
	public:
	Job(){std::cout << "Action" << std::endl;}
	virtual int dojob(char *buffer) = 0;
};
#endif
