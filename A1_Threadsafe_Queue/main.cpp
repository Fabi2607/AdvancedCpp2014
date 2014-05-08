/* 
 * File:   main.cpp
 * Author: fabian
 *
 * Created on May 8, 2014, 9:20 AM
 */

#include <cstdlib>
#include <memory>
#include <condition_variable>

using namespace std;


#ifndef Problem

template<typename T>
class threadsafe_queue {
private:

    struct node {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::mutex tail_mutex;

    std::unique_ptr<node> head;
    node* tail;

    std::condition_variable data_cond;
public:
    threadsafe_queue(): head(new node),tail(head.get()) {}
    
    threadsafe_queue(const threadsafe_queue& other) = delete;
    threadsafe_queue& operator=(const threadsafe_queue& other) = delete;
    
    std::shared_ptr<T> try_pop();
    bool try_pop(T& value);
};

#endif



/*
 * 
 */
int main(int argc, char** argv) {

    return 0;
}

