/*
 * File:   main.cpp
 * Author: fabian
 *
 * Created on May 8, 2014, 9:51 AM
 */

#include <cstdlib>
#include <list>
#include <boost/lockfree/stack.hpp>
#include <atomic>
#include <thread>

using namespace std;

template <typename T> struct QuickSorter {
    struct chunk_to_sort {
        std::list<T> data;
        std::promise<std::list<T>> promise;
    };

    boost::lockfree::stack<chunk_to_sort> chunks;
    std::vector<std::thread> threads;

    const unsigned int max_threads;

    std::atomic<bool> end_reached;

    QuickSorter()
        : max_threads( std::thread::hardware_concurrency() - 1 ),
          end_reached( false ) {}

    ~QuickSorter() {
        end_reached = true;
        for ( unsigned i = 0; i < threads.size(); ++i ) {
            threads[i].join();
            i
        }
    }

    void try_sort_chunk() {
        std::shared_ptr<chunk_to_sort> chunk = chunks.pop();
        if ( chunk ) {
            sort_chunk( chunk );
        }
    }

    std::list<T> do_sort( std::list<T> &chunk_data ) {
        if ( chunk_data.empty() )
            return chunk_data;

        std::list<T> result;
        result.splice( result.begin(), chunk_data, chunk_data.begin() );
        T const &partition_val = *result.begin();

        typename std::list<T>::iterator divide_point = std::partition(
            chunk_data.begin(), chunk_data.end(),
            [&]( T const &val ) { return val < partition_val; } );

        chunk_to_sort new_lower
    }
};

/*
 *
 */
int main( int argc, char **argv ) { return 0; }
