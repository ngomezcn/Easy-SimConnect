
#include "smpch.h"

struct NotSettable
{
private:
	const char* datumName;		 // Name of the MFS2020 simulation variable. Docs: file:///C:/MSFS%20SDK/Documentation/html/Programming_Tools/SimVars/Simulation_Variables.htm
	const char* units;			 // Specifies the units of the variable.     Docs: file:///C:/MSFS%20SDK/Documentation/html/Programming_Tools/SimVars/Simulation_Variable_Units.htm
	DATA_DEFINITION_ID defineID; // Specifies the ID of the client defined data definition.
public:

	sim::type dType = sim::type::null;

	NotSettable(
		HANDLE hSimConnect,
		DATA_DEFINITION_ID _defineID,
		const char* _datumName,
		const char* _units,
		sim::type _dType)
		: datumName(_datumName), defineID(_defineID), units(_units), dType(_dType)
	{
		SimConnect_AddToDataDefinition(hSimConnect, _defineID, _datumName, units); // Adding simulation variable to the SDK observer
	}

	int set(int value)
	{
		SimConnect_SetDataOnSimObject(hSimConnect, defineID, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(value), &value);
		return 0;
	}

	int set(double value)
	{
		SimConnect_SetDataOnSimObject(hSimConnect, defineID, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(value), &value);
		return 0;
	}

	int get()
	{
		return 0;
	}
};