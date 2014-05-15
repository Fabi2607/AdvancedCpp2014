/**
 * @file main.cpp
 * Author: fabian
 *
 * Created on May 8, 2014, 9:44 AM
 */

#include <cstdlib>
#include <atomic>
#include <thread>
#include <mutex>

#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

using namespace std;

// implementing a spinlock using atomic_flag

/**
 * simple busy wait spinlock implementation
 */
class spinlock_mutex {
    atomic_flag flag;

  public:
    spinlock_mutex() : flag( ATOMIC_FLAG_INIT ) {}

    void lock() {
		while(flag.test_and_set());
	}

	void unlock() {
		flag.clear();
	}
};

spinlock_mutex m;

void lock_mutex(void) {
	BOOST_LOG_TRIVIAL ( debug ) << "waiting for lock";
	lock_guard<spinlock_mutex> lock(m);
	BOOST_LOG_TRIVIAL ( debug ) << "spinlock locked";
	this_thread::sleep_for( chrono::milliseconds( 10000 ));
}

void spinlock_example( void ) {
	thread t1{lock_mutex};
	thread t2{lock_mutex};

	t1.join();
	t2.join();
}

void comp_exchange_example( void ) {
	atomic<int> a_i;

	int val1 = 0;
	int val2 = 1;
	bool ret_val = false;

	a_i = -1;
	BOOST_LOG_TRIVIAL (debug ) << "values: a_i = " << a_i << " val1 = " << val1 << "val2 = " << val2 << "ret_val = " << ret_val; 
	ret_val = a_i.compare_exchange_strong( val1, val2 );
	BOOST_LOG_TRIVIAL (debug ) << "values: a_i = " << a_i << " val1 = " << val1 << " val2 = " << val2 << " ret_val = " << ret_val; 
	ret_val = a_i.compare_exchange_strong( val1, val2 );
	BOOST_LOG_TRIVIAL (debug ) << "values: a_i = " << a_i << " val1 = " << val1 << " val2 = " << val2 << " ret_val = " << ret_val; 

}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main( int argc, char **argv ) { 
	spinlock_example();
	comp_exchange_example();
	return 0; 
}
