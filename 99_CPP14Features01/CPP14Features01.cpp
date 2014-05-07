/* 
 * File:   main.cpp
 * Author: fabian
 *
 * this file needs to be compiled with --std=c++1y
 */

#include <cstdlib>

#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>
#include <boost/thread/shared_mutex.hpp>
//#include <shared_mutex> // not supported yet in libstdc++

using namespace boost;



/*
 * 
 */
int main(int argc, char** argv) {
    
    boost::shared_mutex m;
    // std::shared_timed_mutex
    
    return 0;
}

