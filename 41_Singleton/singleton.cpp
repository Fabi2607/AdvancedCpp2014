/* 
 * File:   singleton.cpp
 * Author: fabian
 *
 * Created on May 7, 2014, 1:06 PM
 */

#include <cstdlib>
#include <mutex>

using namespace std;
#define Version4

#ifdef Version1

class A {

    static A* instance_;
    // can you see the problem?
public:
    static A* get_instance() {
        if(!instance_)
            instance_ = new A();
        return instance_;
    }
};

A* A::instance_ = nullptr;

#endif

#ifdef Version2
class A {
    static A* instance_;
    static std::mutex mA_;
    
    static A* get_instance() {
        std::unique_lock<std::mutex> lock(mA_); // this line is slow
        if(!instance_)
            instance_ = new A();
        lock.unlock();
        return instance_;
    }
};

A* A::instance_ = nullptr;

#endif

#ifdef Version3


class A {
    static A* instance_;
    static std::once_flag flag_; // if you know the stl
    
    static A* get_instance() {
        std::call_once(flag_, [&](){instance_ = new A();});
        return instance_;
    }
};

A* A::instance_ = nullptr;

#endif 

#ifdef Version4

// How you should do it
// guaranteed to be thread-safe in C++11
// make sure your compiler is fully C++11 compliant

class A {
public:
    static A& get_instance() {
        static A instance; 
        return instance;
    }
};


#endif 

int main(int argc, char** argv) {

    A& a = A::get_instance();

    
    return 0;
}

