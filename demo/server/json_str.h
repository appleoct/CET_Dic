/*************************************************************************
	> File Name: str2json.h
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年07月18日 星期二 15时53分09秒
 ************************************************************************/

#ifndef _STR2JSON_H
#define _STR2JSON_H
#include<json/json.h>
void str2json(char *buf,char *obj)
{
    char cmd[200];
    sprintf(cmd,"{\"%s\":\"%s\",\"%s\":\"%s\"}","en",buf,"zh",obj);
    strcpy(buf,cmd);
}
void json2str(std::string &buf)
{
    Json::Reader reader;
    Json::Value value;
    if(reader.parse(buf,value))
    std::cout <<value["en"].asString() << "汉译为" << value["zh"].asString() << std::endl;
}

#endif
