/*
 * > File Name: register_factory.cpp
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年07月20日 星期四 15时51分51秒
 */

#include <iostream>
#include <map>
#include <functional>
#include <cstdio>

class Job{
	public:
	Job(){std::cout << "Action" << std::endl;}
	virtual int dojob(char *buffer) = 0;
};
