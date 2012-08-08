#include "Send.h"
#include "Display.h"
#include "Buffer.h"
#include "Constants.h"


	
SOCKET d2Socket;

DWORD lastItemGrabbed = -1;
DWORD horadricCube = -1;
BOOL verbose = false;

// This hooks D2's sendData function.  It can be used for packet logging, filtering,
// displaying, etc.
int __stdcall sendData(SOCKET s, BYTE *buf, int length, int flags)
{
	// Save the socket
	d2Socket = s;

	BOOL sendOut = true;
	BOOL log = true;

	Buffer b(buf, length);

	BYTE code;
	b >> code;

	// Don't display these packets:
	if(code == CREATE || code == CREATE2 || code == KEEPALIVE || code == LEAVE)
		log = false;

	// This is the data that is displayed and/or logged
	char data[256]; 
	*data = '\0';

	// Process the various packets
	switch(code)
	{
	case WALK:
		{
			WORD x;
			WORD y;

			b >> x >> y;
			sprintf(data, "Walked to position (%04x,%04x)", x, y);
		}
		break;
	case WALK_TO:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Walked to entity: type: %08x, code: %08x", entityType, entityCode);
		}
		break;
	case RUN:
		{
			WORD x;
			WORD y;

			b >> x >> y;
			sprintf(data, "Ran to position (%04x,%04x)", x, y);
		}
		break;
	case RUN_TO:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Ran to entity: type: %08x, code: %08x", entityType, entityCode);
		}
		break;
	case LEFT_ATTACK_LOCATION:
		{
			WORD x;
			WORD y;

			b >> x >> y;
			sprintf(data, "Left-attacking location (%04x,%04x)", x, y);
		}
		break;
	case LEFT_ATTACK_ENTITY:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Left-attacking entity: type: %08x, code: %08x", entityType, entityCode);
		}
		break;
	case LEFT_RANGED_ENTITY:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Left-ranged attack at entity: type: %08x, code: %08x", entityType, entityCode);
		}
		break;
	case UNKNOWN_COMBAT:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Unknown packet from combat: Type: %08x, Code: %08x", entityType, entityCode);
		}
		break;

	case RIGHT_ATTACK_LOCATION:
		{
			WORD x;
			WORD y;

			b >> x >> y;
			sprintf(data, "Used right skill on location (%04x, %04x)", x, y);
		}
		break;
	case RIGHT_ATTACK_ENTITY:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Used right skill on entity: type: %08x, code: %08x", entityType, entityCode);
		}
		break;
	case RIGHT_RANGED_ENTITY:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Right-ranged attack at entity: type: %08x, code: %08x", entityType, entityCode);
		}
		break;

	case USE_ENTITY:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType;
			b >> entityCode;
			sprintf(data, "Using an object: Type: %08x, Code: %08x", entityType, entityCode);
		}
		break;

	case GAMECHAT:
		{
			sprintf(data, "You talked");
		}
		break;

	case PICKUP:
		{
			DWORD entityType;
			DWORD entityCode;
			DWORD toCursor;
			
			b >> entityType >> entityCode >> toCursor;
			sprintf(data, "Picking up item from ground to %s: Type: %08x, Code: %08x", toCursor ? "cursor" : "inventory", entityType, entityCode);

			// If we already failed to pick it up, grab it in our hand
			if(toCursor == false && lastItemGrabbed == entityCode)
			{
				Buffer hand(PICKUP);
				hand << entityType;
				hand << entityCode;
				hand << (DWORD) 1;
				send(s, (const char*)hand.c_str(), hand.GetSize(), 0);
				
				sendOut = false;
			}

			// Set the last item
			if(toCursor == false)
				lastItemGrabbed = entityCode;
		}
		break;

	case DROP:
		{
			DWORD entityCode;

			b >> entityCode;
			sprintf(data, "Dropping item from cursor: %08x", entityCode);
		}
		break;

	case MOVE_TO_BUFFER:
		{
			DWORD entityCode;
			DWORD unk1;
			DWORD unk2;
			DWORD unk3;

			b >> entityCode >> unk1 >> unk2 >> unk3;
			sprintf(data, "Buffer move: unk1: %08x, unk2: %08x, unk3: %08x", unk1, unk2, unk3);
		}
		break;

	case INV_TO_CURSOR:
		{
			DWORD entityCode;

			b >> entityCode;

			sprintf(data, "Picked up item from inventory: %08x", entityCode);
		}
		break;

	case CURSOR_TO_BODY:
		{
			DWORD entityCode;
			DWORD location;

			b >> entityCode >> location;
			sprintf(data, "Equipped item %08x to %s", entityCode, getLocation(location));
		}
		break;

	case BODY_TO_CURSOR:
		{
			WORD location;

			b >> location;
			sprintf(data, "Removed %s", getLocation(location));
		}
		break;

	case SWAP_EQUIPPED_ITEM:
		{
			DWORD entityCode;
			DWORD location;

			b >> entityCode >> location;
			sprintf(data, "Swapped item %08x with equipped %s", entityCode, getLocation(location));
		}
		break;

	case SWAP_EQUIPPED_FULL:
		{
			DWORD entityCode;
			DWORD location;

			b >> entityCode >> location;
			sprintf(data, "swapped item %08x for item in both hands (%s)", entityCode, getLocation(location));
		}
		break;

	case SWAP_INV_ITEM:
		{
			DWORD cursorItem;
			DWORD invItem;
			DWORD bufferCode1;
			DWORD bufferCode2;

			b >> cursorItem >> invItem >> bufferCode1 >> bufferCode2;
			sprintf(data, "Swapping %08x with %08x, in buffer %08x - %08x", cursorItem, invItem, bufferCode1, bufferCode2);
		}
		break;

	case USE:
		{
			DWORD entityCode;
			DWORD x;
			DWORD y;

			b >> entityCode >> x >> y;
			sprintf(data, "Used item %08x at location (%04x, %04x)", entityCode, x, y);
		}
		break;

	case BELT_EXPLICIT_ADD:
		{
			DWORD entityCode;
			DWORD location;

			b >> entityCode >> location;
			DWORD locationy = (location % 4) + 1;
			DWORD locationx = (location / 4) + 1;

			sprintf(data, "Inserting item %08x into belt in row %d, column %d", entityCode, locationx, locationy);
		}
		break;

	case BELT_TO_CURSOR:
		{
			DWORD entityCode;

			b >> entityCode;
			sprintf(data, "Removed item %08x from belt", entityCode);
		}
		break;

	case BELT_USE:
		{
			DWORD entityCode;
			DWORD unk1;
			DWORD unk2;

			b >> entityCode >> unk1 >> unk2;
			sprintf(data, "Removed item %08x from belt (unk1: %08x, unk2: %08x)", entityCode, unk1, unk2);
		}
		break;

	case ITEM_ON_ITEM:
		{
			DWORD destination;
			DWORD source;

			b >> destination >> source;
			sprintf(data, "Used item %08x on item %08x", source, destination);
		}
		break;

	case SOCKET_ITEM:
		{
			DWORD gem;
			DWORD item;

			b >> gem >> item;
			sprintf(data, "Inserted %08x into a socket in %08x", gem, item);
		}
		break;

	case STACK:
		{
			DWORD source;
			DWORD destination;

			b >> source >> destination;
			sprintf(data, "Stacking items: %08x goes into %08x", source, destination);
		}
		break;

	case CURSOR_TO_CUBE:
		{
			DWORD item;
			DWORD cube;

			b >> item >> cube;
			sprintf(data, "Placed item %08x into cube %08x", item, cube);

			horadricCube = cube;
		}
		break;

	case NPCTALK:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Talking to npc: type: %08x, code: %08x", entityType, entityCode);
		}
		break;

	case CLOSE_NPCTALK:
		{
			DWORD entityType;
			DWORD entityCode;

			b >> entityType >> entityCode;
			sprintf(data, "Stopped talking to npc: type: %08x, code: %08x", entityType, entityCode);
		}
		break;

	case COMPLETE_QUEST_NPC:
		{
			DWORD entityCode;
			DWORD questNumber;

			b >> entityCode >> questNumber;
			sprintf(data, "You just completed action %08x for %08x", questNumber, entityCode);
		}
		break;

	case BUY_FROM_STORE:
		{
			DWORD entityCode;
			DWORD unknown;
			DWORD unknown2;
			DWORD itemNumber;

			b >> entityCode >> unknown >> unknown2 >> itemNumber;
			sprintf(data, "Purchased %08x from %08x, unk1: %08x, unk2: %08x", itemNumber, entityCode, unknown, unknown2);
		}
		break;

	case SELL_ITEM:
		{
			sprintf(data, "Selling an item.  I have no idea how this works.");
		}
		break;

	case IDENTIFY_ALL:
		{
			DWORD entityCode;

			b >> entityCode;
			sprintf(data, "You just asked %08x to identify all your items.", entityCode);
		}
		break;

	case ID_GAMBLE:
		{
			DWORD entityCode;

			b >> entityCode;
			sprintf(data, "Identifying gambled item: %08x", entityCode);
		}
		break;

	case REQUEST_BUY:
		{
			DWORD entityType;
			DWORD entityCode;
			DWORD unk;

			b >> entityType >> entityCode >> unk;
			sprintf(data, "Requesting purchase screen: type: %08x, code: %08x, unk: %08x", entityType, entityCode, unk);
		}
		break;

	case INCREASE_STAT:
		{
			WORD which;

			b >> which;

			if(which == 0x0000)
				sprintf(data, "Increasing strength");
			else if(which == 0x0001)
				sprintf(data, "Increasing energy");
			else if(which == 0x0000)
				sprintf(data, "Increasing dexterity");
			else if(which == 0x0000)
				sprintf(data, "Increasing vitality");
		}
		break;

	case INCREASE_SKILL:
		{
			WORD which;

			b >> which;
			sprintf(data, "Increasing skill: %04x", which);
		}
		break;

	case CHANGE_SKILL:
		{
			WORD skill;
			WORD side;
			DWORD unknown;

			b >> skill >> side >> unknown;
			sprintf(data, "Changing skill for %s side: %04x", side == 0x8000 ? "left" : "right", skill);
		}
		break;

	case SHOW_QUESTS:
		{
			sprintf(data, "Viewing quests");
		}
		break;

	case RESURRECT:
		{
			sprintf(data, "Getting up.");
		}
		break;

	case USE_WAYPOINT:
		{
			DWORD entityCode;
			DWORD location;

			b >> entityCode >> location;
			if(location == 0)
				sprintf(data, "Canceled waypoint %08x", entityCode);
			else
				sprintf(data, "Used waypoint %08x to go to %08x", entityCode, location);
		}
		break;

	case FINISH_WP:
		{
			DWORD entityCode;

			b >> entityCode;

			sprintf(data, "Exited waypoint %08x", entityCode);
		}
		break;

	case PUSH_BUTTON:
		{
			DWORD button;
			WORD extraData;

			b >> button >> extraData;
			sprintf(data, "You just pressed button %08x (extraData: %u)", button, extraData);
		}
		break;

	case DROP_GOLD:
		{
			DWORD unk;
			DWORD amount;

			b >> unk >> amount;
			sprintf(data, "Dropping %d gold (unk: %08x)", amount, unk);
		}
		break;

	case COMPLETE_QUEST: 
		{
			WORD questNum;

			b >> questNum;
			sprintf(data, "Completed quest %d", questNum);
		}
		break;


	case ENTITY_WALK:
		{
			DWORD entityType;
			DWORD entityCode;
			DWORD unk1;
			DWORD unk2;

			b >> entityType >> entityCode >> unk1 >> unk2;
			sprintf(data, "Asking NPC to walk: type: %08x, code: %08x, unk1: %08x, unk2: %08x", entityType, entityCode, unk1, unk2);
		}
		break;

	case UPDATE_POSITION:
		{
			WORD x;
			WORD y;

			b >> x >> y;
			sprintf(data, "Sending new location to confused server: (%04x, %04x)", x, y);
		}
		break;

	case SWAP:
		{
			sprintf(data, "Swapping hands");
		}
		break;

	case EQUIP_MERC:
		{
			WORD action;

			b >> action;
			if(action == 0x0000)
				sprintf(data, "Equipping mercenary or swapping one of his items");
			else if(action == 0x0001)
				sprintf(data, "Removing mercenary's helmet");
			else if(action == 0x0002)
				sprintf(data, "Removing mercenary's shield");
			else if(action == 0x0003)
				sprintf(data, "Removing mercenary's armour");
			else if(action == 0x0004)
				sprintf(data, "Removing mercenary's weapon");
			else
				display(RED, "Unknown mercenary-equipment action.");
		}
		break;

	case RESURRECT_MERC:
		{
			DWORD entityCode;

			b >> entityCode;
			sprintf(data, "Resurrecting mercenary, code %08x", entityCode);
		}
		break;

	case MOVE_TO_BELT:
		{
			DWORD entityCode;

			b >> entityCode;
			sprintf(data, "Moving item to belt automatically: %08x", entityCode);
		}
		break;
		
	default:
		{
			if(log)
			{
				display(RED, "Unknown packet: 0x%02X:\n%s", code, b.toString().c_str());
				FILE *f = fopen(LOGFILE, "a");
				if(f)
				{
					fprintf(f, "Unknown packet: 0x%02X:\n%s\n\n\n", code, b.toString().c_str());
					fclose(f);
				}
			}
		}
	}

	if(verbose && log)
	{
		display(GREEN, data);
	}

	if(BIGLOG)
	{
		FILE * f = fopen(LOGFILE, "a");
		if(f)
		{
			fprintf(f, data);
			fclose(f);
		}
	}

	// either send out, or just return the length (which tells d2 that send didn't fail)
	if(sendOut)
	{
		return send(s, (const char*)buf, length, flags);
	}
	else
	{
		return length; 
	}
}

void leaveGame()
{
	char leave = LEAVE;

	send(d2Socket, &leave, 1, 0);
}

// This can really screw stuff up, but eh?
void sendPacket(Buffer packet)
{
	send(d2Socket, (char*)packet.c_str(), packet.GetSize(), NULL);
}


char *getLocation(DWORD num)
{
	if(num < 0x01 || num > 0x0A) 
		return "unknown";

	char *locations[] = { "helmet", "amulet", "armour", "left hand", "right hand", "left ring", "right ring", "belt", "boots", "gloves" };

	return locations[num - 1];
}

BOOL toggleVerbose()
{
	verbose = !verbose;
	if(verbose)
		display(CYAN, "Verbose turned on");
	else
		display(CYAN, "Verbose turned off");

	return verbose;
}