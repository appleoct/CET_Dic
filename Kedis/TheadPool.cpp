//
// Created by kang on 17-10-1.
//

#include "TheadPool.h"
#include "Network.h"
#include "protocol.h"
#include "Factory.h"
#include "BaseJob.h"

template <typename T>
void SyncQueue<T>::putTask(const T &x) {
    add(x);
}

template <typename T>
void SyncQueue<T>::putTask(T &&x) {
    add(std::forward<T>(x));
}

template <typename T>
void SyncQueue<T>::takeTask(std::list<T> &list) {
    std::unique_lock<std::mutex> locker(m_mutex); //对于队列的操作需要加锁保护，注意这里使用了unique_lock去加锁
    m_notEmpty.wait(locker, [this]{return m_needStop || isNotEmpty(); }); // lambda表达式捕获了this指针则可以直接调用private 方法
    //如果队列不为空则往下执行，否则阻塞

    //如果m_needStop标志为true 则直接结束
    if(m_needStop)
        return;

    list = std::move(m_queue);
    m_notFull.notify_one(); //唤醒一个等待Put的线程
}

template <typename T>
void SyncQueue<T>::takeTask(T &x) {
    std::unique_lock<std::mutex> locker(m_mutex);
    m_notEmpty.wait(locker, [this]{return m_needStop || isNotEmpty();});

    if(m_needStop)
        return ;

    x = m_queue.front();
    m_queue.pop_front();

    m_notFull.notify_one();
}
template <typename T>
template <typename F>
void SyncQueue<T>::add(F &&x) {
    std::unique_lock<std::mutex> locker(m_mutex);
    m_notFull.wait(locker, [this]{return m_needStop || isNotFull();});
    if(m_needStop)
        return;
    std::cout << "push" << std::endl;
    m_queue.push_back(std::forward<F>(x));
    m_notEmpty.notify_one();
}

template <typename T>
bool SyncQueue<T>::isEmpty() {
    std::lock_guard<std::mutex> locker(m_mutex);
    return m_queue.empty();
}

template <typename T>
bool SyncQueue<T>::isFull() {
    std::lock_guard<std::mutex> locker(m_mutex);
    return m_queue.size() == m_maxsize;
}

template <typename T>
bool SyncQueue<T>::isNotEmpty() const {
    bool empty = m_queue.empty();
    if(empty){
        std::cout << "buffer is empty... please wait. Thread ID:" << std::this_thread::get_id() << std::endl;
    }
    return !empty;
}

template <typename T>
bool SyncQueue<T>::isNotFull() const {
    bool full = m_queue.size() >= m_maxsize;
    if(full)
        std::cout << "buffer is full ...please wait" << std::endl;
    else
        return !full;
}

template <typename T>
size_t SyncQueue<T>::getSize() {
    std::lock_guard<std::mutex> locker(m_mutex);
    return m_queue.size();
}

template <typename T>
void SyncQueue<T>::stopQueue() {
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_needStop = true;
    }
    m_notFull.notify_all(); //唤醒正在等待Take和Put的线程，然后所有线程return
    m_notEmpty.notify_all();
}
//-------------------------ThreadPool------------------------------------
void ThreadPool::startThreadPool(int numThreads) {
    int real_numThreads;
    if(!numThreads)
        real_numThreads = std::thread::hardware_concurrency();
    else
        real_numThreads  = numThreads;
    m_running = true;
    for(int i = 0;i < real_numThreads; ++i)
        m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::runInThread, this));

}
void ThreadPool::runInThread() {
    while(m_running){
        Task my_task;
        M_queue.takeTask(my_task);
        if(!m_running)
            return;
        else
            doJob(my_task);
    }
}

void ThreadPool::addTask(Task &&task) {
    M_queue.putTask(std::forward<Task>(task));

}
void ThreadPool::stopThreadPool() {
    std::call_once(m_flag, [this]{stopThreadGroup();});
}

void ThreadPool::addTask(const Task &task) {
    M_queue.putTask(task);
}

ThreadPool& ThreadPool::getInstance() {
    static ThreadPool instance;
    return instance;
}

void ThreadPool::doJob(int fd) {
    char buffer[MAX_BUFFER_SIZE];
    std::string job_type;
    Network::recvMessage(fd,buffer,job_type);
    std::cout << "Job type is "+job_type << std::endl;


    BaseJob *myjob = Factory::produce(job_type);
    myjob->doJob(buffer);

    // std::cout << buffer << std::endl;
    Network::sendMessage(fd,buffer);
    delete myjob;
    std::cout << "This job is ok" << std::endl;

}
void ThreadPool::stopThreadGroup() {
    M_queue.stopQueue();
    m_running = false;

    for(auto thread : m_threadgroup)
        if(thread)
            thread->join();
    m_threadgroup.clear();
}