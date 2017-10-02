//
// Created by kang on 17-10-2.
//

#include "Factory.h"
#include "BaseJob.h"

Factory& Factory::getInstance()
{
    static Factory instance;
    return instance;
}
BaseJob* Factory::produce(const std::string& key)
{
    auto map = Factory::getInstance().m_map;
    if(map.find(key) == map.end()){
        throw std::invalid_argument("error");
    }
    return map[key]();
}