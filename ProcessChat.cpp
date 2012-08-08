#include <stdio.h>
#include <stdlib.h>

#include "Buffer.h"
#include "Constants.h"
#include "Display.h"
#include "Health.h"
#include "Send.h"

typedef int BOOL;

#define NOTPROCESSED 0
#define PROCESSED	 1


static BOOL __fastcall process( const char *command, const char *parameter)
{
	if(stricmp(command, "min") == 0)
	{
		if(parameter != NULL)
		{
			setMin(atoi(parameter));
		}
		else
		{
			display(RED, "Error, please specify a minimum health, ie, \"/setmin 50\"");
		}
		return PROCESSED;
	}

	if(stricmp(command, "log") == 0)
	{
		if(parameter != NULL)
		{
			FILE *f = fopen(LOGFILE, "a");
			if(f)
			{
				fprintf(f, "%s\n\n", parameter);
				fclose(f);
				display(CYAN, "Added to log: %s", parameter);
			}
		}
		else
		{
			display(RED, "What did you want to log?");
		}

		return PROCESSED;
	}

	if(stricmp("v", command) == 0 || stricmp("verbose", command) == 0)
	{
		toggleVerbose();
		return PROCESSED;
	}

	if(stricmp("rkick", command) == 0)
	{
		Buffer k(CHANGE_SKILL);
		k << (WORD) 0x0001;
		k << (WORD) 0x0000;
		k << (DWORD) -1;
		sendPacket(k);
		display(CYAN, "Your right skill is now kick");

		return PROCESSED;
	}

	if(stricmp("rkick", command) == 0)
	{
		Buffer k(CHANGE_SKILL);
		k << (WORD) 0x0001;
		k << (WORD) 0x8000;
		k << (DWORD) -1;
		sendPacket(k);
		display(CYAN, "Your left skill is now kick");

		return PROCESSED;
	}

	return NOTPROCESSED;
}

// This is automatically called by diablo ii whenever an outgoing chat message
// with a leading / is sent out during a game.
BOOL __fastcall ProcessChat( const char *message )
{
	// If we had no message, just exit
	if(message == NULL || strlen(message) == 0)
		return NOTPROCESSED;

	// The command they typed (eg, ***/w*** iago hey!)
	char *command;
	// The parameter(s) (eg, /w ***iago hey!***)
	char *parameter;

	// Make a copy of the message that we can play with
	char *bak = (char*) malloc(strlen(message));

	// This may be cause by an out of memory error
	if(bak == NULL)
		return NOTPROCESSED;

	// Copy everything except the leading slash into bak
	strcpy( bak, message + 1 );

	// Set command to the beginning of the string
	command = bak;
	// Set the parameter to the first space
	parameter = strchr(bak, ' ');
	if(parameter != NULL)
	{
		// If it found a space, turn the space to \0 
		*parameter = '\0';
		// And move the parameter to the character after the space
		parameter++;
	}

	// ** At this point, we have:
	//    command = the command string, with no leading /, at least 1 character
	//    parameter = the parameter string, or NULL
	//display(BLUE, "Command: %s\nParameter: %s", command, parameter == NULL ? "[blank]" : parameter);
	
	BOOL ret = process(command, parameter);
	free(bak);
	return ret;
}
