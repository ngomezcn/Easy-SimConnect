#include <iostream>
#include <Windows.h>

#include "SimConnect.h"



int quit = 0; // 0 = Keep Running - 1 = Exit app
HANDLE hSimConnect = NULL; // SimConnect Handle

// To make a data request we need:
static enum DATA_DEFINITION_ID {
	DEFINITION_1,
	DEFINITION_THROTTLE1,
	DEFINITION_THROTTLE2,
	ALTITUDE,

};

enum DATA_REQUEST_ID {
	REQUEST_1,
	REQUEST_2,
	// ... (we can use different request to organize our response data)
};

// This structure will hold the data
struct SimResponse {
	double altitude = 0;
	int32_t heading;
	int32_t speed;
	int32_t vertical_speed;
};


struct structThrottleControl
{
	double throttlePercent;
};

structThrottleControl tc;


/*
// Event callback
void CALLBACK MyDispatchProc1(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext) {
	switch (pData->dwID)
	{
		// CASE SIMULATION DATA RECIVE EVENT

	case SIMCONNECT_RECV_ID_SYSTEM_STATE:
	{
		SIMCONNECT_RECV_SYSTEM_STATE* pSysData = (SIMCONNECT_RECV_SYSTEM_STATE*)pData;
		std::cout << pSysData->szString << std::endl;

	}
	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
	{
		// Retrieve simulation data and cast it to SIMCONNECT_RECV_SIMOBJECT_DATA pointer
		SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

		// Switch to find the right request
		switch (pObjData->dwRequestID)
		{
		case REQUEST_1:
			SimResponse* pS = (SimResponse*)&pObjData->dwData;

			// print data
			/*std::cout
				<< "\rAltitude: " << pS->altitude
				<< "- Heading: " << pS->heading
				<< "- Speed (knots): " << pS->speed
				<< "- Vertical Speed: " << pS->vertical_speed << "  "
				<< std::flush;

			break;
		}

		break;
	}
	// Case simulation quit event
	case SIMCONNECT_RECV_ID_QUIT:
	{
		quit = 1;
		break;
	}
	// Other events
	default:
		break;
	}
}*/

namespace sim
{
	enum class type
	{
		null = -1,
		INT ,
		DOUBLE,
		BOOL,
	};
}


struct Data
{
	void* pValue = 0;

	void insert(int var)
	{
		pValue = new int(var);
	}

	void insert(double var)
	{
		pValue = new double(var);
	}

};



class SimVar
{
private:
	const char* datumName;
	DATA_DEFINITION_ID defineID;
	const char* units;

public:

	Data data;
	sim::type dType = sim::type::null;

	SimVar(
		HANDLE hSimConnect,
		DATA_DEFINITION_ID _defineID,
		const char* _datumName,
		const char* _units,
		sim::type _dType)
		: datumName(_datumName), defineID(_defineID), units(_units), dType(_dType)
	{

		// Add new sim variable to observer
		SimConnect_AddToDataDefinition(hSimConnect, _defineID, _datumName, units);
	}

	void set(int value)
	{
		SimConnect_SetDataOnSimObject(hSimConnect, defineID, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(value), &value);
	}	

	void set(double value)
	{
		SimConnect_SetDataOnSimObject(hSimConnect, defineID, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(value), &value);
	}

	void get()
	{
		data.pValue = new double(10.2);
		std::cout << *((double*)data.pValue);
	}
};



struct Aircraft
{
public:

	SimVar* throttle1;
	SimVar* throttle2;
	SimVar* altitude;

	SimResponse* pS;

	int init(HANDLE hSimConnect)
	{

		pS = new SimResponse();
		std::cout << "TEEEST";
		pS->altitude = 99.9;

		std::cout << pS->altitude;
		using namespace sim;
		throttle1 = new SimVar(hSimConnect, DEFINITION_THROTTLE1, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", type::DOUBLE);
		throttle2 = new SimVar(hSimConnect, DEFINITION_THROTTLE2, "GENERAL ENG THROTTLE LEVER POSITION:2", "percent", type::DOUBLE);
		altitude  = new SimVar(hSimConnect, ALTITUDE, "PLANE ALTITUDE", "feet", type::DOUBLE);

		return 0;
	}
};

namespace sim
{
	Aircraft aircraft;
}

double* prueba;
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
			//SimResponse* pS = (SimResponse*)&pObjData->dwData;
			sim::aircraft.pS = (SimResponse*)&pObjData->dwData;

			// print data
			/*std::cout
				<< "\rAltitude: " << sim::aircraft.pS->altitude << std::endl;
				 << "- Heading: " << pS->heading
				<< "- Speed (knots): " << pS->speed
				<< "- Vertical Speed: " << pS->vertical_speed << "  "
				<< std::flush;*/


			//prueba = &pS->altitude;

			break;
		}

		break;
	}
	default:
		break;
	}
}

bool initSimEvents() {
	HRESULT hr;

	double xd = 7.7;
	prueba = &xd;


	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Client Event", NULL, NULL, NULL, NULL))) {
		std::cout << "Connected To Microsoft Flight Simulator 2020!" << std::endl;

		// Initialize sub interfaces
		sim::aircraft.init(hSimConnect);
		std::cout << "\nInit\n";

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


			sim::aircraft.throttle1->set((rand() % 100) + 0.1);
			sim::aircraft.throttle2->set((rand() % 50)+0.1);
			std::cout << sim::aircraft.pS->altitude << std::endl;

			//sim::aircraft.altitude->get();

			//std::cout << *prueba << std::endl;
			
			//std::cout << Sim::aircraft.altitude->get() << std::endl;


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


	//sim.aircraft->engines.set();

	// If could not connect to MFS2020 retry
	//while (!initSimEvents())
	//{
	initSimEvents();
	//}

}


