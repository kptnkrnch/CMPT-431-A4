
all: Shooter

Shooter: Shooter.o 
	g++ -mrtm -std=c++0x -Wl,--no-as-needed -pthread $< -o $@ 

Shooter.o: Shooter.cpp
	g++ -c -mrtm -std=c++0x -Wl,--no-as-needed -pthread $<

clean:
	rm -rf *o Shooter
