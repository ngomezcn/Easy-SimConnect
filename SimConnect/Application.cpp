#pragma once

#include "smpch.h"


struct Settable
{
private:
	const char* datumName;		 // Name of the MFS2020 simulation variable. Docs: file:///C:/MSFS%20SDK/Documentation/html/Programming_Tools/SimVars/Simulation_Variables.htm
	const char* units;			 // Specifies the units of the variable.     Docs: file:///C:/MSFS%20SDK/Documentation/html/Programming_Tools/SimVars/Simulation_Variable_Units.htm
	DATA_DEFINITION_ID defineID; // Specifies the ID of the client defined data definition.
public:

	sim::type dType = sim::type::null;

	Settable(
		HANDLE hSimConnect,
		DATA_DEFINITION_ID _defineID,
		const char* _datumName,
		const char* _units,
		sim::type _dType)
		: datumName(_datumName), defineID(_defineID), units(_units), dType(_dType)
	{
		SimConnect_AddToDataDefinition(hSimConnect, _defineID, _datumName, units); // Adding simulation variable to the SDK observer
	}

	int set()
	{
		return 0;
	}

	int get()
	{
		return 0;
	}
};


namespace sim
{
	class Aircraft
	{
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
}

int sim::Aircraft::init(HANDLE hSimConnect) {

	throttle1 = new Settable(hSimConnect, DEFINITION_THROTTLE1, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", type::DOUBLE);
	throttle2 = new Settable(hSimConnect, DEFINITION_THROTTLE2, "GENERAL ENG THROTTLE LEVER POSITION:2", "percent", type::DOUBLE);
	//indicated_altitude = new NotSettable(hSimConnect, ALTITUDE, "PLANE ALTITUDE", "feet", type::DOUBLE);
	return 0;
}


void myDispatch(SIMCONNECT_RECV* pData, DWORD cbData)
{

	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
	{
		// Retrieve simulation data and cast it to SIMCONNECT_RECV_SIMOBJECT_DATA pointer
		SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

		// Switch to find the right request 
		switch (pObjData->dwRequestID)
		{
		case REQUEST_1:
			SimResponse* pS = (SimResponse*)&pObjData->dwData;
			//sim::aircraft.pS = (SimResponse*)&pObjData->dwData;

			// print data
			/*std::cout
				<< "\rAltitude: " << sim::aircraft.pS->altitude << std::endl;
				 << "- Heading: " << pS->heading
				<< "- Speed (knots): " << pS->speed
				<< "- Vertical Speed: " << pS->vertical_speed << "  "
				<< std::flush;*/


			//sim::aircraft.pS->altitude = pS->altitude;

			break;
		}
		break;
	}
	default:
		break;
	}
}


namespace sim
{
	Aircraft aircraft;
}

bool initSimEvents() {
	HRESULT hr;


	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Client Event", NULL, NULL, NULL, NULL))) {
		std::cout << "Connected To Microsoft Flight Simulator 2020!" << std::endl;

		// Initialize sub interfaces
		//sim::aircraft.init(hSimConnect);

		// REQUESTING OUR DATA
		// #IMPORTANT: the request order must follow the declaration order o the response struct!
		// SimConnect_AddToDataDefinition takes: HANDLem enum DEFINITION_ID, const char* DATA_NAME, const char* UNIT, DATATYPE (default is FLOAT64)
		// Data input
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Indicated Altitude", "feet");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "HEADING INDICATOR", "degrees", SIMCONNECT_DATATYPE_INT32);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Airspeed Indicated", "knots", SIMCONNECT_DATATYPE_INT32);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "VERTICAL SPEED", "Feet per second", SIMCONNECT_DATATYPE_INT32);

		// System status
		hr = SimConnect_RequestSystemState(hSimConnect, REQUEST_1, "AircraftLoaded");

		// EVERY SECOND REQUEST DATA FOR DEFINITION 1 ON THE CURRENT USER AIRCRAFT (SIMCONNECT_OBJECT_ID_USER)
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

		// Process incoming SimConnect Server messages while the app is running
		while (quit == 0) {

			SIMCONNECT_RECV* pData;
			DWORD cbData;
			hr = SimConnect_GetNextDispatch(hSimConnect, &pData, &cbData);
			if (SUCCEEDED(hr))
			{
				myDispatch(pData, cbData);
			}

			//sim::aircraft.throttle1->set((rand() % 100) + 0.1);
			//sim::aircraft.throttle2->set((rand() % 50)+0.1);
				
			//std::cout << sim::aircraft.pS->altitude << std::endl;
			//sim->aircraft->throttle1->set(100);
			//sim->aircraft->throttle2->set(100);
			//sim.aircraft->throttle1.set(-16000);
			//sim.aircraft->throttle1.set(-100);
			//tc.throttlePercent = 100000;
			//hr = SimConnect_SetDataOnSimObject(hSimConnect, DEFINITION_THROTTLE, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(tc), &tc);
			Sleep(1);
		}

		// When we finish running we can close our SimmConnect handle
		hr = SimConnect_Close(hSimConnect);
		return true;
	}
	else {
		std::cout << "Failed to Connect!\n";
		return false;
	}
}


int main() {

	initSimEvents();
}


