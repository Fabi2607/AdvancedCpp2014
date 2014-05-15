/**
 * @file   main.cpp
 * @author Fabian Kantereit
 *
 * @date May 8, 2014, 1:37 PM
 */

#include <cstdlib>
#include <memory>
#include <atomic>

using namespace std;

template<typename T>
class lock_free_queue
{
private:
	struct node
	{
		shared_ptr<T> data;
		node* next;
		
		node(): next(nullptr) {}
	};

	atomic<node*> head; // protect head and tail pointer with atomic
	atomic<node*> tail;

	// single consumer implementation
	node* pop_head()
	{
		// load head
		node* const old_head=head.load();
		// check if it is the dummy tail 
		if (old_head==tail.load()) //synchronize with push and end of queue
		{
			return nullptr; // queue is empty
		}
		// race condition in multiple consumer environment
		head.store(old_head->next); // store next;
		return old_head;
	}

public:
	// dummy node to seperate consumer/producer
	lock_free_queue() : head(new node),tail(head.load()) {}

	lock_free_queue(const lock_free_queue& other) = delete;
	lock_free_queue& operator=(const lock_free_queue& other)=delete;
	
	~lock_free_queue()
	{
		while(node* const old_head=head.load())
		{
			head.store(old_head->next);
			delete old_head;
		}
	}

	//single consumer pop
	shared_ptr<T> pop()
	{
		// obtain head
		node* old_head=pop_head();
		// check if queue is empty
		if(!old_head)
		{
			return shared_ptr<T>(); // return empty ptr
		}
		// obtain data
		shared_ptr<T> const res(old_head->data);
		delete old_head; // delete old head
		return res; // return data
	}

	// single producer
	void push(T new_value)
	{
		shared_ptr<T> new_data(make_shared<T>(new_value));
		node* p=new node;
		
		// first local copy
		node* const old_tail=tail.load();
		// then update data
		old_tail->data.swap(new_data);
		old_tail->next=p;
		// store, synchronizes with load in pop_head()
		tail.store(p);
	}
};

/**
 * 
 */
int main( int argc, char **argv ) { 
	lock_free_queue<int> qu;
	qu.push(5);
	qu.pop();
	
	return 0; 
}
