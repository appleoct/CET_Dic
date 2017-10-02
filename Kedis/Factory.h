//
// Created by kang on 17-10-2.
//

#ifndef KEDIS_FACTORY_H
#define KEDIS_FACTORY_H
#include <map>
#include <functional>
#include "BaseJob.h"

class Factory {
public:
    template <typename F>
    struct Register{
        explicit Register(const std::string& key)
        {
            Factory::getInstance().m_map.emplace(key, []{return new F();});
        }
        /*
    template <typename... Args>
    Register(const std::string& key, Args... args)
    {
        Factory::get_instance().m_map.emplace(key, [&]{return new F(args...);});
    }
    */
    };

    static BaseJob* produce(const std::string& key);

private:
    Factory() = default;
    Factory(const Factory&) = delete;
    Factory(Factory&&) = delete;

    static Factory &getInstance();
    std::map<std::string, std::function<BaseJob* (void)>> m_map;

};

#define REGISTER_ACTION_NAME(T) msg_name_##T##_
#define REGISTER_ACTION(T, key, ...) static Factory::Register<T> REGISTER_ACTION_NAME(T)(key, ##__VA_ARGS__)
//#define REGISTER_ACTION(T, key, ...) \
//            static Factory::Register<T> T(key, __VA_ARGS__)
#endif //KEDIS_FACTORY_H
