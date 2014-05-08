/* 
 * File:   main.cpp
 * Author: fabian
 *
 * Created on May 8, 2014, 1:18 PM
 */

#include <cstdlib>
#include <exception>
#include <stack>
#include <mutex>
#include <memory>

using namespace std;

struct empty_stack_exception : std::exception
{
    const char* what() const throw();
};


// where is the problem with this code?
template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack(){}
    
    threadsafe_stack(const threadsafe_stack& other)
    {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }
    
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;
    
    void push(T new_value) 
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));
    }
    
    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack_exception();
        std::shared_ptr<T> const res(std::make_shared(std::move(data.top())));
        data.pop();
        return res;
    }
    
    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);
        value=std::move(data.top());
        data.pop();
    }
    
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
    }
   
};

int main(int argc, char** argv) {

    return 0;
}

