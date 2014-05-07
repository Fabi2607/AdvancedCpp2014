/**
 * @file ThreadPool.cpp
 * @author fabian
 * 
 * @date April 3, 2014, 3:23 PM
 */

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(unsigned int threads) {
    for(int i = 0; i<threads;++i)
    {
        threadpool_.create_thread([this]{
            while(true)
            {
                std::unique_lock<std::mutex> lock(this->mutex_);
                while(!this->stop_ && this->tasks_.empty())
                    work_available_.wait(lock);
                if(this->stop_ && this->tasks_.empty())
                        return;
                std::function<void()> task(this->tasks_.front());
                this->tasks_.pop();
                lock.unlock();
                task();
            }});
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
    }
    work_available_.notify_all();
    threadpool_.join_all();
}

