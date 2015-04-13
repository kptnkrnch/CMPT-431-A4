#ifndef ATOMICHELPER_H
#define ATOMICHELPER_H

#include <atomic>
#include <unistd.h>
#include <iostream>

class AtomicLock {
private: 
	std::atomic<bool> val;
public:	
	AtomicLock() {
		val = false;
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
};

#endif