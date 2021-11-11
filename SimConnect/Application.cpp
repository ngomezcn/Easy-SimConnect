#include <iostream>
#include <Windows.h>

#include "SimConnect.h"


int main()
{
	HANDLE hSimConnect = NULL;

	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Client Event", NULL, NULL, NULL, NULL)))
	{
		std::cout << "SIM CONNECT WORKING" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect";
	}
}