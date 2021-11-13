#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include "SimConnect.h"
#include <iostream>

int     quit = 0;
HANDLE  hSimConnect = NULL;
#define BASE_DATA_ID 1000
typedef bool SETTABLE;

static enum DATA_DEFINITION_ID {
	DEFINITION_1,
	DEFINITION_2,
};

static enum DATA_REQUEST_ID {
	REQUEST_1,
	REQUEST_2,
};


static enum DATA_NAMES {
	DATANAME1,
	DATANAME2,
};

/*
	Main manager of the simvars
*/
class Data
{
public:
	int ID; // Unique global identifier for SimVar

private:
	HANDLE  hSimConnect;
	DATA_DEFINITION_ID  DefineID;	// This will be the identifier for the variable, its needed to do some operations, ex: edit variable, crate event
	const char* DatumName;			// Name of the MFS2020 simulation variable. Docs: file:///C:/MSFS%20SDK/Documentation/html/Programming_Tools/SimVars/Simulation_Variables.htm
	const char* UnitsName;			// Specifies the units of the variable.     Docs: file:///C:/MSFS%20SDK/Documentation/html/Programming_Tools/SimVars/Simulation_Variable_Units.htm
	SIMCONNECT_DATATYPE DatumType;  // Data type of the variable, specification by SDK Docs: ???
	DATA_NAMES DataName;
	SETTABLE isSettable;            // A bool that indicated is variable can be modified		
	

public:
	Data(
		HANDLE  _hSimConnect,
		DATA_DEFINITION_ID  _DefineID,
		const char* _DatumName,
		const char* _UnitsName,
		SIMCONNECT_DATATYPE _DatumType,
		SETTABLE _isSettable
	) : hSimConnect(_hSimConnect), DefineID(_DefineID), DatumName(_DatumName), UnitsName(_UnitsName), isSettable(_isSettable), DatumType(_DatumType)
	{

		static int _id = BASE_DATA_ID;
		ID = _id++;
		SimConnect_AddToDataDefinition(hSimConnect, _DefineID, _DatumName, _UnitsName, DatumType, 0, ID);
		std::cout << "new sim object: '" << _DatumName << "' ID->" << ID << std::endl;
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



struct SimConnectResponse {
	int		id;
	float	value;
};

#define maxReturnedItems	0

// A structure that can be used to receive Tagged data
struct SimConnectSerializer {
	SimConnectResponse  response[1];
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
		/*case DEFINITION_1:
		{

			int	count = 0;;
			StructDatum* pS = (StructDatum*)&pObjData->dwData;
			while (count < (int)pObjData->dwDefineCount)
			{
				switch (pS->datum[count].id)
				{
				case DATANAME1:
					printf("\nTHROTTLE1 = %f", pS->datum[count].value);
					break;									
				default:
					break;
				}
				++count;
			}
			break;
		}*/
		case DEFINITION_1:
		{
			int	count = 0;;
			SimConnectSerializer* pS = (SimConnectSerializer*)&pObjData->dwData;
			std::cout << (int)pObjData->dwDefineCount << std::endl;

			while (count < (int)pObjData->dwDefineCount)
			{

				switch (pS->response[count].id)
				{
				case 1000:
					printf("\nTHROTTLE1 = %f", pS->response[count].value);
					break;
				case 1001:
					printf("\nTHROTTLE2 = %f", pS->response[count].value);
					break;
				case 1002:
					printf("\nTHROTTLE2 = %f", pS->response[count].value);
					break;
				default:
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
		//hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GENERAL ENG THROTTLE LEVER POSITION:1", "Bool", SIMCONNECT_DATATYPE_FLOAT32, 0 , DATANAME1);

		Data throttle1(hSimConnect, DEFINITION_1, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", SIMCONNECT_DATATYPE_FLOAT32, true);
		Data throttle2(hSimConnect, DEFINITION_1, "GENERAL ENG THROTTLE LEVER POSITION:2", "percent", SIMCONNECT_DATATYPE_FLOAT32, true);
		Data gear_left(hSimConnect, DEFINITION_1, "GEAR LEFT POSITION", "percent", SIMCONNECT_DATATYPE_FLOAT32, true);

		

		//hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GENERAL ENG THROTTLE LEVER POSITION:1", "Bool", SIMCONNECT_DATATYPE_FLOAT32, 0, DATANAME1);
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME, SIMCONNECT_DATA_REQUEST_FLAG_CHANGED | SIMCONNECT_DATA_REQUEST_FLAG_TAGGED);
		//hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", SIMCONNECT_DATATYPE_FLOAT64);
		
		//hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);

		//hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_2, THROTTLE1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);

		while (0 == quit)
		{
			//throttle1.set(7.7);
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





