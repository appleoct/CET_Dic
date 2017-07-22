/*
 * > File Name: thread_poll.cpp
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年07月19日 星期三 14时47分45秒
 */

#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <functional>
template <typename T>
class SyncQueue{
public:

    SyncQueue(int maxsize) : m_maxsize(maxsize), m_needStop(false) {}

    void Put(const T &x)
    { 
        Add(x); 
    }

    void Put(T &&x) 
    {
        Add(std::forward<T>(x));
    }
    void Take(std::list<T>& list)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty(); });

        if(m_needStop)
            return;
        list = std::move(m_queue);
        m_notEmpty.notify_one();
    }
    void Take(T& x)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty();});

        if(m_needStop)
            return ;
        x = m_queue.front();
        m_queue.pop_front();
        m_notFull.notify_one();

    }

    void Stop()
    {
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            m_needStop = true;
        }
        m_notFull.notify_all();
        m_notEmpty.notify_all();
    }
    bool Empty()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.empty();
    }
    bool Full()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size() == m_maxsize;
    }
    size_t Size()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size();
    }
    int Count()
    {
        return m_queue.size();
    }

private:
    bool NotFull() const
    {
        bool full = m_queue.size() >= m_maxsize;
        if(full)
        std::cout << "buffer is full ...please wait" << std::endl;
        else 
            return !full;
    }
    bool NotEmpty() const
    {
        bool empty = m_queue.empty();
        if(empty){
            std::cout << "buffer is empty... please wait. Thread ID:" << std::this_thread::get_id() << std::endl;
        }
        return !empty;
    }

    template <typename F>
    void Add(F &&x)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notFull.wait(locker, [this]{return m_needStop || NotFull();});
        if(m_needStop)
            return;
        m_queue.push_back(std::forward<F>(x));
        m_notEmpty.notify_one();
    }

private:
    std::list<T> m_queue;
    std::mutex   m_mutex;
    std::condition_variable m_notEmpty;
    std::condition_variable m_notFull;
    int m_maxsize;
    bool m_needStop;
};

const int MaxTaskCount = 100;
class ThreadPool{
    public:
    using Task = std::function<void()>;
    ThreadPool(int numThreads = std::thread::hardware_concurrency()): M_queue(MaxTaskCount) { Start(numThreads); }
    ~ThreadPool(void) { Stop();}
    void Stop()
    {
        std::call_once(m_flag, [this]{StopThreadGroup();});
    }
    void AddTask(Task&& task)
    {
        M_queue.Put(std::forward<Task>(task));
    }
    void AddTask(const Task& task)
    {
        M_queue.Put(task);
    }
    void Start(int numThreads)
    {
        m_running = true;
        for(int i = 0;i < numThreads; ++i)
            m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));

    }
    private:
    void RunInThread()
    {
        while(m_running){
            std::list<Task> List;
            M_queue.Take(List);
            for(auto& task : List){
                if(!m_running)
                    return;
                task();
            }
        }
    }
    void StopThreadGroup()
    {
        M_queue.Stop();
        m_running = false;

        for(auto thread : m_threadgroup)
            if(thread)
                thread->join();
        m_threadgroup.clear();
    }

    std::list<std::shared_ptr<std::thread>> m_threadgroup;
    SyncQueue<Task>         M_queue;
    std::atomic_bool        m_running;
    std::once_flag          m_flag;
};
void testThePool()
{
    ThreadPool pool;
    pool.Start(2);
    std::cout << "start" << std::endl;
    std::thread thd1([&pool]{
        for(int i = 0; i < 10; ++i){
            auto thdId =std::this_thread::get_id();
            pool.AddTask([thdId]{std::cout << "同步层线程1的线程ID: " << thdId << std::endl;});
        }
    });
    std::thread thd2([&pool]{
        for(int i = 0; i < 10; ++i){
            auto thdId =std::this_thread::get_id();
            pool.AddTask([thdId]{std::cout << "同步层线程2的线程ID:" << thdId << std::endl;});
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    getchar();
    pool.Stop();
    thd1.join();
    thd2.join();

}

int main(void)
{
    testThePool();
    
}
