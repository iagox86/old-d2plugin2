#include "Constants.h"
#include "Display.h"
#include "Health.h"
#include "MemoryEdit.h"
#include "ProcessChat.h"
#include "Send.h"

#include <iostream>

#define NAME "D2Plugin - by iago - Version 1.2 #1"

MemoryEdit *ME = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  dwReason, 
                       LPVOID lpReserved
					 )
{
	if(DLL_PROCESS_ATTACH == dwReason)
	{
		// Load necessary libraries
		if(!GetModuleHandle("d2client.dll"))
		{
			LoadLibrary("d2client.dll");
		}

		// Create the new memory editor
		ME = new MemoryEdit(GetCurrentProcess());

		// Do the memory patching here.
		if(
			ME->PatchMem(PROCESSCHAT_ADDR, ProcessChat, PROCESSCHAT_SIZE, false, MemoryEdit::EBX) &&
			//ME->PatchMem(LOSEHEALTH_ADDR, loseHealth,  LOSEHEALTH_SIZE, true,   MemoryEdit::EBP) &&
			ME->PatchMem(SEND_ADDR,       sendData,    SEND_SIZE,       false,  MemoryEdit::EBX) 
		   )
		{
			display(ORANGE, "Memory patched, plugin is ready." );

		}
		//else
		//{
		//	display(RED, "There was an error patching Diablo ii, no changes made.");
		//	delete ME;
		//}
	}
	else if(DLL_PROCESS_DETACH == dwReason)
	{

		//display(ORANGE, "Dll ejected, memory restored!");
		if(ME != NULL)
			delete ME;

	}


	return true;
}
