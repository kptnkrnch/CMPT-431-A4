#ifndef ROGUE_H

#define ROGUE_H

#include <mutex>
#include <unistd.h>
#include "Lanes.h"
#include "helper.h"

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

/*void barrier (int expect)
{
    static int arrived = 0;

    pthread_mutex_lock (&mut);	//lock

    arrived++;
    if (arrived < expect)
        pthread_cond_wait (&cond, &mut);
    else {
        arrived = 0;		// reset the barrier before broadcast is important
        pthread_cond_broadcast (&cond);
    }

    pthread_mutex_unlock (&mut);	//unlock
}*/

class RogueCoarse {
public:
	/* data */
	Color bullet; // The bullet color to paint the lane
	int shotRate; // Rate/s required to shoot the lanes
	int success; // Rate/s of lanes shot by ROGUE   
	std::mutex * lock;
	AtomicLock * l;

	RogueCoarse(Color color, int rate, AtomicLock * _l) {
		bullet = color;
		shotRate = rate;
		success = 0;
		l = _l;
	}
	
	void shoot() {
		while(1) {
			int lane = rand() % LANE_COUNT;
			usleep(1000000/shotRate);
			Color check = Gallery->Get(lane);
			if (check == white) {
				//lock->lock();
				l->lock();
				check = Gallery->Get(lane);
				if (check == white) {
					//usleep(10000);
					check = Gallery->Set(lane, bullet);
					if (check == bullet) {
						success++;
					}
				}
				l->unlock();
				//lock->unlock();
			}
		}
	}
};

class RogueFine {
public:
	RogueFine(int color, int rate);

	/* data */
	Color Bullet; // The bullet color to paint the lane
	int ShotRate; // Rate/s required to shoot the lanes
	int Success; // Rate/s of lanes shot by ROGUE
};

class RogueTM {
public:
	RogueTM(int color, int rate);

	/* data */
	Color Bullet; // The bullet color to paint the lane
	int ShotRate; // Rate/s required to shoot the lanes
	int Success; // Rate/s of lanes shot by ROGUE
};

class RogueCoarse2 {
public:
	RogueCoarse2(int color, int rate);

	/* data */
	Color Bullet; // The bullet color to paint the lane
	int ShotRate; // Rate/s required to shoot the lanes
	int Success; // Rate/s of lanes shot by ROGUE
};

class RogueFine2 {
public:
	RogueFine2(int color, int rate);

	/* data */
	Color Bullet; // The bullet color to paint the lane
	int ShotRate; // Rate/s required to shoot the lanes
	int Success; // Rate/s of lanes shot by ROGUE
};

class RogueTM2 {
public:
	RogueTM2(int color, int rate);

	/* data */
	Color Bullet; // The bullet color to paint the lane
	int ShotRate; // Rate/s required to shoot the lanes
	int Success; // Rate/s of lanes shot by ROGUE
};
class RogueCoarseCleaner {
public:
	AtomicLock * lock;
	
	RogueCoarseCleaner(AtomicLock * _lock) {
		lock = _lock;
	}

	void clean() {
		while(1) {
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
				lock->lock();
				/*for (int i = 0; i < LANE_COUNT; i++) {
					Gallery->Set(i, white);
				}*/
				Gallery->Clear();
				lock->unlock();
			}
		}
	}
};
/*
class RogueFineCleaner {
};

class RogueTMCleaner {
};
*/
#endif