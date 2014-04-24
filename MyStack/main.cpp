/* 
 * File:   main.cpp
 * Author: fabian
 *
 * Created on April 17, 2014, 11:19 AM
 */

#include <cstdlib>
#include "MyStack.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    MyStack<double> my;
    

    my.push(6);
    my.push(7);
    my.push(8);
    my.push(8);
    my.push(8);
    my.push(8);
    
    
    std::cout<< my.pop();
    std::cout<< my.pop();
    std::cout<< my.pop() ;
    std::cout<< my.pop() ;
    std::cout<< my.pop() ;
    std::cout<< my.pop() ;
    try {
        std::cout<< my.pop() ;
        std::cout<< my.pop() ;
    } catch(StackException& e) {
        cout << e.what() <<endl ;
    }
    
    MyStack<string> stringStack;
    

    stringStack.push("Welt!");
    stringStack.push(" ");   
    stringStack.push("Hallo");
    std::cout <<  stringStack.pop();
    std::cout <<  stringStack.pop();
    std::cout <<  stringStack.pop();

    return 0;
}

