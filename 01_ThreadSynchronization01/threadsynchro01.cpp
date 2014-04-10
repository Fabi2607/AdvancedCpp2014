/* 
 * File:   main.cpp
 * Author: fabian
 *
 * Created on April 10, 2014, 11:29 AM
 * 
 * boost library needed
 * linked libraries: pthread , boost_log , boost_log_setup , boost_thread ,
 *                      boost_chrono, boost_system
 * 
 * this is not working probably on my machine  
 *  libstdc++ / gcc-4.8.2 seems to be buggy so I used the boost threading library
 */

// comment out for stl
#define USE_BOOST

// Boost libraries for thread-safe logging
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

#ifdef USE_BOOST
    #include <boost/thread.hpp>
    #include <boost/thread/mutex.hpp>
    using namespace boost;
#else
    #include <cstdlib>
    #include <thread>
    #include <chrono>
    #include <mutex>
    using namespace std;
#endif

void use_mutexes(void)
{
    mutex m;
    m.lock();
    BOOST_LOG_TRIVIAL(debug) << "m locked" ;
    // simple mutex lock
    thread t1([&](void) {
       BOOST_LOG_TRIVIAL(debug) << "waiting for m" ;
       
       m.lock(); // blocking call until mutex is unlocked 
       
       BOOST_LOG_TRIVIAL(debug) << "lock acquired" ;
       
       // critical section
       this_thread::sleep_for(chrono::milliseconds(1000));
       
       m.unlock(); // release mutex
       BOOST_LOG_TRIVIAL(debug) << "lock released" ;
    });
    
    this_thread::sleep_for(chrono::milliseconds(3000));
    m.unlock();
    t1.join();

    // timed mutex  
    timed_mutex tm;

    tm.lock();
    thread t2([&](void) {
       BOOST_LOG_TRIVIAL(debug) << "waiting for m" ;
       
       while(!tm.try_lock_for(chrono::milliseconds(500))) 
            BOOST_LOG_TRIVIAL(debug) << "locking timed out after 500ms" ;  
       
       BOOST_LOG_TRIVIAL(debug) << "lock acquired" ;
       
       // critical section
       this_thread::sleep_for(chrono::milliseconds(1000));
       
       tm.unlock(); // release mutex
       BOOST_LOG_TRIVIAL(debug) << "lock released" ;
    });
    
    this_thread::sleep_for(chrono::milliseconds(3000));
    tm.unlock();
    
    t2.join();
}

void use_recursive_mutexes(void)
{
    
}

void use_locks(void)
{
    
}
/*
 * 
 */
int main(int argc, char** argv) {

    use_mutexes();
  
    return 0;
}

