/* 
 * @file futures01.cpp
 * @author Fabian Kantreit
 *
 * Created on April 2, 2014, 6:28 PM
 */

#include <cstdlib>
#include <thread>
#include <future>

#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

using namespace std;

int add(int a, int b) {
    BOOST_LOG_TRIVIAL(debug) << "Thread started";
    this_thread::sleep_for(chrono::milliseconds(2000));
    return a + b;
}

/*
 * 
 */
int main(int argc, char** argv) {

    // using packaged_task and detached thread
    std::packaged_task<int(int, int) > task(add);
    std::future<int> value = task.get_future();
    std::thread(std::move(task), 6, 7).detach();
    BOOST_LOG_TRIVIAL(debug) << value.get();

    // using lambda expression and running task locally
    std::packaged_task<double(double, double) > task2([](double a, double b) {
        BOOST_LOG_TRIVIAL(debug) << "Task started";
        this_thread::sleep_for(chrono::milliseconds(2000));
        return a + b; });
        
    std::future<double> value2 = task2.get_future();
    task2(4,7);
    BOOST_LOG_TRIVIAL(debug) << "Task will run in main_thread";
    BOOST_LOG_TRIVIAL(debug) << value2.get();
    
    //using async
    std::future<int> value3 = std::async(std::launch::async, std::bind(add, 3,4));
    BOOST_LOG_TRIVIAL(debug) << "running async -> waiting";
    value3.wait();
    BOOST_LOG_TRIVIAL(debug) << "running async -> ready";
    BOOST_LOG_TRIVIAL(debug) << value3.get();
    
    // using promise
    std::promise<int> p;
    std::future<int> f3 = p.get_future();
    std::thread t1([](std::promise<int> p){ 
        BOOST_LOG_TRIVIAL(debug) << "Value set";
        p.set_value(5);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        BOOST_LOG_TRIVIAL(debug) << "Thread ended";
    }, std::move(p));
    
    f3.wait();
    BOOST_LOG_TRIVIAL(debug) << "Value ready";
    t1.join();
    
    return 0;
}

