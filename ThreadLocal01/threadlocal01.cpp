/* 
 * File:   main.cpp
 * Author: fabian
 *
 * Created on April 9, 2014, 12:53 PM
 */

#include <cstdlib>
#include <thread>

#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

using namespace std;

thread_local int i = 1;

class Counter {
public:
    thread_local static int cnt;
    
    Counter()
    {
        cnt++;
    }
    
    ~Counter()
    {
        cnt--;
    }
    
};

thread_local int Counter::cnt = 0;

void inc()
{
    i+=10;
    BOOST_LOG_TRIVIAL(debug) << i;
}

/*
 * 
 */
int main(int argc, char** argv) {
    thread t1(inc);
    thread t2(inc);
    
    t1.join();
    t2.join();
    
    BOOST_LOG_TRIVIAL(debug) << i;

    Counter c1;
    thread t3([](void){Counter c,c2,c3; BOOST_LOG_TRIVIAL(debug) << "local thread objects: " << Counter::cnt; });
    
    t3.join();
    BOOST_LOG_TRIVIAL(debug) << Counter::cnt;
    return 0;
}

