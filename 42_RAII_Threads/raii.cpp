/*
 * File:   main.cpp
 * Author: fabian
 *
 * Created on May 7, 2014, 2:47 PM
 */

#include <cstdlib>
#include <thread>
#include <iostream>

using namespace std;

void unsafe_function() {

    // where is the problem here ?
    // how to solve it?
    int n = 42;

    std::thread t1( [&]() {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
        cout << n << endl;
    } );
    t1.detach();
    // what happens now?
}

class thread_guard {
    std::thread &t_;

  public:
    explicit thread_guard( std::thread &t ) : t_( t ) {}

    ~thread_guard() {
        if ( t_.joinable() ) {
            t_.join();
        }
    }

    thread_guard( const thread_guard & ) = delete;
    thread_guard &operator=( const thread_guard & ) = delete;
};

/**
 *
 */
int main( int argc, char **argv ) {

    unsafe_function();
    std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) );
    std::thread t1{[]() { cout << "safe" << endl; }};
    thread_guard g{t1}; // threads are joined, even if an exception is thrown
    return 0;
}
