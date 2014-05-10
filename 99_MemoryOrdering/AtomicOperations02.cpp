/** 
 * @file AtomicOperations02.cpp
 * @author Fabian Kantereit
 *
 * @date April 2, 2014, 4:51 PM
 */

#include <cstdlib>
#include <atomic>
#include <thread>

// Boost library for logging
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>
using namespace boost;

const int maxValue = 100000000;

void atomic_relaxed_increment(std::atomic_int& n, bool mode)
{
    for(int i=0;i<maxValue;++i)
    {
        if(mode)
            n.fetch_add(1, std::memory_order_relaxed);
        else
            n.fetch_sub(1, std::memory_order_relaxed);
    }
}

void atomic_seq_cst_increment(std::atomic_int& n, bool mode)
{
    for(int i=0;i<maxValue;++i)
    {
        if(mode)
            n.fetch_add(1, std::memory_order_seq_cst);
        else
            n.fetch_sub(1, std::memory_order_seq_cst);
    }
}

void atomic_memory_order_increment(std::atomic_int& n, bool mode, std::memory_order order)
{
    for(int i=0;i<maxValue;++i)
    {
        if (mode) {
            // note that this will NOT work like a lock

            n.fetch_add(1, order);
        }
        else
        {
            n.fetch_sub(1, order);
        }
    }
}

/**
 * demonstrates different memory order options
 */
void memory_order_options(void)
{
    // memory order allows for thread synchronization in addition to race condition prevention
    // useful if multiple operations are performed on a thread
    /*
     enum memory_order {
        memory_order_relaxed, // no synchronization only atomicity
        memory_order_consume, // consume on load
        memory_order_acquire, // acquire on load
        memory_order_release, // release on store
        memory_order_acq_rel, // performs acquire on load and release on store
        memory_order_seq_cst  // sequentially consistent ordering
     };
    */
    
    // examples
    
    std::thread t1;
    std::thread t2;
    std::atomic_int n = {0};
    
    // relaxed ordering: only read-modify-write atomicity is required
    n = 0;
    BOOST_LOG_TRIVIAL(debug) << "initial value of n: " << n;
    auto begin = std::chrono::high_resolution_clock::now();  
    t1 = std::thread(atomic_memory_order_increment, std::ref(n), true, std::memory_order_relaxed);
    t2 = std::thread(atomic_memory_order_increment, std::ref(n), false, std::memory_order_relaxed);
    t1.join();
    t2.join();
    BOOST_LOG_TRIVIAL(debug) << "value of n: " << n;
    BOOST_LOG_TRIVIAL(debug) << "elapsed time: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::high_resolution_clock::now()-begin)).count();
    
    // acquire-release ordering synchronizes this variable
    n = 0;
    BOOST_LOG_TRIVIAL(debug) << "initial value of n: " << n;
    begin = std::chrono::high_resolution_clock::now();  
    t1 = std::thread(atomic_memory_order_increment, std::ref(n), true, std::memory_order_acq_rel);
    t2 = std::thread(atomic_memory_order_increment, std::ref(n), false, std::memory_order_acq_rel);
    t1.join();
    t2.join();
    BOOST_LOG_TRIVIAL(debug) << "value of n: " << n;
    BOOST_LOG_TRIVIAL(debug) << "elapsed time: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::high_resolution_clock::now()-begin)).count();
    
    // sequentially consistent ordering provides full synchronization
    // default, always use this if you don't know exactly what  you are doing!
    // The performance differences are minimal
    n = 0;
    BOOST_LOG_TRIVIAL(debug) << "initial value of n: " << n;
    begin = std::chrono::high_resolution_clock::now();  
    t1 = std::thread(atomic_memory_order_increment, std::ref(n), true, std::memory_order_seq_cst);
    t2 = std::thread(atomic_memory_order_increment, std::ref(n), false, std::memory_order_seq_cst);
    t1.join();
    t2.join();
    BOOST_LOG_TRIVIAL(debug) << "value of n: " << n;
    BOOST_LOG_TRIVIAL(debug) << "elapsed time: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::high_resolution_clock::now()-begin)).count();
}

/**
 * 
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char** argv) {

    // demonstrates performances differences on 
    memory_order_options();
    
    return 0;
}

