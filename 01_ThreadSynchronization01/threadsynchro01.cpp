/**
 * @file threadsynchro01.cpp
 *
 * boost libraries needed ( v1.55 or higher recommended )
 * linked libraries: pthread , boost_log , boost_log_setup , boost_thread ,
 *                      boost_chrono, boost_system
 *
 * this is not working probably on my machine
 *  libstdc++ / gcc-4.8.2 seems to be buggy so I used the boost threading
 *library
 *
 * @author Fabian Kantereit
 * @date April 10, 2014, 11:29 AM
 */

/**
 * @def USE_BOOST
 * USE_BOOST enables the usage of boost libraries instead of stl,
 * stl's timed_mutex is not working as expected
 */
#define USE_BOOST

/**
 * @def BOOST_LOG_DYN_LINK
 * needed for dynamic linked boost log libraries
 */
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

#include <iostream>

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

/**
 * demonstrates mutex , timed_mutex , lock , try_lock , try_lock_for
 */
void use_mutexes( void ) {
    mutex m;
    m.lock();
    BOOST_LOG_TRIVIAL( debug ) << "m locked";

    // simple mutex lock
    thread t1( [&]( void ) {
        BOOST_LOG_TRIVIAL( debug ) << "waiting for m";

        // blocking until mutex is unlocked
        m.lock();

        BOOST_LOG_TRIVIAL( debug ) << "lock acquired";

        // critical section
        this_thread::sleep_for( chrono::milliseconds( 1000 ) );

        m.unlock(); // release mutex
        BOOST_LOG_TRIVIAL( debug ) << "lock released";
    } );

    this_thread::sleep_for( chrono::milliseconds( 3000 ) );
    m.unlock();
    t1.join();

    // timed mutex
    timed_mutex tm;

    tm.lock();
    thread t2( [&]( void ) {
        BOOST_LOG_TRIVIAL( debug ) << "waiting for m";

        // try locking until time limit is reached, avoids/reveals deadlocking
        while ( !tm.try_lock_for( chrono::milliseconds( 500 ) ) )
            BOOST_LOG_TRIVIAL( debug ) << "locking timed out after 500ms";

        BOOST_LOG_TRIVIAL( debug ) << "lock acquired";

        // critical section
        this_thread::sleep_for( chrono::milliseconds( 1000 ) );

        tm.unlock(); // release mutex
        BOOST_LOG_TRIVIAL( debug ) << "lock released";
    } );

    // wait until releasing the mutex
    this_thread::sleep_for( chrono::milliseconds( 3000 ) );

    tm.unlock();

    t2.join();
}

/**
 * demonstrates the usage of recursive mutexes
 */
void use_recursive_mutexes( void ) {
    static int limit = 0;
    static recursive_mutex m;

    {
        lock_guard<recursive_mutex> lock( m );
        BOOST_LOG_TRIVIAL( debug ) << "entered protected area";
        while ( ++limit < 10 ) {
            use_recursive_mutexes();
        }
    }
    BOOST_LOG_TRIVIAL( debug ) << "lock released";
}

/**
 * demonstrates the use of locks, lock_guards, unique_locks
 */
void use_condition_variables( void ) {
    mutex m;
    bool ready = false; // prevent spurious wakeups
    condition_variable cv;

    auto f = [&]( int n ) {
        unique_lock<mutex> lock( m );
        cv.wait( lock, [&] { return ready; } ); // wait for lock and condition
        // here the lock is acquired
        BOOST_LOG_TRIVIAL( debug ) << "Thread " << n << "signaled";
    };

    thread t1{f, 1};
    thread t2{f, 2};
    thread t3{f, 3};

    BOOST_LOG_TRIVIAL( debug ) << "notify all";
    cv.notify_all();

    BOOST_LOG_TRIVIAL( debug ) << "set ready";
    ready = true;

    BOOST_LOG_TRIVIAL( debug ) << "notify one";
    cv.notify_one();
    this_thread::sleep_for( chrono::milliseconds( 500 ) );

    BOOST_LOG_TRIVIAL( debug ) << "notify all";
    cv.notify_all();
}

/**
 * @param argc unused
 * @param argv unused
 * @return default return value
 */
int main( int argc, char **argv ) {
    std::cout << "(1) mutex (2) recursive_mutexes (3) condition_variable"
              << std::endl;
    std::cout << "Input: ";

    int n;
    std::cin >> n;

    switch ( n ) {
        case 1:
            use_mutexes();
            break;
        case 2:
            use_recursive_mutexes();
            break;
        case 3:
            use_condition_variables();
            break;
    }

    return EXIT_SUCCESS;
}
