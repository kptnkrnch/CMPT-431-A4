#ifndef ATOMICHELPER_H
#define ATOMICHELPER_H

#include <atomic>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <iostream>

class AtomicLock {
private: 
	std::atomic<bool> val;
	bool waiting;
public:	
	AtomicLock() {
		val = false;
		waiting = false;
	}
	
	void wait() {
		waiting = true;
		while(waiting);
	}
	
	void release() {
		waiting = false;
	}
	
	void lock() {
		bool temp = false;
		int backoff = 10;
		while (!val.compare_exchange_weak(temp, true, std::memory_order_acquire)) {
			temp = false;
			//usleep(backoff);
			//backoff *= 2;
		}
	}
	
	void unlock() {
		val.store(false , std::memory_order_release);
	}
};


class AtomicBarrier {
private:
	AtomicLock * lock;
	std::condition_variable * block;
	std::mutex block_mutex;
	int expected;
	int arrived;
public:
	AtomicBarrier(int _expected) {
		expected = _expected;
		lock = new AtomicLock();
		block = new std::condition_variable();
		//block_mutex = new std::mutex();
		arrived = 0;
	}
	
	~AtomicBarrier() {
		delete lock;
		delete block;
		//delete block_mutex;
	}
	
	void barrier() {
		lock->lock();
		arrived++;
		if (arrived < expected) {
			lock->unlock();
			std::unique_lock<std::mutex> lk(block_mutex);
			//lock->wait();
			block->wait(lk);
			//atomic_thread_fence(std::memory_order_acquire);
		} else {
			//atomic_thread_fence(std::memory_order_release);
			arrived = 0;
			block->notify_all();
			//lock->release();
		}
		lock->unlock();
	}
};

#endif