/* 
 * File:   barrier01.cpp
 * Author: fabian
 *
 * Created on April 3, 2014, 10:00 AM
 */

#include <cstdlib>
#include <thread>



#define BOOST_LOG_DYN_LINK
#include<boost/thread/barrier.hpp>
#include<boost/log/trivial.hpp>
#include <condition_variable>

using namespace std;

void thread_wait(std::chrono::milliseconds ms, boost::barrier& waitPoint)
{
    BOOST_LOG_TRIVIAL(debug) << "Thread started";
    std::this_thread::sleep_for(ms);
    BOOST_LOG_TRIVIAL(debug) << "reached waitPoint";
    
    waitPoint.wait();
    BOOST_LOG_TRIVIAL(debug) << "Thread ended";
}

/*
 * 
 */
int main(int argc, char** argv) {
    boost::barrier b(6);
    
    for (int i = 0; i < 5; ++i)
        std::thread(thread_wait, std::chrono::milliseconds(i*2000), std::ref(b)).detach();
    
    BOOST_LOG_TRIVIAL(debug) << "main Thread waiting";
    b.wait();
    BOOST_LOG_TRIVIAL(debug) << "main Thread finished waiting";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return 0;
}
