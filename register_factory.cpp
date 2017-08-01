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

#define REGISTER_ACTION_NAME(T) msg_name_##T##_
#define REGISTER_ACTION(T, key, ...) \
		static Factory::Register<T> REGISTER_ACTION_NAME(T)(key,##__VA_ARGS__)

class Action{
	public:
	Action(){std::cout << "Action" << std::endl;}
	virtual void doAction() {std::cout << "doAction" << std::endl;}
};

class login_action: public Action{
	public:
	login_action() {std::cout << "login_action" << std::endl;}
	void doAction(){std::cout << "do login_action" << std::endl;};

};

class Factory{
	public:
	template <typename F>
		struct Register{
		Register(const std::string& key)
		{
			Factory::get().m_map.emplace(key, []{return new F();});
		}
		template <typename... Args>
		Register(const std::string& key, Args... args)
		{
			Factory::get().m_map.emplace(key, [&]{return new F(args...);});
		}
	};

	static Action* produce(const std::string& key)
	{
		auto map = Factory::get().m_map;
		if(map.find(key) == map.end()){
			throw std::invalid_argument("error");
		}
		return map[key]();
	}

	private:
	Factory() = default;
	Factory(const Factory&) = delete;
	Factory(Factory&&) = delete;

	static Factory &get()
	{
		static Factory instance;
		return instance;
	}
	std::map<std::string, std::function<Action* (void)>> m_map;

};
//#define REGISTER_ACTION_NAME(T) msg_name_##T##_
//#define REGISTER_ACTION(T, keym ...) \
		static Factory::Register<T> REGISTER_ACTION_NAME(T)(key, ##__VA_ARGS__)
int main(void)
{
	REGISTER_ACTION(login_action, "login_action");
	
	Action *login = Factory::produce("login_action");
	login->doAction();
	delete(login);
	
}
