#include "message_queue.hpp"

template<typename T>
bool MsgSafeQueue<T>::empty() const {
	std::lock_guard<std::mutex> lock(msg_mutex);
	return msg_queue.empty();
}

template<typename T>
void MsgSafeQueue<T>::push(const T& val) {
	std::unique_lock<std::mutex> lock(msg_mutex);
	msg_queue.push(val);
	lock.unlock();
	msg_condition_var.notify_one();
}

template<typename T>
bool MsgSafeQueue<T>::try_pop(T &pop_val) {
	std::lock_guard<std::mutex> lock(msg_mutex);
	if (msg_queue.empty()) {
		return false;
	}

	pop_val = msg_queue.front();
	msg_queue.pop();
	return true;
}

template<typename T>
void MsgSafeQueue<T>::wait_and_pop(T &pop_val) {
	std::unique_lock<std::mutex> lock(msg_mutex);
	while (msg_queue.empty()) {
		msg_condition_var.wait(lock);
	}

	pop_val = msg_queue.front();
	msg_queue.pop();
}
