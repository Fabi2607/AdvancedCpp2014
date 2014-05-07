/* 
 * File:   main.cpp
 * Author: fabian
 *
 * Created on May 7, 2014, 2:12 PM
 */

#include <cstdlib>
#include <thread>

#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

using namespace std;

class backgroundWorker {

public:
    void operator() () {
        BOOST_LOG_TRIVIAL(debug) << "doing background work";
    }
};

/**
 * 
 */
int main(int argc, char** argv) {

    // callable object
    backgroundWorker b; 
    thread t{b};
    
    // careful this would be a function definition:
    // thread t(b());
    t.join();
    
    return 0;
}

