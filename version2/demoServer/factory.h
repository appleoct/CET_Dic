/*************************************************************************
	> File Name: factory.h
	> Author: Jack Kang
	> Mail: xiyoulinux.kangyijie@gmail.com
	> Created Time: 2017年08月02日 星期三 11时15分19秒
 ************************************************************************/

#ifndef _FACTORY_H
#define _FACTORY_H
#include <map>
#include <functional>
#include "./Job_base.h"
class Factory{
	public:
	template <typename F>
		struct Register{
		Register(const std::string& key)
		{
			Factory::get_instance().m_map.emplace(key, []{return new F();});
		}
            /*
		template <typename... Args>
		Register(const std::string& key, Args... args)
		{
			Factory::get_instance().m_map.emplace(key, [&]{return new F(args...);});
		}
        */
	};

	static Job* produce(const std::string& key);

	private:
	Factory() = default;
	Factory(const Factory&) = delete;
	Factory(Factory&&) = delete;

	static Factory &get_instance();
	std::map<std::string, std::function<Job* (void)>> m_map;

};

#define REGISTER_ACTION_NAME(T) msg_name_##T##_
#define REGISTER_ACTION(T, key, ...) \
            static Factory::Register<T> REGISTER_ACTION_NAME(T)(key, ##__VA_ARGS__)
#endif
