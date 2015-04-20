Compiling instructions:
	To compile the regular (without attempting to force a race condition) version, run:
		1. make clean
		2. make
	To compile the attempted race condition version, run:
		1. make clean
		2. make race
		
Who did what?
	Joshua Campbell (jkcampbe):
		- AtomicLock class in helper.h (all methods)
		- AtomicBarrier class in helper.h (all methods)
		- HLELock class in helper.h (all methods)
		- reset method in Lanes.h
		- Rogue (for unprotected race condition compilation) in rogue.h
		- RogueFine (all except thread safe random generator) in rogue.h
		- RogueTM (all except thread safe random generator) in rogue.h, includes both HLEShoot and RTMShoot methods
		- RogueFine2 (all except thread safe random generator) in rogue.h
		- RogueTM2 (all except thread safe random generator) in rogue.h, includes both HLEShoot and RTMShoot methods
		- RogueFineCleaner (all) in rogue.h
		- Thread creation and framework for main function in Shooter.cpp
	
	Adam Penner (adpenner):
		- hasRounds method in Lanes.h
		- allDirty method in Lanes.h
		- hasViolet method in Lanes.h
		- usage of thread safe random generator in rogue.h
		- RogueCourse in rogue.h
		- RogueCourse2 in rogue.h
		- RogueCourseCleaner in rogue.h
		- RogueTMCleaner (both RTMClean and HLEClean) in rogue.h
		
