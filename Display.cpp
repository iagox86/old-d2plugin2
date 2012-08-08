#include "Buffer.h"
#include "Constants.h"
#include "Display.h"

#include <stdio.h>
#include <stdarg.h>


void display(WORD color, const char *format, ...)
{
	// Displays on the screen, using printf style format
	char OutStr[512];
	va_list argptr;
	va_start(argptr, format);
	//vsprintf(OutStr, format, argptr);
	vsprintf(OutStr, format, argptr);

	for(int i = 0; OutStr[i] != '\0'; i++)
	{
		if(OutStr[i] == '\t')
			OutStr[i] = ' ';
	}

	for(char *dispstr = strtok(OutStr, "\n"); dispstr; dispstr = strtok(NULL, "\n"))
	{
		Buffer b;

		// Add the 4 header bytes
		b << (BYTE) 0x26;
		b << (BYTE) 0x01;
		b << (BYTE) 0x00;
		b << (BYTE) 0x00;

		// For this, the object is null
		b << (DWORD) 0x00000000;

		// The color, for chat messages
		b << (WORD) color;

		// Just blank out the name
		b << "###############" << (BYTE) 0x00;

		// Add the text, null-terminated
		b << dispstr << (BYTE) 0x00;

		const BYTE *dispStr = b.c_str();

		// Do the actual send
		__asm
		{
			push ecx
			mov ecx, dispStr
			call D2Display
			pop ecx
		}
	}
}



