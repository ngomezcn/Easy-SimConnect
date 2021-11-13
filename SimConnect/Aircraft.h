#include "smpch.h"
#include "Settable.h"

namespace sim
{
	class Aircraft
	{
	public:
		/*
		* Define simulation variables
		*
		* #IMPORTANT: Need to initialize the variable in init() to be able to use it
		*/
		Settable* throttle1;
		Settable* throttle2;

		//NotSettable* indicated_altitude;

		int init(HANDLE hSimConnect); // Call this before do anything with the class
	};

	// Object interface
	Aircraft aircraft;
}

int sim::Aircraft::init(HANDLE hSimConnect) {

	throttle1 = new Settable(hSimConnect, DEFINITION_THROTTLE1, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", type::DOUBLE);
	//throttle2 = new Settable(hSimConnect, DEFINITION_THROTTLE2, "GENERAL ENG THROTTLE LEVER POSITION:2", "percent", type::DOUBLE);
	//indicated_altitude = new NotSettable(hSimConnect, ALTITUDE, "PLANE ALTITUDE", "feet", type::DOUBLE);
	return 0;
}
