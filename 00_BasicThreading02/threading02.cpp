/**
 * @file  threading02.cpp
 *
 * Shows the use of callable objects instead of functions
 *
 * @author Fabian Kantereit
 * @date May 7, 2014, 2:12 PM
 */

#include <cstdlib>
#include <thread>
#include <mutex>

/**
 * @def BOOST_LOG_DYN_LINK
 * needed for dynamically linked boost log libraries
 */
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

using namespace std;

// create a worker class
class backgroundWorker {
    // private members, this will be useful e.g. for promises

  public:
    // define a a call operator
    void operator()() { BOOST_LOG_TRIVIAL( debug ) << "doing background work"; }
};

void foo ( void ) {
	BOOST_LOG_TRIVIAL ( debug ) << "foo called";
}

/**
 * main function
 */
int main( int argc, char **argv ) {

    // callable object
    backgroundWorker b;
    thread t{b};

    // careful this would be a function definition:
    // thread t(b());

    t.join();

	once_flag flag;
	auto f = [&]{
		call_once(flag, foo);
	};
	
	std::thread ts[thread::hardware_concurrency()-1];
	for(int i = 0; i < thread::hardware_concurrency()-1; ++i){
		ts[i] = thread{f};
		ts[i].detach();
	}


	// undefined behaviour, why?
    return 0;
}
