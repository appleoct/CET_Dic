/*
 * > File Name: client.cpp
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年07月18日 星期二 16时13分53秒
 */

#include <iostream>
#include "client.h"
int main(void)
{
    Client cli("123.206.89.123",5473);
    cli.Connect();
    cli.handle();
}
