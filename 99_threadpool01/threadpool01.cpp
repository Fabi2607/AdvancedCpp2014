/*
 * File:   threadpool01.cpp
 * Author: fabian
 *
 * Created on April 3, 2014, 3:21 PM
 */

#include <cstdlib>

#include "ThreadPool.hpp"

using namespace std;

int task( int i ) { return i; }

/*
 *
 */
int main( int argc, char **argv ) {
    ThreadPool pool( 6 );

    auto future = pool.addTask( &task, 1 );

    cout << "Value: " << future.get() << endl;

    return 0;
}
