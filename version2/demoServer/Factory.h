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

	static Job* produce(const std::string& key)
	{
		auto map = Factory::get_instance().m_map;
		if(map.find(key) == map.end()){
			throw std::invalid_argument("error");
		}
		return map[key]();
	}

	private:
	Factory() = default;
	Factory(const Factory&) = delete;
	Factory(Factory&&) = delete;

	static Factory &get_instance()
	{
		static Factory instance;
		return instance;
	}
	std::map<std::string, std::function<Job* (void)>> m_map;

};

#define REGISTER_Job_NAME(T) msg_name_##T##_
#define REGISTER_Job(T, key, ...) \
		static Factory::Register<T> REGISTER_Job_NAME(T)(key,##__VA_ARGS__)
