#pragma once
#include <Windows.h>
#include "SimConnect.h"
#include <iostream>
//#include "Settable.h"

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
	int heading;
	int speed;
	int vertical_speed;
};
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
		INT,
		DOUBLE,
		BOOL,
	};
}

/*
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

};*/

/*
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
*/
