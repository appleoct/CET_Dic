//
// Created by kang on 17-10-1.
//

#ifndef KEDIS_THEADPOOL_H
#define KEDIS_THEADPOOL_H
#include<string>
#include<iostream>
#include<mutex>
#include<thread>
#include<list>
#include<atomic>
#include<condition_variable>

// 同步队列---我们采用模板类来实现
template <typename T>
class SyncQueue{
public:
    //构造函数：这里实现的较为简单，主要是记录了队列允许的最大任务数，同时将终止队列的flag--m_needStop设置为false
    explicit SyncQueue(int maxsize)  : m_maxsize(maxsize), m_needStop(false) {}
    //向任务队列中添加任务
    void putTask(const T &x);
    void putTask(T &&x) ;
    //从任务队列中获取任务
    void takeTask(std::list<T>& list);
    void takeTask(T& x);
    void stopQueue();
    bool isEmpty();
    bool isFull();
    size_t getSize();
    //int getCount();

private:
    bool isNotFull() const;
    bool isNotEmpty() const;
    template <typename F> void add(F &&x);

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
    ~ThreadPool() { stopThreadPool();}
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;

    void stopThreadPool();
    void addTask(Task&& task);
    void addTask(const Task& task);
    void startThreadPool(int numThreads);

    static ThreadPool &getInstance();

private:

    ThreadPool() :M_queue(MaxTaskCount),m_running(false) {};


    void runInThread();
    void doJob(int fd);

    void stopThreadGroup();

    std::list<std::shared_ptr<std::thread>> m_threadgroup;
    SyncQueue<Task>         M_queue;
    std::atomic_bool        m_running;
    std::once_flag          m_flag;
};



#endif //KEDIS_THEADPOOL_H
