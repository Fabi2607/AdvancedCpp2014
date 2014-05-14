/*
 * File:   main.cpp
 * Author: fabian
 *
 * Created on May 8, 2014, 2:30 PM
 */

#include <cstdlib>
#include <atomic>
#include <memory>
#include <iostream>

using namespace std;

//#define Step1
//#define Step2
#define Step3

template <typename T> class lockfree_stack {
  private:
#ifdef Step1
    struct node {
        T data;
        node *next;

        node( T const &data_ ) : data( data_ ) {}
    };
#endif
#ifndef Step1
    struct node {
        std::shared_ptr<T> data; // now we need a shared pointer
        node *next;

        node( T const &data_ ) : data( std::make_shared<T>( data_ ) ) {}
    };
#endif

    std::atomic<node *> head;

#ifdef Step3
    // we can only clean-up, if nobody else is in pop()
    std::atomic<unsigned> threads_in_pop;
    // nodes to clean up
    std::atomic<node *> to_be_deleted;

    /**
     * here we will actually delete the nodes
     * @param nodes
     */
    static void delete_nodes( node *nodes ) {
        // cleanup the remaining nodes
        while ( nodes ) {
            node *next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

    /**
     * this chains the nodes from first to last to to_be_deleted
     */
    void chain_pending_nodes( node *first, node *last ) {
        last->next = to_be_deleted;
        while ( !to_be_deleted.compare_exchange_weak( last->next, first ) )
            ;
    }

    /**
     * iterate over
     * @param nodes
     */
    void chain_pending_nodes( node *nodes ) {
        node *last = nodes;
        while ( node *const next = last->next ) {
            last = next;
        }
        chain_pending_nodes( nodes, last );
    }

    void chain_pending_node( node *n ) { chain_pending_nodes( n, n ); }

    void try_reclaim( node *old_head ) {
        //
        if ( threads_in_pop == 1 ) {
            node *nodes_to_delete = to_be_deleted.exchange( nullptr );
            if ( !--threads_in_pop ) {
                delete_nodes( nodes_to_delete );
            } else if ( nodes_to_delete ) {
                chain_pending_nodes( nodes_to_delete );
            }
            delete old_head;
        } else {
            chain_pending_node( old_head );
            --threads_in_pop;
        }
    }

#endif

  public:
    void push( T const &data ) {
        // this operation is thread-safe
        node *const new_node = new node( data );

        new_node->next = head.load();
        // after this a race condition could occur in non-threadsafe code

        // check if new_node-> next still points to head
        // if true
        //      set to new_node
        // if false
        //      update new_node->next to head
        while ( !head.compare_exchange_weak( new_node->next, new_node ) )
            ;
    }

#ifdef Step1
    // There are 2 main problems here, can you find them?
    void pop( T &result ) {
        node *old_head = head.load();

        while ( !head.compare_exchange_weak( old_head, old_head->next ) )
            ;
        result = old_head->data;
    }
#endif

#ifdef Step2
    // the memory_leak still exists -> solution garbage collector
    std::shared_ptr<T> pop() {
        node *old_head = head.load();

        while ( old_head &&
                !head.compare_exchange_weak( old_head, old_head->next ) )
            ;
        return old_head ? old_head->data : std::shared_ptr<T>();
    }
#endif

#ifdef Step3
    // the memory_leak still exists -> solution garbage collector
    std::shared_ptr<T> pop() {
        ++threads_in_pop;
        node *old_head = head.load();

        while ( old_head &&
                !head.compare_exchange_weak( old_head, old_head->next ) )
            ;
        std::shared_ptr<T> result;

        if ( old_head )
            result.swap( old_head->data );

        try_reclaim( old_head );
        return result;
    }

#endif
};

/*
 *
 */
int main( int argc, char **argv ) {
    lockfree_stack<int> i;
    int j = 1;
    i.push( j );
    cout << *( i.pop() ) << endl;
    return 0;
}
