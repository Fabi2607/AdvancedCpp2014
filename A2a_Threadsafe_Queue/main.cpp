/**
 * @file   main.cpp
 * @author fabian
 *
 * @date May 8, 2014, 9:20 AM
 */

#include <cstdlib>
#include <memory>
#include <condition_variable>
#include <iostream>

using namespace std;

template <typename T> class threadsafe_queue {
  private:
    struct node {
        shared_ptr<T> data;
        unique_ptr<node> next;
    };

    // mutexes for head/tail protection
    mutex head_mutex;
    mutex tail_mutex;

    // head and tail pointer
    unique_ptr<node> head;
    node *tail;

    // signal new data
    condition_variable data_cond;

	// helper functions
	// locks the tail and returns it
	node* get_tail() {
		lock_guard<mutex> tail_lock{tail_mutex};
		return tail;
	}

	unique_ptr<node> pop_head() {
		unique_ptr<node> old_head = move(head);
		head = move(old_head->next);
		return old_head;
	}

	unique_lock<mutex> wait_for_data() {
		unique_lock<mutex> head_lock(head_mutex);
		data_cond.wait(head_lock,[&]{return head.get()!=get_tail();});
		return move(head_lock);
	}

	unique_ptr<node> wait_pop_head() {
		unique_lock<mutex> head_lock(wait_for_data());
		return pop_head();
	}

	unique_ptr<node> wait_pop_head(T& value) {
		std::unique_lock<mutex> head_lock(wait_for_data());
		value=move(*head->data);
		return pop_head();
	}

	unique_ptr<node> try_pop_head() {
		lock_guard<mutex> head_lock(head_mutex);
		if(head.get()==get_tail()) {
			return unique_ptr<node>();
		}
		return pop_head();
	}

	unique_ptr<node> try_pop_head(T& value){
		lock_guard<mutex> head_lock(head_mutex);
		if(head.get()==get_tail()) {
			return unique_ptr<node>();
		}
		value = move(*head->data);
		return pop_head();
	}

  public:
    // construct a dummy node
    threadsafe_queue() : head( new node ), tail( head.get() ) {}

    // copy and copy-assignment is deleted
    threadsafe_queue( const threadsafe_queue &other ) = delete;
    threadsafe_queue &operator=( const threadsafe_queue &other ) = delete;

    shared_ptr<T> try_pop();
    bool try_pop( T &value );
    shared_ptr<T> wait_and_pop();
    void wait_and_pop( T &value );
    void push( T new_value );
    bool empty();
};

template <typename T> void threadsafe_queue<T>::push( T new_value ) {
    shared_ptr<T> new_data( make_shared<T>( move( new_value ) ) );

    unique_ptr<node> new_node( new node );

    unique_lock<std::mutex> lock( tail_mutex );
    tail->data = new_data;
    node *const new_tail = new_node.get();

    tail->next = move( new_node );
    tail = new_tail;
    lock.unlock();

    data_cond.notify_one();
}

template <typename T> shared_ptr<T> threadsafe_queue<T>::wait_and_pop( ){
	unique_ptr<node> const old_head=wait_pop_head();
	return old_head->data;
}

template <typename T> void threadsafe_queue<T>::wait_and_pop(T& value) {
	unique_ptr<node> const old_head=wait_pop_head(value);
}

template <typename T> shared_ptr<T> threadsafe_queue<T>::try_pop() {
	unique_ptr<node> old_head=try_pop_head();
	return old_head?old_head->data : shared_ptr<T>();
}
/*
 *
 */
int main( int argc, char **argv ) {

    threadsafe_queue<int> queue;

    queue.push( 5 );


	cout << *queue.wait_and_pop()<< endl;
    return 0;
}
