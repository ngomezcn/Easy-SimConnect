#include <iostream>
#include <Windows.h>

#include "SimConnect.h"

int quit = 0; // 0 = Keep Running - 1 = Exit app
HANDLE hSimConnect = NULL; // SimConnect Handle

// To make a data request we need:
static enum DATA_DEFINE_ID {
	DEFINITION_1,
};

static enum DATA_REQUEST_ID {
	REQUEST_1,
	REQUEST_2,
	// ... (we can use different request to organize our response data)
};

// This structure will hold the data
struct SimResponse {
	double altitude;
	int32_t heading;
	int32_t speed;
	int32_t vertical_speed;
};

// Event callback
void CALLBACK MyDispatchProc1(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext) {
	switch (pData->dwID)
	{
		// CASE SIMULATION DATA RECIVE EVENT
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
			std::cout
				<< "\rAltitude: " << pS->altitude
				<< "- Heading: " << pS->heading
				<< "- Speed (knots): " << pS->speed
				<< "- Vertical Speed: " << pS->vertical_speed
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
}

bool initSimEvents() {
	HRESULT hr;

	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Client Event", NULL, NULL, NULL, NULL))) {
		std::cout << "Connected To Microsoft Flight Simulator 2020!" << std::endl;

		// REQUESTING OUR DATA

		// #IMPORTANT: the request order must follow the declaration order o the response struct!
		// SimConnect_AddToDataDefinition takes: HANDLem enum DEFINITION_ID, const char* DATA_NAME, const char* UNIT, DATATYPE (default is FLOAT64)
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Indicated Altitude", "feet");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "HEADING INDICATOR", "degrees", SIMCONNECT_DATATYPE_INT32);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Airspeed Indicated", "knots", SIMCONNECT_DATATYPE_INT32);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "VERTICAL SPEED", "Feet per second", SIMCONNECT_DATATYPE_INT32);

		// EVERY SECOND REQUEST DATA FOR DEFINITION 1 ON THE CURRENT USER AIRCRAFT (SIMCONNECT_OBJECT_ID_USER)
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

		// Process incoming SimConnect Server messages while the app is running
		while (quit == 0) {
			// Call SimConnect_CallDispatch, MyDispatchProc1 will handle simulation events
			SimConnect_CallDispatch(hSimConnect, MyDispatchProc1, NULL);
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

	// If could not connect to MFS2020 retry
	//while (!initSimEvents())
	//{
	initSimEvents();
	//}


	
}