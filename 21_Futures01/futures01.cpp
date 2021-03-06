/*
 * @file futures01.cpp
 * @author Fabian Kantreit
 *
 * @date Created on April 2, 2014, 6:28 PM
 */

#include <cstdlib>
#include <thread>
#include <future>

#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

using namespace std;

/**
 * sleeps for 2s and adds a and b
 * @param a
 * @param b
 * @return
 */
int add( int a, int b ) {
    BOOST_LOG_TRIVIAL( debug ) << "Thread started";
    this_thread::sleep_for( chrono::milliseconds( 2000 ) );
    return a + b;
}

class Worker {
  public:
    std::promise<int> result;

    void operator()() {
        BOOST_LOG_TRIVIAL( debug ) << "Value set";
        result.set_value( 5 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );
        BOOST_LOG_TRIVIAL( debug ) << "Thread ended";
    }
};

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main( int argc, char **argv ) {

    // using packaged_task and detached thread
    std::packaged_task<int(int, int)> task( add );
    std::future<int> value = task.get_future();
    
    std::thread( std::move( task ), 6, 7 ).detach();
    BOOST_LOG_TRIVIAL( debug ) << value.get();

    // using lambda expression and running task locally
    std::packaged_task<double(double, double)> task2( []( double a, double b ) {
        BOOST_LOG_TRIVIAL( debug ) << "Task started";
        this_thread::sleep_for( chrono::milliseconds( 2000 ) );
        return a + b;
    } );

    std::future<double> value2 = task2.get_future();
    task2( 4, 7 );	
	
    BOOST_LOG_TRIVIAL( debug ) << "Task will run in main_thread";
    BOOST_LOG_TRIVIAL( debug ) << value2.get();

    auto f = [](double a){
	BOOST_LOG_TRIVIAL( debug ) << "Task started with arg " << a;
	return a;
    };

    // using async
    std::future<double> f_value = std::async(std::launch::async, f, 3);

    // using async and bind
    std::future<int> value3 =
        std::async( std::launch::async, std::bind( add, 3, 4 ) );
    BOOST_LOG_TRIVIAL( debug ) << "running async -> waiting";
    value3.wait();
    BOOST_LOG_TRIVIAL( debug ) << "running async -> ready";
    BOOST_LOG_TRIVIAL( debug ) << value3.get();

    // using promise
    Worker w;
    std::future<int> f3 = w.result.get_future();
    std::thread t1{std::move( w )};
    BOOST_LOG_TRIVIAL( debug ) << "Value ready: " << f3.get();
    t1.join();

    return 0;
}
