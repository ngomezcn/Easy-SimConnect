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
	double  left_gear;
	double  brakes;
	double  heading;
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

typedef bool SETTABLE;
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

void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
	HRESULT hr;

	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
	{
		SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;
		std::cout 
			<< "dwDefineCount " << pObjData->dwDefineCount << "       " << 
			" | dwRequestID " << pObjData->dwRequestID << "         " <<
			" | dwObjectID " << pObjData->dwObjectID << "         " <<
			" | dwDefineID " << pObjData->dwDefineID << "         " <<
			" | dwFlags " << pObjData->dwFlags << "         " <<
			" | dwData " << pObjData->dwData << "         " <<

			
		std::endl;

		switch (pObjData->dwRequestID)
		{
		case REQUEST_1:
		{
			DWORD ObjectID = pObjData->dwObjectID;
			Struct1* pS = (Struct1*)&pObjData->dwData;
			/*std::cout << "\r thrott:" << pS->throttle1 << "       " <<
			    " | brakes:" << pS->brakes << "       " <<
				" | gear:" << pS->left_gear << "       " <<
				" | heading:" << pS->heading << "       " <<


			std::flush;*/

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
		//("\nReceived:%d",pData->dwID);
		break;
	}
}

void initSimConnect()
{
	HRESULT hr;


	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Request Data", NULL, 0, 0, 0)))
	{
		printf("\nConnected to Flight Simulator!\n");
		
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GEAR LEFT POSITION", "percent");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "BRAKE INDICATOR", "Position", SIMCONNECT_DATATYPE_FLOAT64, 0, 600);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE HEADING DEGREES GYRO", "Degrees", SIMCONNECT_DATATYPE_FLOAT64, 0, 77);
		
		Data throttle1(hSimConnect, THROTTLE1, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", true);
		//hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", SIMCONNECT_DATATYPE_FLOAT64);
		
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND, SIMCONNECT_DATA_REQUEST_FLAG_TAGGED);
		//hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_2, THROTTLE1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);

		while (0 == quit)
		{
			throttle1.set(777.7);
			SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, NULL);
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





