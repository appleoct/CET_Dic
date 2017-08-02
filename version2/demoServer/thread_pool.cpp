/*
 * > File Name: thread_poll.cpp
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年07月19日 星期三 14时47分45秒
 */
 #include "thread_pool.h"
 #include "./factory.h"
 #include "server.h"


template <typename T>
void SyncQueue<T>::Put(const T &x)
    { 
        Add(x); 
    }
template <typename T>
void SyncQueue<T>::Put(T &&x) 
    {
        Add(std::forward<T>(x));
    }

    //从任务队列中获取任务
template <typename T>
void SyncQueue<T>::Take(std::list<T>& list)
    {
        std::unique_lock<std::mutex> locker(m_mutex); //对于队列的操作需要加锁保护，注意这里使用了unique_lock去加锁
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty(); }); // lambda表达式捕获了this指针则可以直接调用private 方法
        //如果队列不为空则往下执行，否则阻塞

        //如果m_needStop标志为true 则直接结束
        if(m_needStop)
            return;

        list = std::move(m_queue);
        m_notFull.notify_one(); //唤醒一个等待Put的线程
    }
template <typename T>
void SyncQueue<T>::Take(T& x)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty();});

        if(m_needStop)
            return ;

        x = m_queue.front();
        m_queue.pop_front();

        m_notFull.notify_one();

    }
    
    //停止所有任务
template <typename T>
void SyncQueue<T>::Stop()
    {
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            m_needStop = true;
        }
        m_notFull.notify_all(); //唤醒正在等待Take和Put的线程，然后所有线程return
        m_notEmpty.notify_all();
    }

template <typename T>
bool SyncQueue<T>::Empty()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.empty();
    }
template <typename T>
bool SyncQueue<T>::Full()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size() == m_maxsize;
    }
template <typename T>
size_t SyncQueue<T>::Size()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size();
    }

template <typename T>
int SyncQueue<T>::Count()
    {
        return m_queue.size();
    }

template <typename T>
bool SyncQueue<T>::NotFull() const
    {
        bool full = m_queue.size() >= m_maxsize;
        if(full)
        std::cout << "buffer is full ...please wait" << std::endl;
        else 
            return !full;
    }

template <typename T>
bool SyncQueue<T>::NotEmpty() const
    {
        bool empty = m_queue.empty();
        if(empty){
            std::cout << "buffer is empty... please wait. Thread ID:" << std::this_thread::get_id() << std::endl;
        }
        return !empty;
    }

template <class T>
template <class F>
void SyncQueue<T>::Add(F &&x)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notFull.wait(locker, [this]{return m_needStop || NotFull();});
        if(m_needStop)
            return;
        m_queue.push_back(std::forward<F>(x));
        m_notEmpty.notify_one();
    }

// ThreadPool
//
    void ThreadPool::Stop()
    {
        std::call_once(m_flag, [this]{StopThreadGroup();});
    }
    void ThreadPool::AddTask(Task&& task)
    {
        M_queue.Put(std::forward<Task>(task));
    }
    void ThreadPool::AddTask(const Task& task)
    {
        M_queue.Put(task);
    }
    void ThreadPool::Start(int numThreads)
    {
        int real_numThreads;
        if(!numThreads)
            real_numThreads = std::thread::hardware_concurrency();
        else 
            real_numThreads  = numThreads;
        m_running = true;
        for(int i = 0;i < real_numThreads; ++i)
            m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));

    }

    ThreadPool& ThreadPool::get_instance()
    {
        static ThreadPool instance;
        return instance;
    }
    void ThreadPool::RunInThread()
    {
        while(m_running){
            Task my_task;
            M_queue.Take(my_task);
            if(!m_running)
                return;
            else
                doJob(my_task);
        }
    }
    void ThreadPool::doJob(int fd)
    {
        char buffer[MAXSIZE];
        std::string job_type;
        Server::get_msg(fd,buffer,job_type); 
        std::cout << buffer << std::endl;

        
        Job *myjob = Factory::produce(job_type);
        myjob->dojob(buffer);
        
        std::cout << buffer << std::endl;
        Server::send_msg(fd,buffer);
        delete myjob;
    }

void ThreadPool::StopThreadGroup()
    {
        M_queue.Stop();
        m_running = false;

        for(auto thread : m_threadgroup)
            if(thread)
                thread->join();
        m_threadgroup.clear();
    }
