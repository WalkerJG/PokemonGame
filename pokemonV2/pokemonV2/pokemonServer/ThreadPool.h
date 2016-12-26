#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <mutex>
#include <functional>
#include <string>
#include <condition_variable>
#include <deque>
#include <vector>
#include <memory>

#include "nocopyable.h"
namespace myThreadPool
{
    class ThreadPool :public nocopyable
    {
    public:
        typedef std::function<void()> Task;

        explicit ThreadPool(const std::string &name = std::string());
        ~ThreadPool();


        void start(int numThreads);//设置线程数，创建numThreads个线程
        void stop();//线程池结束
        void run(const Task& f);//任务f在线程池中运行
        void setMaxQueueSize(int maxSize) { _maxQueueSize = maxSize; }//设置任务队列可存放最大任务数

    private:
        bool isFull();//任务队列是否已满
        void runInThread();//线程池中每个thread执行的function
        Task take();//从任务队列中取出一个任务

        std::mutex _mutex;
        std::condition_variable _notEmpty;
        std::condition_variable _notFull;
        std::string _name;
        std::vector<std::thread> _threads;
        std::deque<Task> _queue;
        size_t _maxQueueSize;
        bool _running;
    };
}

#endif // THREADPOOL_H
