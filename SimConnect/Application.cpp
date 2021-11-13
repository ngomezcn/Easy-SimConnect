// Copyright (c) Asobo Studio, All rights reserved. www.asobostudio.com
//------------------------------------------------------------------------------
//
//  SimConnect Data Request Sample
//  
//	Description:
//				After a flight has loaded, request the lat/lon/alt of the user 
//				aircraft
//------------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include "SimConnect.h"
#include <iostream>



int     quit = 0;
HANDLE  hSimConnect = NULL;

struct Struct1
{
	//double  left_gear;
	//double  brakes;
	//double  heading;
	double  throttle1;
};

static enum EVENT_ID {
	EVENT_SIM_START,
};

static enum DATA_DEFINITION_ID {
	DEFINITION_1,
	THROTTLE1,
	THROTTLE2,
};

static enum DATA_REQUEST_ID {
	REQUEST_1,
	REQUEST_2,
};

/*
	Custom Easy SimConnect types
*/
typedef bool SETTABLE;


/*
	Main manager of the simvars
*/
class Data
{
private:
	HANDLE  hSimConnect;
	DATA_DEFINITION_ID  DefineID;	// This will be the identifier for the variable, its needed to do some operations, ex: edit variable, crate event
	const char* DatumName;			// Name of the MFS2020 simulation variable. Docs: file:///C:/MSFS%20SDK/Documentation/html/Programming_Tools/SimVars/Simulation_Variables.htm
	const char* UnitsName;			// Specifies the units of the variable.     Docs: file:///C:/MSFS%20SDK/Documentation/html/Programming_Tools/SimVars/Simulation_Variable_Units.htm
	SIMCONNECT_DATATYPE DatumType;  // Data type of the variable, specification by SDK Docs: ???
	SETTABLE isSettable;            // A bool that indicated is variable can be modified		

public:
	Data(
		HANDLE  _hSimConnect,
		DATA_DEFINITION_ID  _DefineID,
		const char* _DatumName,
		const char* _UnitsName,
		SETTABLE _isSettable,
		SIMCONNECT_DATATYPE _DatumType = SIMCONNECT_DATATYPE_FLOAT64
	) : hSimConnect(_hSimConnect), DefineID(_DefineID), DatumName(_DatumName), UnitsName(_UnitsName), isSettable(_isSettable), DatumType(_DatumType)
	{
		SimConnect_AddToDataDefinition(hSimConnect, _DefineID, _DatumName, _UnitsName);
	}

	template <typename T>
	T set(T value)
	{
		if (!isSettable)
		{
			std::cout << "\nError! trying to set a no settable variable\n";
			return -1;
		}

		// TODO: make a switch
		if (DatumType == SIMCONNECT_DATATYPE_FLOAT64)
		{
			double _value = value;
			SimConnect_SetDataOnSimObject(hSimConnect, DefineID, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(_value), &_value);
			return 0;
		}
		if (DatumType == SIMCONNECT_DATATYPE_INT64)
		{
			int _value = value;
			SimConnect_SetDataOnSimObject(hSimConnect, DefineID, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(_value), &_value);
			return 0;
		}

		return -1;
	}
};


/*
	Handler of the data received and events.

	#TODO: Improve this, maybe create a class
*/


struct StructOneDatum {
	int		id;
	float	value;
};

// maxReturnedItems is 2 in this case, as the sample only requests
// vertical speed and pitot heat switch data
#define maxReturnedItems	1

// A structure that can be used to receive Tagged data
struct StructDatum {
	StructOneDatum  datum[maxReturnedItems];
};

void CALLBACK MyDispatchProcPDR(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
	HRESULT hr;

	switch (pData->dwID)
	{

	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
	{
		SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

		switch (pObjData->dwRequestID)
		{
		case DEFINITION_1:
		{
			int	count = 0;;
			StructDatum* pS = (StructDatum*)&pObjData->dwData;

			// There can be a minimum of 1 and a maximum of maxReturnedItems
			// in the StructDatum structure. The actual number returned will
			// be held in the dwDefineCount parameter.

			while (count < (int)pObjData->dwDefineCount)
			{
				switch (pS->datum[count].id)
				{
				case THROTTLE1:
					printf("\nTHROTTLE1 = %f", pS->datum[count].value);
					break;

				default:
					//printf("\nUnknown datum ID: %d", pS->datum[count].id);
					break;
				}
				++count;
			}
			break;
		}

		default:
			break;
		}
		break;
	}


	case SIMCONNECT_RECV_ID_QUIT:
	{
		quit = 1;
		break;
	}

	default:
		break;
	}
}

void initSimConnect()
{
	HRESULT hr;

	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Request Data", NULL, 0, 0, 0)))
	{
		printf("\nConnected to Flight Simulator!\n");
		
		//hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GEAR LEFT POSITION", "percent");
		//hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "BRAKE INDICATOR", "Position");
		//hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE HEADING DEGREES GYRO", "Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GENERAL ENG THROTTLE LEVER POSITION:1", "Bool", SIMCONNECT_DATATYPE_FLOAT32, 0 , THROTTLE1);

		//Data throttle1(hSimConnect, THROTTLE1, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", true);
		//hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", SIMCONNECT_DATATYPE_FLOAT64);
		
		//hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME, SIMCONNECT_DATA_REQUEST_FLAG_CHANGED | SIMCONNECT_DATA_REQUEST_FLAG_TAGGED);

		//hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_2, THROTTLE1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);

		while (0 == quit)
		{
			throttle1.set(777.7);
			SimConnect_CallDispatch(hSimConnect, MyDispatchProcPDR, NULL);
			Sleep(1);
		}

		hr = SimConnect_Close(hSimConnect);
	}
}

int main()
{
	initSimConnect();

	//Settable flap(777);
	//myMax(1, 1);
	//std::cout << myMax(1, 1);

	return 0;
}





