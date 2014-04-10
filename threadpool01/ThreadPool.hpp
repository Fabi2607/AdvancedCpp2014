/* 
 * File:   ThreadPool.hpp
 * Author: fabian
 *
 * Created on April 3, 2014, 3:23 PM
 */

#ifndef THREADPOOL_HPP
#define	THREADPOOL_HPP

#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <boost/thread.hpp>

class ThreadPool {
public:
    ThreadPool(unsigned int);
    ThreadPool(const ThreadPool& orig) = delete; // Threadpool should be non-copyable

    template<class Function, class... Args>
    auto addTask(Function&& f, Args&&... args) -> std::future< typename std::result_of<Function(Args...)>::type > {
        typedef typename std::result_of < Function(Args...)>::type return_type;

        if (stop_)
            throw std::runtime_error("ThreadPool already stopped!");

        auto task = std::make_shared < std::packaged_task < return_type()> >(
                std::bind(std::forward<Function>(f), std::forward<Args>(args)...)
                );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.push([task]() {
                (*task)(); });
        }

        work_available_.notify_one();
        return res;
    }

    virtual ~ThreadPool();

private:
    boost::thread_group threadpool_;
    bool stop_ = false;
    std::mutex mutex_;

    std::condition_variable work_available_;
    std::queue< std::function<void()> > tasks_;

};

#endif	/* THREADPOOL_HPP */

