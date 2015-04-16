#ifndef ROGUE_H

#define ROGUE_H

#include <mutex>
#include <unistd.h>
#include <iostream>
#include "Lanes.h"
#include "helper.h"
#include <random>

#define MAX_RETRIES 100

extern Lanes* Gallery;

/*class Rogue
{
public:
	Rogue(int color, int rate);
	~Rogue();

	// data 
	Color Bullet; // The bullet color to paint the lane
	int ShotRate; // Rate/s required to shoot the lanes
	int Success; // Rate/s of lanes shot by ROGUE   
};*/

class RogueCoarse {
public:
	/* data */
	Color bullet; // The bullet color to paint the lane
	int shotRate; // Rate/s required to shoot the lanes
	int success; // Rate/s of lanes shot by ROGUE   
	AtomicLock * lock;
	AtomicBarrier * barrier;



	RogueCoarse(Color color, int rate, AtomicLock * _lock, AtomicBarrier * _barrier) {
		bullet = color;
		shotRate = rate;
		lock = _lock;
		barrier = _barrier;
		success = 0; //not really used
	}
	
	void shoot() {

		//locally allocate and setup random number generator
		thread_local std::mt19937 gen((std::random_device())());
		thread_local std::uniform_int_distribution<int> dist(0,LANE_COUNT-1);

		if(barrier != 0 ) {
			//wait until every thread is here
			barrier->barrier();
		}
		else {
			std::cerr << "ERROR: Course barrier was not set!" << std::endl;
		}

		if(lock != 0) {
			//lock properly initialized, moving on

			while(Gallery->hasRounds()) {

				int lane = dist(gen);
				usleep(1000000/shotRate);

				Color check = Gallery->Get(lane);

				if(check == white) {
					lock->lock();

					check = Gallery->Get(lane);

					if(check == white) {
						//still good so color it
						check = Gallery->Set(lane, bullet);

						if(check == bullet) {
							success++;
						}
					}

					lock->unlock();
				}
			}
		}
		else {
			std::cerr << "ERROR: Course lock was not set!" << std::endl;
		}
	}
};

class RogueFine {
public:
	/* data */
	Color bullet; // The bullet color to paint the lane
	int shotRate; // Rate/s required to shoot the lanes
	int success; // Rate/s of lanes shot by ROGUE
	AtomicLock * locks;
	AtomicBarrier * barrier;
	
	RogueFine(Color color, int rate, AtomicLock * _locks, AtomicBarrier * _barrier) {
		bullet = color;
		shotRate = rate;
		success = 0;
		locks = _locks;
		barrier = _barrier;
	}
	
	void shoot() {
		if (barrier != 0) {
			barrier->barrier();
		}
		if (locks != 0) {
			while(Gallery->hasRounds()) {
				int lane = rand() % LANE_COUNT;
				usleep(1000000/shotRate);
				Color check = Gallery->Get(lane);
				if (check == white) {
					locks[lane].lock();
					check = Gallery->Get(lane);
					if (check == white) {
						check = Gallery->Set(lane, bullet);
						if (check == bullet) {
							success++;
						}
					}
					locks[lane].unlock();
				}
			}
		} else {
			std::cerr << "ERROR: the locks were not set!" << std::endl;
		}
	}
};

class RogueTM {
public:
	/* data */
	Color bullet; // The bullet color to paint the lane
	int shotRate; // Rate/s required to shoot the lanes
	int success; // Rate/s of lanes shot by ROGUE
	AtomicLock * fallback_lock;
	HLELock * hle_lock;
	AtomicBarrier * barrier;
	
	RogueTM(Color color, int rate, HLELock * _hlelock, AtomicLock * _atomiclock, AtomicBarrier * _barrier) {
		bullet = color;
		shotRate = rate;
		success = 0;
		fallback_lock = _atomiclock;
		hle_lock = _hlelock;
		barrier = _barrier;
	}
	
	void RTMShoot() {
		if (barrier != 0) {
			barrier->barrier();
		}
		if (fallback_lock != 0) {
			while(Gallery->hasRounds()) {
				int lane = rand() % LANE_COUNT;
				usleep(1000000/shotRate);
				Color check = Gallery->Get(lane);
				if (check == white) {
					int nretries = 0;
					bool retry = false;
					int status = 0;
					do {
						retry = false;
						if ((status = _xbegin ()) == _XBEGIN_STARTED) {
							check = Gallery->Get(lane);
							if (check == white) {
								check = Gallery->Set(lane, bullet);
								if (check == bullet) {
									success++;
								}
							}
							_xend ();
						} else {
							retry = true;
							nretries++;
						}
					} while (retry == true && nretries < MAX_RETRIES);
					if (nretries == MAX_RETRIES) {
						std::cerr << "ERROR: using fallback shoot" << std::endl;
						fallback_lock->lock();
						check = Gallery->Get(lane);
						if (check == white) {
							check = Gallery->Set(lane, bullet);
							if (check == bullet) {
								success++;
							}
						}
						fallback_lock->unlock();
					}
				}
			}
		} else {
			std::cerr << "ERROR: the locks were not set!" << std::endl;
		}
	}
	
	void HLEShoot() {
		if (barrier != 0) {
			barrier->barrier();
		}
		if (hle_lock != 0) {
			while(Gallery->hasRounds()) {
				int lane = rand() % LANE_COUNT;
				usleep(1000000/shotRate);
				Color check = Gallery->Get(lane);
				if (check == white) {
					hle_lock->lock();
					check = Gallery->Get(lane);
					if (check == white) {
						check = Gallery->Set(lane, bullet);
						if (check == bullet) {
							success++;
						}
					}
					hle_lock->unlock();
				}
			}
		} else {
			std::cerr << "ERROR: HLE lock was not set!" << std::endl;
		}
	}
};

class RogueCoarse2 {
public:
	/* data */
	Color bullet; // The bullet color to paint the lane
	int shotRate; // Rate/s required to shoot the lanes
	int success; // Rate/s of lanes shot by ROGUE   
	AtomicLock * lock;
	AtomicBarrier * barrier;

	RogueCoarse2(Color color, int rate, AtomicLock * _lock, AtomicBarrier * _barrier) {
		bullet = color;
		shotRate = rate;
		lock = _lock;
		barrier = _barrier;
		success = 0;
	}
	
	void shoot() {

		//locally allocate and setup random number generator
		thread_local std::mt19937 gen((std::random_device())());
		thread_local std::uniform_int_distribution<int> dist(0,LANE_COUNT-1);

		if(barrier != 0 ) {
			//wait until every thread is here
			barrier->barrier();
		}
		else {
			std::cerr << "ERROR: Course barrier was not set!" << std::endl;
		}

		if(lock != 0) {
			//lock properly initialized, moving on

			while(Gallery->hasRounds()) {

				int lane = dist(gen);
				int lane2 = dist(gen);

				while(lane == lane2) {
					lane2 = dist(gen);
				}

				if(lane > lane2) {
					int temp = lane2;
					lane2 = lane;
					lane = temp;
				}

				usleep(1000000/shotRate);

				Color check = Gallery->Get(lane);
				Color check2 = Gallery->Get(lane2);

				if(check == white && check2 == white) {
					lock->lock();

					check = Gallery->Get(lane);
					check2 = Gallery->Get(lane2);

					if(check == white && check2 == white) {
						//still good so color it
						check = Gallery->Set(lane, bullet);
						check2 = Gallery->Set(lane2, bullet);

						if(check == bullet) {
							success++;
						}
						if(check2 == bullet) {
							success++;
						}
					}

					lock->unlock();
				}
			}
		}
		else {
			std::cerr << "ERROR: Course lock was not set!" << std::endl;
		}
	}
};

class RogueFine2 {
public:
	/* data */
	Color bullet; // The bullet color to paint the lane
	int shotRate; // Rate/s required to shoot the lanes
	int success; // Rate/s of lanes shot by ROGUE
	AtomicLock * locks;
	AtomicBarrier * barrier;
	
	RogueFine2(Color color, int rate, AtomicLock * _locks, AtomicBarrier * _barrier) {
		bullet = color;
		shotRate = rate;
		success = 0;
		locks = _locks;
		barrier = _barrier;
	}
	
	void shoot() {
		if (barrier != 0) {
			barrier->barrier();
		}
		if (locks != 0) {
			while(Gallery->hasRounds()) {
				int lane1 = rand() % LANE_COUNT;
				int lane2 = rand() % LANE_COUNT;
				while (lane2 == lane1) {
					lane2 = rand() % LANE_COUNT;
				}
				if (lane1 > lane2) {
					int temp = lane2;
					lane2 = lane1;
					lane1 = temp;
				}
				usleep(1000000/shotRate);
				Color check1 = Gallery->Get(lane1);
				Color check2 = Gallery->Get(lane2);
				if (check1 == white && check2 == white) {
					locks[lane1].lock();
					locks[lane2].lock();
					check1 = Gallery->Get(lane1);
					check2 = Gallery->Get(lane2);
					if (check1 == white && check2 == white) {
						check1 = Gallery->Set(lane1, bullet);
						check2 = Gallery->Set(lane2, bullet);
						if (check1 == bullet) {
							success++;
						}
						if (check2 == bullet) {
							success++;
						}
					}
					locks[lane1].unlock();
					locks[lane2].unlock();
				}
			}
		} else {
			std::cerr << "ERROR: the locks were not set!" << std::endl;
		}
	}
};

class RogueTM2 {
public:
	/* data */
	Color bullet; // The bullet color to paint the lane
	int shotRate; // Rate/s required to shoot the lanes
	int success; // Rate/s of lanes shot by ROGUE
	AtomicLock * fallback_lock;
	HLELock * hle_lock;
	AtomicBarrier * barrier;
	
	RogueTM2(Color color, int rate, HLELock * _hlelock, AtomicLock * _atomiclock, AtomicBarrier * _barrier) {
		bullet = color;
		shotRate = rate;
		success = 0;
		fallback_lock = _atomiclock;
		hle_lock = _hlelock;
		barrier = _barrier;
	}
	
	void RTMShoot() {
		if (barrier != 0) {
			barrier->barrier();
		}
		if (fallback_lock != 0) {
			while(Gallery->hasRounds()) {
				int lane1 = rand() % LANE_COUNT;
				int lane2 = rand() % LANE_COUNT;
				while (lane2 == lane1) {
					lane2 = rand() % LANE_COUNT;
				}
				if (lane1 > lane2) {
					int temp = lane2;
					lane2 = lane1;
					lane1 = temp;
				}
				usleep(1000000/shotRate);
				Color check1 = Gallery->Get(lane1);
				Color check2 = Gallery->Get(lane2);
				if (check1 == white && check2 == white) {
					int nretries = 0;
					bool retry = false;
					int status = 0;
					do {
						retry = false;
						if ((status = _xbegin ()) == _XBEGIN_STARTED) {
							check1 = Gallery->Get(lane1);
							check2 = Gallery->Get(lane2);
							if (check1 == white && check2 == white) {
								check1 = Gallery->Set(lane1, bullet);
								check2 = Gallery->Set(lane2, bullet);
								if (check1 == bullet) {
									success++;
								}
								if (check2 == bullet) {
									success++;
								}
							}
							_xend ();
						} else {
							retry = true;
							nretries++;
						}
					} while (retry == true && nretries < MAX_RETRIES);
					if (nretries == MAX_RETRIES) {
						std::cerr << "ERROR: using fallback shoot" << std::endl;
						fallback_lock->lock();
						check1 = Gallery->Get(lane1);
						check2 = Gallery->Get(lane2);
						if (check1 == white && check2 == white) {
							check1 = Gallery->Set(lane1, bullet);
							check2 = Gallery->Set(lane2, bullet);
							if (check1 == bullet) {
								success++;
							}
							if (check2 == bullet) {
								success++;
							}
						}
						fallback_lock->unlock();
					}
				}
			}
		} else {
			std::cerr << "ERROR: the locks were not set!" << std::endl;
		}
	}
	
	void HLEShoot() {
		if (barrier != 0) {
			barrier->barrier();
		}
		if (hle_lock != 0) {
			while(Gallery->hasRounds()) {
				int lane1 = rand() % LANE_COUNT;
				int lane2 = rand() % LANE_COUNT;
				while (lane2 == lane1) {
					lane2 = rand() % LANE_COUNT;
				}
				if (lane1 > lane2) {
					int temp = lane2;
					lane2 = lane1;
					lane1 = temp;
				}
				usleep(1000000/shotRate);
				Color check1 = Gallery->Get(lane1);
				Color check2 = Gallery->Get(lane2);
				if (check1 == white && check2 == white) {
					hle_lock->lock();
					check1 = Gallery->Get(lane1);
					check2 = Gallery->Get(lane2);
					if (check1 == white && check2 == white) {
						check1 = Gallery->Set(lane1, bullet);
						check2 = Gallery->Set(lane2, bullet);
						if (check1 == bullet) {
							success++;
						}
						if (check2 == bullet) {
							success++;
						}
					}
					hle_lock->unlock();
				}
			}
		} else {
			std::cerr << "ERROR: HLE lock was not set!" << std::endl;
		}
	}
};
class RogueCoarseCleaner {
public:
	AtomicLock * lock;
	
	RogueCoarseCleaner(AtomicLock * _lock) {
		lock = _lock;
	}

	void clean() {

		if(lock != 0) {
			while(Gallery->hasRounds()) {

				if(Gallery->hasViolet()) {
					exit(2);
				}

				if (Gallery->allDirty()) {
					lock->lock();

					Gallery->Print();
					//more stats go here

					Gallery->Clear();
					
					lock->unlock();
				}
			}
		}
		else {
			std::cerr << "ERROR: Course cleaner lock was not set!" << std::endl;
		}
		
	}
};

class RogueFineCleaner {
public:
	AtomicLock * locks;
	
	RogueFineCleaner(AtomicLock * _locks) {
		locks = _locks;
	}

	void clean() {
		if (locks != 0) {
			while(Gallery->hasRounds()) {
				int dirty_lanes = 0;
				for (int i = 0; i < LANE_COUNT; i++) {
					Color temp = Gallery->Get(i);
					if (temp == violet) {
						exit(2);
					}
					if (temp != white) {
						dirty_lanes++;
					}
				}
				if (dirty_lanes == LANE_COUNT) {
					for (int i = 0; i < LANE_COUNT; i++) {
						locks[i].lock();
					}
					Gallery->Print();
					//more stats go here
					Gallery->Clear();
					for (int i = 0; i < LANE_COUNT; i++) {
						locks[i].unlock();
					}
				}
			}
		} else {
			std::cerr << "ERROR: the locks were not set!" << std::endl;
		}
	}
};

class RogueTMCleaner {
public:
	AtomicLock * fallback_lock;
	HLELock * hle_lock;
	
	RogueTMCleaner(HLELock * _hlelock, AtomicLock * _atomiclock) {
		fallback_lock = _atomiclock;
		hle_lock = _hlelock;
	}
	
	void RTMClean() {

		if(fallback_lock != 0) {

			while(Gallery->hasRounds()) {
				if(Gallery->hasViolet()) {
					exit(2);
				}

				if(Gallery->allDirty()) {

					bool retry = false;
					int nretries = 0;

					do {
						retry = false;
						if(_xbegin() == _XBEGIN_STARTED) {
							Gallery->Clear();

							//TODO find way to print here as RTM doesnt work with printing...
							_xend();
						}
						else {
							retry = true;
							nretries++;
						}
					} while(retry && nretries < MAX_RETRIES);

					if(nretries >= MAX_RETRIES) {
						fallback_lock->lock();
						Gallery->Print();
						//more stats go here
						Gallery->Clear();
						fallback_lock->unlock();
					}
				}
			}
		}
		else {
			std::cerr << "ERROR: RTM cleaner fallback lock was not set!" << std::endl;
		}
		
	}
	
	void HLEClean() {
		if(hle_lock != 0) {
			while(Gallery->hasRounds()) {

				if(Gallery->hasViolet()) {
					exit(2);
				}

				if (Gallery->allDirty()) {
					hle_lock->lock();

					Gallery->Print();
					//more stats go here
					Gallery->Clear();
					
					hle_lock->unlock();
				}
			}
		}
		else {
			std::cerr << "ERROR: HLE cleaner lock was not set!" << std::endl;
		}
	}
};

#endif