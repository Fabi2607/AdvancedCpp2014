/* 
 * File:   MyStack.h
 * Author: fabian
 *
 * Created on April 17, 2014, 11:22 AM
 */

#ifndef MYSTACK_H
#define	MYSTACK_H

#include <iostream>
#include <cstdlib>
#include <exception>
#include <memory>

template<class T>
class MyStack {
public:
    MyStack() : v_(NULL), vsize_(4), vused_(0) {
        try {
            v_ = new std::unique_ptr<T>[vsize_];
        } catch(std::exception_ptr e) {
            if (v_ != NULL)
                delete [] v_;
            vsize_ = 0;
            std::rethrow_exception(e);
        }
    };
    
    void push(const T& value) {
        if (vused_ == vsize_) {
            try {
                std::unique_ptr<T>* new_array = new std::unique_ptr<T>[vsize_*2];
                std::move(v_,v_+vsize_, new_array);
                vsize_ *= 2;
                v_ = new_array;
            } catch (std::bad_alloc) {
                std::cout << "memory allocation failed";
            } catch (std::exception_ptr e) {
                std::rethrow_exception(e);
            }
        }
        
        try {
           v_[vused_] = std::unique_ptr<T>(new T(value));
        } catch (std::exception_ptr e) {
            std::rethrow_exception(e);
        }
        
        ++vused_;
    }
    
    T pop(void) {
        return (vused_ > 0 ) ? *(v_[--vused_]) : NULL;
    }
    
    MyStack(const MyStack& orig);
    virtual ~MyStack() {
        delete [] v_;
    };
private:
    std::unique_ptr<T>* v_;
    size_t vsize_;
    size_t vused_;
};

template<>
int MyStack<int>::pop() {
   return (vused_ > 0 ) ? *(v_[--vused_]) : 0;
}

#endif	/* MYSTACK_H */

