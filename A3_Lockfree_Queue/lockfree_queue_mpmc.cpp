/**
 * @file   main.cpp
 * @author Fabian Kantereit
 *
 * @date May 8, 2014, 1:37 PM
 */

#include <cstdlib>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>

using namespace std;

template <typename T> class lock_free_queue {
  private:
    struct node;

    // count references to ensure safe delete operations
    // might not work on 64 bit machines, memory alignment is very important
    // here
    struct counted_node_ptr {
        int external_count;
        node *ptr;
    };

    struct node_counter {
        // align memory to 32 bit, will most likely be lock-free
        unsigned internal_count : 30; // internal count
        unsigned external_counters
            : 2; // max 2 external counters (next, head/tail)
    };

    struct node {
        std::atomic<T *> data;              // data storage
        std::atomic<node_counter> count;    // internal node counter
        std::atomic<counted_node_ptr> next; // next node

        node() {
            // init counter
            node_counter new_count;
            new_count.internal_count = 0;
            new_count.external_counters = 2;
            count.store( new_count );

            // init next
            counted_node_ptr new_ptr;
            new_ptr.ptr = nullptr;
            new_ptr.external_count = 0;
            next.store( new_ptr );
        }

        // release currently held reference
        void release_ref() {
            // load old counter
            node_counter old_counter = count.load();
            node_counter new_counter;
            do {
                // assign new_counter
                new_counter = old_counter;
                // decrease internal count
                --new_counter.internal_count;
            } while (
                !count.compare_exchange_strong( old_counter, new_counter ) );
            // check if old_counter still equals count, otherwise loop again

            if ( !new_counter.internal_count &&
                 !new_counter.external_counters ) {
                delete this; // all references released, delete object
            }
        }
    };

    // increase external count
    static void increase_external_count( atomic<counted_node_ptr> &counter,
                                         counted_node_ptr &old_counter ) {
        // local copy
        counted_node_ptr new_counter;
        do {
            // make local copy
            new_counter = old_counter;
            // increase external count
            ++new_counter.external_count;
        } while (
            !counter.compare_exchange_strong( old_counter, new_counter ) );
        // check if old_counter is still the same, if yes atomic exchange,
        // otherwise
        // loop again

        // new_counter now contains the old external count
        old_counter.external_count = new_counter.external_count;
    }

    // free an external counter
    static void free_external_counter( counted_node_ptr &old_node_ptr ) {
        // obtain ptr from old node
        node *const ptr = old_node_ptr.ptr;

        // external count, that is added to internal count on releasing
        // the external counter
        int const count_increase = old_node_ptr.external_count - 2;

        // make local copy of old counter
        node_counter old_counter = ptr->count.load();
        node_counter new_counter;

        do {
            // assign new counter
            new_counter = old_counter;
            // update external counters
            --new_counter.external_counters;
            // update internal count
            new_counter.internal_count += count_increase;
        } while (
            !ptr->count.compare_exchange_strong( old_counter, new_counter ) );
        // check if old_counter is still ptr->count

        // if all references are released, delete ptr
        if ( !new_counter.internal_count && !new_counter.external_counters ) {
            delete ptr;
        }
    }

    // reference counted nodes
    atomic<counted_node_ptr> head; // protect head and tail pointer with atomic
    atomic<counted_node_ptr> tail;

  public:
    lock_free_queue() {
        // construct dummy node
        counted_node_ptr new_next;
        new_next.ptr = new node;
        new_next.external_count = 1;
        tail = new_next;
        head = tail.load();
    }

    lock_free_queue( const lock_free_queue &other ) = delete;
    lock_free_queue &operator=( const lock_free_queue &other ) = delete;

    ~lock_free_queue() {
        while ( pop() )
            ; // free all nodes
        delete head.load().ptr;
    }

    // multiconsumer pop
    unique_ptr<T> pop() {
        // obtain old_head
        counted_node_ptr old_head = head.load();
        while ( true ) {
            // increase external_reference count
            increase_external_count( head, old_head );

            // get old ptr
            node *const ptr = old_head.ptr;

            // check if end is reached
            if ( ptr == tail.load().ptr ) {
                ptr->release_ref();     // release reference
                return unique_ptr<T>(); // return empty pointer
            }

            // try to claim data
            if ( head.compare_exchange_strong( old_head, ptr->next ) ) {
                // here head has been claimed
                T *const res = ptr->data.exchange( nullptr );

                // free external counter ( head )
                free_external_counter( old_head );
                return unique_ptr<T>( res );
            }
            // release the reference
            ptr->release_ref();
        }
    }

    // multiproducer push
    void push( T new_value ) {
        // unique ptr to new data
        unique_ptr<T> new_data( new T( new_value ) );

        // ptr to new_next
        counted_node_ptr new_next;

        // initialise with new node
        new_next.ptr = new node;
        // one external count
        new_next.external_count = 1;

        // load old tail
        counted_node_ptr old_tail = tail.load();

        while ( true ) {
            // increase external count on old tail
            increase_external_count( tail, old_tail );

            // initialise old_data with null pointer
            T *old_data = nullptr;
            if ( old_tail.ptr->data.compare_exchange_strong(
                    old_data, new_data.get() ) ) { // check if tail has been
                                                   // updated by another thread
                old_tail.ptr->next =
                    new_next; // update old_tail ptr to new node

                old_tail =
                    tail.exchange( new_next ); // exchange old tail with new

                free_external_counter(
                    old_tail ); // remove external count from old

                new_data.release(); // release new data
                break; // successfull
            }
            old_tail.ptr->release_ref(); // release reference
        }
    }
};

/**
 *
 */
int main( int argc, char **argv ) {
    lock_free_queue<int> qu;
    qu.push( 5 );
    cout << *qu.pop() << endl;
    return 0;
}
