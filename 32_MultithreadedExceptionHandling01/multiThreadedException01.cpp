/**
 * @file multiThreadedException01.cpp
 * @author fabian
 *
 * Created on April 3, 2014, 11:40 AM
 */

#include <cstdlib>
#include <future>

#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>

using namespace std;

/**
 * demonstration of exceptions thrown by std::future and std::promise
 */
void future_errors( void ) {
    // std::future : no_state
    std::future<int> empty_future;
    try {
        empty_future.get();
    }
    catch ( std::future_error &e ) {
        BOOST_LOG_TRIVIAL( error ) << "Code: " << e.code();
        BOOST_LOG_TRIVIAL( error ) << "Message: " << e.what();
        BOOST_LOG_TRIVIAL( error ) << "std::future_errc::no_state";
    }

    // std::promise : future_already_retrieved
    std::promise<int> p;
    try {
        p.get_future();
        p.get_future();
    }
    catch ( std::future_error &e ) {
        BOOST_LOG_TRIVIAL( error ) << "Code: " << e.code();
        BOOST_LOG_TRIVIAL( error ) << "Message: " << e.what();
        BOOST_LOG_TRIVIAL( error )
            << "std::future_errc::future_already_retrieved";
    }

    // std::promise : promise already satisfied
    std::promise<int> promise;
    try {
        promise.set_value( 5 );
        promise.set_value( 6 );
    }
    catch ( std::future_error &e ) {
        BOOST_LOG_TRIVIAL( error ) << "Code: " << e.code();
        BOOST_LOG_TRIVIAL( error ) << "Message: " << e.what();
        BOOST_LOG_TRIVIAL( error )
            << "std::future_errc::promise_already_satisfied";
    }

    // std::promise : broken promise
    std::future<void> future;
    {
        std::promise<void> p1;
        future = p1.get_future();
    }
    try {
        future.get();
    }
    catch ( std::future_error &e ) {
        BOOST_LOG_TRIVIAL( error ) << "Code: " << e.code();
        BOOST_LOG_TRIVIAL( error ) << "Message: " << e.what();
        BOOST_LOG_TRIVIAL( error ) << "std::future_errc::broken_promise";
    }
}

/**
 * return exceptions with a exception_ptr
 * @param exc_thrown
 */
void threaded_exception( std::exception_ptr &exc_thrown ) {
    try {
        // do critical stuff here
        throw std::exception();
        // throw 0;
        // int i = 1 / 0;
    }
    catch ( ... ) {
        exc_thrown = std::current_exception();
        return;
    }
}

/**
 * demonstrate exception ptr
 */
void exception_example( void ) {
    exception_ptr thrown_exc;

    std::thread t( threaded_exception, std::ref( thrown_exc ) );
    t.join();
    try {
        if ( thrown_exc ) {
            rethrow_exception( thrown_exc );
        }
    }
    catch ( std::exception &e ) {
        BOOST_LOG_TRIVIAL( error ) << "Error: " << e.what();
    }
    catch ( ... ) {
        BOOST_LOG_TRIVIAL( error ) << "Generic Error";
    }
}

void promise_store_exception( void ) {
    // use futures / promises to store exception
    // this allows to throw exceptions for each result the thread
    // useful if a thread is working on many operation and can continue
    std::promise<int> result;

    std::thread( [&]( void ) {
                     try {
                         throw std::exception();
                     }
                     catch ( ... ) {
                         result.set_exception( std::current_exception() );
                     }
                 } ).detach();

    try {
        BOOST_LOG_TRIVIAL( debug ) << result.get_future().get();
    }
    catch ( std::exception &e ) {
        BOOST_LOG_TRIVIAL( error ) << e.what();
    }

    // std::async will store excpetions in the future
    std::future<void> result2 = std::async( [] { throw std::exception(); } );
    try {
        result2.get();
    }
    catch ( std::exception &e ) {
        BOOST_LOG_TRIVIAL( error ) << "Autostore: " << e.what();
    }
}

/*
 *
 */
int main( int argc, char **argv ) {

    // errors concerning the futures
    future_errors();

    // exception handling in threads
    exception_example();

    // store exception in promise / future
    promise_store_exception();
    return 0;
}
