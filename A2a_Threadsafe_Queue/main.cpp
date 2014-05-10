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

template<typename T>
class threadsafe_queue {
private:
	struct node {
		std::shared_ptr<T> data;
		std::unique_ptr<node> next;
	};

	// mutexes for head/tail protection
    std::mutex head_mutex;
    std::mutex tail_mutex;

	// head and tail pointer
    std::unique_ptr<node> head;
    node* tail;

	// signal new data
    std::condition_variable data_cond;
public:
	// construct a dummy node
    threadsafe_queue(): head(new node),tail(head.get()) {}
    
	// copy and copy-assignment is deleted
    threadsafe_queue(const threadsafe_queue& other) = delete;
    threadsafe_queue& operator=(const threadsafe_queue& other) = delete;
	
    std::shared_ptr<T> try_pop();
    bool try_pop(T& value);
	std::shared_ptr<T> wait_and_pop();
	void wait_and_pop(T& value);
	void push(T new_value);
	void empty();
};

template<typename T>
void threadsafe_queue<T>::push(T new_value){
	std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
	
	std::unique_ptr<node> new_node(new node);

	std::unique_lock<std::mutex> lock(tail_mutex);
	tail->data = new_data;
	node* const new_tail = new_node.get();

	tail->next = std::move(new_node);
	tail = new_tail;
	lock.unlock();
	data_cond.notify_one();
}

/*
 * 
 */
int main(int argc, char** argv) {

    return 0;
}

