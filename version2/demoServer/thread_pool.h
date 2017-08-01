/*
 * > File Name: thread_poll.cpp
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年07月19日 星期三 14时47分45秒
 */
#include<string>
#include<iostream>
#include<mutex>
#include<thread>
#include<list>
#include<atomic>
#include<condition_variable>

#include "./common.h"
// 同步队列---我们采用模板类来实现
template <typename T>
class SyncQueue{
public:
    //构造函数：这里实现的较为简单，主要是记录了队列允许的最大任务数，同时将终止队列的flag--m_needStop设置为false
    SyncQueue(int maxsize) : m_maxsize(maxsize), m_needStop(false) {}
    //向任务队列中添加任务
    void Put(const T &x);
    void Put(T &&x) ;
    //从任务队列中获取任务
    void Take(std::list<T>& list);
    void Take(T& x);
    void Stop();
    bool Empty();
    bool Full();
    size_t Size();
    int Count();

private:
    bool NotFull() const;
    bool NotEmpty() const;
    template <typename F> void Add(F &&x);

private:
    std::list<T> m_queue;               //使用std::list来保存任务
    std::mutex   m_mutex;               //C++11的互斥锁 用于线程同步
    std::condition_variable m_notEmpty; //C++11的条件变量 
    std::condition_variable m_notFull;  //
    int m_maxsize;                      //队列中允许保存的最大任务数
    bool m_needStop;                    //用于终止队列的标志，默认为false
};

const int MaxTaskCount = 100;
class ThreadPool{
    public:
    using Task = int ; // TO DO:   task type !!
    ~ThreadPool(void) { Stop();}

    void Stop();
    void AddTask(Task&& task);
    void AddTask(const Task& task);
    void Start(int numThreads);

    static ThreadPool &get_instance();

    private:

    ThreadPool(int numThreads = std::thread::hardware_concurrency()): M_queue(MaxTaskCount) { Start(numThreads); }
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;

    void RunInThread();
    void doJob(int fd);

    void StopThreadGroup();

    std::list<std::shared_ptr<std::thread>> m_threadgroup;
    SyncQueue<Task>         M_queue;
    std::atomic_bool        m_running;
    std::once_flag          m_flag;
};

