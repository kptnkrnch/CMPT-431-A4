#include <immintrin.h>
#include <iostream>
#include <vector>
#include <thread>
using namespace std;

int counter;

void ShooterAction() {

	int nretries=0;
	int retry = 1;
	int status;
	while (retry) {
		if ((status = _xbegin ()) == _XBEGIN_STARTED) {
			retry = 0;
			for (int i = 0; i < 1000000; ++i)
			{
				counter++;
				/* code */
			}


			_xend ();
		}
		else {
			retry = 1;
			nretries++;
		}
	}

	std::cout<<nretries << std::endl;
}


int main()
{

    counter = 0;

    vector<thread> ths;

    ths.push_back(std::thread(&ShooterAction));
    ths.push_back(std::thread(&ShooterAction));


    for (auto& th : ths) {

        th.join();


        std::cout << counter << std::endl; 

    }




    return 0;
}
