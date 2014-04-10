/* 
 * File:   atomicOperations01.cpp
 * Author: Fabian Kantereit
 *
 * Created on March 28, 2014, 4:54 PM
 * Last modified: April 03, 2014, 10:52 PM
 * 
 * libboost needed
 * linked libraries: pthread , boost_log , boost_log_setup
 */

// standard library
#include <cstdlib>

// threading
#include <thread>
#include <mutex>
#include <atomic>

// Boost libraries for thread-safe logging
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>
#include <boost/integer.hpp>

using namespace boost;

const int maxValue = 10000000;


/*******************************************************************************
 * parallel increment functions
 ******************************************************************************/

/*
 * atomic increment using std::atomic<int> 
 * race condition safe data types
 */
void atomic_increment(std::atomic_int& n, bool mode)
{
    for(int i=0;i<maxValue;++i)
    {
        if(mode)
            n++;
        else
            n--;
    }
}

/*
 * slow protected increment using a lock_guard
 * race condition safe
 */
void slow_protected_increment(int& n, bool mode, std::mutex& m_n)
{
    for(int i=0;i<maxValue;++i)
    {
        std::lock_guard<std::mutex> lk(m_n); // slow locking
        if(mode)
            n++;
        else
            n--;
    }
}

/*
 * unsafe increment
 * demonstrates a race condition
 */
void unsafe_threaded_increment(int& n, bool mode)
{
    for(int i=0;i<maxValue;++i)
    {
        if(mode)
            n++;
        else
            n--;
    }
}


/*******************************************************************************
 * demonstrate_race_condition:
 *      this is why we use locks , 
 *      mutex and atomics and why atomic is preferable
 ******************************************************************************/

void demonstrate_race_condition(void) {
    
    // unsafe variant using no synchronization
    int n=0;
    BOOST_LOG_TRIVIAL(debug) << "initial value of n: " << n;
    std::thread t1(unsafe_threaded_increment,std::ref(n),true);
    std::thread t2(unsafe_threaded_increment,std::ref(n),false);
    t1.join();
    t2.join();
    BOOST_LOG_TRIVIAL(debug) << "value of n: " << n;
    
    // slow variant using a blocking lock_guard
    n=0;
    BOOST_LOG_TRIVIAL(debug) << "initial value of n: " << n;
    std::mutex m;
    std::thread t3(slow_protected_increment,std::ref(n),true, std::ref(m));
    std::thread t4(slow_protected_increment,std::ref(n),false, std::ref(m));
    t3.join();
    t4.join();
    BOOST_LOG_TRIVIAL(debug) << "value of n: " << n;
    
    // alternative using atomic data type and operations
    std::atomic<int> n1(0);
    BOOST_LOG_TRIVIAL(debug) << "initial value of n1: " << n;
    std::thread t5(atomic_increment,std::ref(n1),true );
    std::thread t6(atomic_increment,std::ref(n1),false);
    t5.join();
    t6.join();
    BOOST_LOG_TRIVIAL(debug) << "value of n1: " << n;
    
}

/*******************************************************************************
 * atomic_types_lock_free:
 *      returns if the atomic_types are lockfree this 
 *      may very from system to system depending on the architecture and
 *      implementation
 ******************************************************************************/

void atomic_types_lock_free(void)
{
    std::atomic_bool    aBool(true);
    std::atomic_flag    aFlag(false);
    std::atomic_char    aChar('c');     // std::atomic<char>
    std::atomic_schar 	aSChar('c');    // std::atomic<signed char>
    std::atomic_uchar 	aUChar('c');    // std::atomic<unsigned char>
    std::atomic_short 	aShort(-4);     // std::atomic<short>
    std::atomic_ushort 	aUShort(3);     // std::atomic<unsigned short>
    std::atomic_int 	aInt(255);      // std::atomic<int>
    std::atomic_uint 	aUInt(aUShort); // std::atomic<unsigned int>
    std::atomic_long 	aLong(aInt);    // std::atomic<long>
    std::atomic_ulong 	aULong((unsigned long)aLong); // std::atomic<unsigned long>
    std::atomic_llong 	aLLong(1000000000);// std::atomic<long long>
    std::atomic_ullong 	aULLong(12323213); // std::atomic<unsigned long long>
    std::atomic_char16_t aChar16;       // std::atomic<char16_t>
    std::atomic_char32_t aChar32;	// std::atomic<char32_t>
    std::atomic_wchar_t  aWChar;	// std::atomic<wchar_t>
    std::atomic<double> aDouble;
   
    BOOST_LOG_TRIVIAL(debug) << "lock free?" ;
    BOOST_LOG_TRIVIAL(debug) << "flag  : true (guaranteed) " ;
    BOOST_LOG_TRIVIAL(debug) << "bool  : " << aBool.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "char  : " << aChar.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "uchar : " << aUChar.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "schar : " << aSChar.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "short : " << aShort.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "ushort: " << aUShort.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "int   : " << aInt.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "uint  : " << aUInt.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "long  : " << aLong.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "ulong : " << aULong.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "llong : " << aLLong.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "ullong: " << aULLong.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "char16: " << aChar16.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "char32: " << aChar32.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "wchar : " << aChar32.is_lock_free() ;
    BOOST_LOG_TRIVIAL(debug) << "double: " << aDouble.is_lock_free() ;
}

/*
 * demonstrates various atomic operations
 */
void atomic_operations(void) {
    // std::atomic<int> safe_int = 5; not allowed use constructor instead
    std::atomic<int> safe_int(5);
   
    // initializer list is also permitted
    std::atomic_char c = {'a'};    
    
    // operator=
    safe_int = 6;
    
    // operator T
    int x = safe_int;
    
    BOOST_LOG_TRIVIAL(debug) << "value of x : "  << x;
   
    x = safe_int.exchange(1);
    BOOST_LOG_TRIVIAL(debug) << "value of x : "  << x;
    BOOST_LOG_TRIVIAL(debug) << "value of safe_int : "  << safe_int;
    
    x = safe_int.fetch_add(6);
    BOOST_LOG_TRIVIAL(debug) << "value of x : "  << x;
    BOOST_LOG_TRIVIAL(debug) << "value of safe_int : "  << safe_int;
    
    
    // using load and store
    x = safe_int.load();
    x += 5;
    safe_int.store(x);
    
}


/*******************************************************************************
 * main function
 ******************************************************************************/
int main(int argc, char** argv) {
    // Demonstrate idea of atomic types
    demonstrate_race_condition();
    
    // are atomic types lock free
    atomic_types_lock_free();
    
    // are atomic types lock free
    atomic_operations();
    

    return 0;
}

