/**
 * @file barrier01.cpp
 * @author Fabian Kantereit
 *
 * @date April 3, 2014, 10:00 AM
 *
 * demonstrates the use of boost barrier for thread synchronization
 * boost library needed
 *
 */

#include <cstdlib>
#include <thread>

/**
 * @def BOOST_LOG_DYN_LINK
 * needed for dynamic linked boost log libraries
 */
#define BOOST_LOG_DYN_LINK
#include <boost/thread/barrier.hpp>
#include <boost/log/trivial.hpp>

using namespace std;

/**
 * thread_wait:
 *      wait for ms until barrier is reached
 * keywords:
 *      boost::barrier
 */
void thread_wait( std::chrono::milliseconds ms, boost::barrier &waitPoint ) {
    BOOST_LOG_TRIVIAL( debug ) << "Thread started";
    std::this_thread::sleep_for( ms );
    BOOST_LOG_TRIVIAL( debug ) << "reached waitPoint";

    waitPoint.wait();
    BOOST_LOG_TRIVIAL( debug ) << "Thread ended";
}

/**
 * main function
 */
int main( int argc, char **argv ) {
    boost::barrier b( 6 );

    for ( int i = 0; i < 5; ++i ) {
        std::thread( thread_wait, std::chrono::milliseconds( i * 2000 ),
                     std::ref( b ) ).detach();
    }

    BOOST_LOG_TRIVIAL( debug ) << "main Thread waiting";
    b.wait();
    BOOST_LOG_TRIVIAL( debug ) << "main Thread finished waiting";
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
    return 0;
}
