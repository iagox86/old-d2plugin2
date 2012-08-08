// This is the file that takes care of the player losing health during a game.  
// It allows the player to set his minimum health, and, when he loses health, it
// checks his current health against his minimum and leaves if it is below.

#include <stdio.h>

#include "Health.h"		// Definitions for this
#include "Display.h"	// For displaying messages

#include "Send.h"		// For leaving the game at low health

// The minimum health before leaving the game
static int minHealth = 50;

static int previousHealth = -1;
static int maxHealth = -1;


void __fastcall loseHealthProcess(int currentHealth)
{
	int healthChange = -1;

	// Set the new maxHealth, if we have one
	if(currentHealth > maxHealth)
		maxHealth = currentHealth;

	// Set up the healthChange if we can
	if(previousHealth != -1)
		healthChange = previousHealth - currentHealth;

	// Set the new previous health
	previousHealth = currentHealth;

	// If the healthchange was negative, they healed
	if(healthChange >= 0)
	{
		// Debug
		// display(ORANGE, "Your health decreased by %d", healthChange);
	}

	// Debug
	//display(ORANGE, "Your health is now %d", currentHealth);
	if(currentHealth == 0)
	{
		// If they have 0 health, then they're dead, do nothing except print a quick
		// report
		if(healthChange > 0)
			display(RED, "Sorry, that hit did %d damage, killing you.", healthChange);
		else
			display(RED, "Sorry, there was nothing I could do :(");
	}
	else if(currentHealth < minHealth)
	{
		display(RED, "Health has fallen below minHealth, leaving the game.");
		// If they're below the minHealth, leave the game
		leaveGame();
	}
	else if(healthChange > 0 && (healthChange > currentHealth))
	{
		// If they lose moret han half of their remaining health, leave
		leaveGame();
	}


	return;
}

void __fastcall setMin(int newminHealth)
{
	minHealth = newminHealth;

	display(CYAN, "minimum health is now %d", minHealth);
}
	
void __fastcall initialize(int minHealth)
{

}


void __declspec(naked) __fastcall loseHealth()
{
	__asm
	{
		mov ecx, esi
		shr ecx, 8
		call loseHealthProcess
		ret
	}
}