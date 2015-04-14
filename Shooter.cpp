#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstdlib>
#include "Lanes.h"
#include "Rogue.h"
#include "helper.h"
#include <vector>
#include <mutex>

Lanes* Gallery;
int nlanes;
using namespace std;

mutex master_lock;
mutex * lane_lock;

void ShooterAction(int rate,Color PlayerColor) {

    /**
     *  Needs synchronization. Between Red and Blue shooters.
     *  Choose a random lane and shoot.
     *  Rate: Choose a random lane every 1/rate s.
     *  PlayerColor : Red/Blue.
     */
     
     
     while(1) {
     	int lane = rand() % LANE_COUNT;
     	usleep(1000000/rate);
		Color check = Gallery->Get(lane);
		if (check == white) {
			Gallery->Set(lane, PlayerColor);
		}
     }
     
     //Gallery->Set(0,red);
}


void Cleaner() {

    /**
     *  Cleans up lanes. Needs to synchronize with shooter.
     *  Use a monitor
     *  Should be in action only when all lanes are shot.
     *  Once all lanes are shot. Cleaner starts up.
     *  Once cleaner starts up shooters wait for cleaner to finish.
     */
	
}


void Printer(int rate) {

    /**
     *  NOT TRANSACTION SAFE; cannot be called inside a transaction. Possible conflict with other Txs Performs I/O
     *  Print lanes periodically rate/s.
     *  If it doesn't synchronize then possible mutual inconsistency between adjacent lanes.
     *  Not a particular concern if we don't shoot two lanes at the same time.
     *
     */

   while(1)
   {
       sleep(1);
	   //usleep(1000000/200000);
       Gallery->Print();
       cout<<Gallery->Count();

   }

}



int main(int argc, char** argv)
{

    std::vector<thread> ths;

	AtomicLock * l = new AtomicLock();
	AtomicBarrier barrier(2);
    Gallery = new Lanes(LANE_COUNT);
    //    std::thread RedShooterT,BlueShooterT,CleanerT,PrinterT;

	RogueCoarse red_shooter(red, 1000000, l, &barrier); 
	RogueCoarse blue_shooter(blue, 1000000, l, &barrier);
	/*RogueCoarse red_shooter2(red, 1000000, &master_lock); 
	RogueCoarse blue_shooter2(blue, 1000000, &master_lock);
	RogueCoarse red_shooter3(red, 1000000, &master_lock); 
	RogueCoarse blue_shooter3(blue, 1000000, &master_lock);*/
	RogueCoarseCleaner cleaner(l);

    //ths.push_back(std::thread(&ShooterAction,49,red));
    //ths.push_back(std::thread(&ShooterAction,50,blue));
    //ths.push_back(std::thread(&Cleaner));
	
	ths.push_back(std::thread(&RogueCoarse::shoot, red_shooter));
    ths.push_back(std::thread(&RogueCoarse::shoot, blue_shooter));
	/*ths.push_back(std::thread(&RogueCoarse::shoot, red_shooter2));
    ths.push_back(std::thread(&RogueCoarse::shoot, blue_shooter2));
	ths.push_back(std::thread(&RogueCoarse::shoot, red_shooter3));
    ths.push_back(std::thread(&RogueCoarse::shoot, blue_shooter3));*/
	ths.push_back(std::thread(&RogueCoarseCleaner::clean, cleaner));

	ths.push_back(std::thread(&Printer,5));

    // Join with threads
    //    RedShooterT.join();
    //  BlueShooterT.join();
    //  CleanerT.join();
    // PrinterT.join();


    for (auto& th : ths) {

        th.join();

    }


    return 0;
}
