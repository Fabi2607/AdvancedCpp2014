/**
 * @file threading01.cpp
 * 
 * boost libraries needed ( v1.55 or higher recommended )
 * linked libraries: pthread , boost_log , boost_log_setup
 * 
 * @author Fabian Kantereit
 * @date March 28, 2014, 3:53 PM
 * @version 0.1
 * 
 */
#include <cstdlib>
#include <thread>

/**
 * @def BOOST_LOG_DYN_LINK 
 * needed for dynamic linked boost log libraries 
 */
#define BOOST_LOG_DYN_LINK

/** Boost libraries for thread-safe logging */
#include <boost/log/trivial.hpp>


/**
 * prints the current thread id
 */
void foo(void) {
    BOOST_LOG_TRIVIAL(debug) << "foo called in thread: " << std::this_thread::get_id();
}

/**
 * dummy thread function that prints the argument
 * @param n argument to print
 */
void bar(int n) {
    BOOST_LOG_TRIVIAL(debug) << "bar called with argument " << n;
}

/**
 * dummy thread function that sleeps for 500ms
 */
void sleeper(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    BOOST_LOG_TRIVIAL(debug) << "Finished sleeping";
}

/**
 * start simple thread which does nothing
 */
void start_simple_thread(void)
{
    BOOST_LOG_TRIVIAL(trace) << ">>>void start_simple_thread(void)";
    // starting a new the thread
    std::thread t1{foo};

    // join Thread t1
    t1.join();
}

/**
 * start an array of threads
 */
void start_thread_array(void)
{
  BOOST_LOG_TRIVIAL(trace) << ">>>void start_thread_array(void)";
  // declaring an array of threads
  int max_threads = std::thread::hardware_concurrency()-1;
  std::thread t[max_threads]; // "Not-a-thread" threads until constructor is called
  for (int i = 0; i < max_threads; ++i)
    t[i] = std::thread{bar, i};

  // join all threads
  for (int i = 0; i < max_threads; ++i)
    t[i].join();
}

/**
 * start a detached thread
 */
void start_detached_thread(void) {
    BOOST_LOG_TRIVIAL(trace) << ">>>void start_detached_thread(void)";
    // we can detach a thread to make it independent from our handle
    // it will terminate automatically once it is done

    // start a sleeping thread
    std::thread t2{sleeper, 500};

    BOOST_LOG_TRIVIAL(debug) << "t2 joinable: " << t2.joinable();
    t2.detach(); // thread is detached from our local handle
                 // resources will be freed when the thread is terminated
    BOOST_LOG_TRIVIAL(debug) << "t2 joinable: " << t2.joinable();
} // without detach t2 would have been destroyed and the thread terminated

/**
 * perform move and swap operations on threads
 */
void move_swap_thread(void)
{
    BOOST_LOG_TRIVIAL(trace) << ">>>void move_swap_thread(void)";
    // Additionally we can move a thread
    std::thread t3{foo};
    BOOST_LOG_TRIVIAL(debug) << "t3 joinable: " << t3.joinable();
    std::thread t4(std::move(t3)); // constructor call with move => no copies allowed
    BOOST_LOG_TRIVIAL(debug) << "t3 moved to t4 " ;
    BOOST_LOG_TRIVIAL(debug) << "t3 joinable: " << t3.joinable();
    BOOST_LOG_TRIVIAL(debug) << "t4 joinable: " << t4.joinable();
    
    // the assignment operator= is deleted!
    // t3 = t4;
    
    // use move assignment instead
    t3 = std::move(t4);
    BOOST_LOG_TRIVIAL(debug) << "t3 joinable: " << t3.joinable();
    BOOST_LOG_TRIVIAL(debug) << "t4 joinable: " << t4.joinable();
    t4 = std::move(t3);
    
    // we can also swap our handles
    std::thread t5(foo);
    BOOST_LOG_TRIVIAL(debug) << "t4 id: " << t4.get_id();
    BOOST_LOG_TRIVIAL(debug) << "t5 id: " << t5.get_id();
    std::swap(t4,t5);
    BOOST_LOG_TRIVIAL(debug) << "t4 id: " << t4.get_id();
    BOOST_LOG_TRIVIAL(debug) << "t5 id: " << t5.get_id();
    
    // never forget to join
    t4.join();
    t5.join();
}

/**
 * 
 * @param argc shell argument count
 * @param argv shell argument values
 * @return default return value
 */
int main(int argc, char** argv) {
    // Threads supported by hardware
    // BOOST_LOG is used for thread-safe display of Log-Messages
    BOOST_LOG_TRIVIAL(debug) << "Number of Hardware Threads: " << std::thread::hardware_concurrency();

    // get thread id
    BOOST_LOG_TRIVIAL(debug) << "ID of main - thread: " << std::this_thread::get_id();

    // call function foo from main thread
    foo();

    // starting a simple thread
    start_simple_thread();

    // starting an array of threads
    start_thread_array();
    
    // let's start a thread in another function
    start_detached_thread();
   
    // we can also swap and move
    move_swap_thread();
    
    // detached thread is still running, be careful
    // what happens to detached threads at end of main?
    // >>>
    // http://stackoverflow.com/questions/19744250/c11-what-happens-to-a-detached-thread-when-main-exits
    // std::this_thread::sleep_for(std::chrono::seconds(1));

    BOOST_LOG_TRIVIAL(debug) << "End of Main Thread";

    return EXIT_SUCCESS;
}

