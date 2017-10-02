//
// Created by kang on 17-10-2.
//

#include "JobTest.h"
int JobTest::doJob(char *buffer)
{
    /*
    Json::Reader json_reader;
    Json::Value   json_value;
    if(json_reader.parse(std::string(buffer), json_value))
        std::cout << "查询的单词为" << json_value["en"] << std::endl;
    */
    std::cout << "data is " <<buffer << std::endl;
}