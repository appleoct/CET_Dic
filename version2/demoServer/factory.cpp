/*************************************************************************
	> File Name: Factory.cpp
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年08月02日 星期三 11时06分53秒
 ************************************************************************/
#include "./factory.h"
Factory& Factory::get_instance()
{
		static Factory instance;
		return instance;
}
Job* Factory::produce(const std::string& key)
{
		auto map = Factory::get_instance().m_map;
		if(map.find(key) == map.end()){
			throw std::invalid_argument("error");
		}
		return map[key]();
}
