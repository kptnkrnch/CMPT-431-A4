#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstdlib>
#include "Lanes.h"
#include <vector>

Lanes* Gallery;
int nlanes;
using namespace std;



void ShooterAction(int rate,Color PlayerColor) {

    /**
     *  Needs synchronization. Between Red and Blue shooters.
     *  Choose a random lane and shoot.
     *  Rate: Choose a random lane every 1/rate s.
     *  PlayerColor : Red/Blue.
     */
     
     
     while(1) {
     	int lane = rand() % 5;
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
       Gallery->Print();
       cout<<Gallery->Count();

   }

}



int main(int argc, char** argv)
{

    std::vector<thread> ths;


    Gallery = new Lanes(5);
    //    std::thread RedShooterT,BlueShooterT,CleanerT,PrinterT;



    ths.push_back(std::thread(&ShooterAction,49,red));
    ths.push_back(std::thread(&ShooterAction,50,blue));
    ths.push_back(std::thread(&Cleaner));
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
