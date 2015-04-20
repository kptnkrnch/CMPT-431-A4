
all: Shooter

Shooter: Shooter.o 
	g++ -mrtm -std=c++11 -Wl,--no-as-needed -pthread $< -o $@ 

Shooter.o: Shooter.cpp
	g++ -c -mrtm -std=c++11 -Wl,--no-as-needed -pthread $<

clean:
	rm -rf *o Shooter

race: Shooter.cpp
	g++ -c -mrtm -std=c++11 -Wl,--no-as-needed -pthread -D TRYRACE $<
	g++ -mrtm -std=c++11 -Wl,--no-as-needed -pthread -D TRYRACE $< -o Shooter