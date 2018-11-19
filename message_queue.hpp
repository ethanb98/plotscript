#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP
#include <queue>
#include <iostream>
#include <mutex>
#include <string>
#include <condition_variable>
#include <thread>

template<typename T>
class MsgSafeQueue
{
public: 
	void push(const T & val);

	bool empty() const;

	bool try_pop(T & pop_val);

	void wait_and_pop(T & pop_val);

private:
	std::queue<T> msg_queue;

	mutable std::mutex msg_mutex;
	
	std::condition_variable msg_condition_var;
};


#include "message_queue.tpp"

#endif