#pragma once

#include <winsock.h>
#include "Buffer.h"

// In-game packet codes
#define WALK					(BYTE) 0x01
#define WALK_TO					(BYTE) 0x02
#define RUN						(BYTE) 0x03
#define RUN_TO					(BYTE) 0x04
#define LEFT_ATTACK_LOCATION	(BYTE) 0x05
#define LEFT_ATTACK_ENTITY		(BYTE) 0x06
#define LEFT_RANGED_ENTITY		(BYTE) 0x07
#define UNKNOWN_COMBAT			(BYTE) 0x09
#define RIGHT_ATTACK_LOCATION	(BYTE) 0x0C
#define RIGHT_ATTACK_ENTITY		(BYTE) 0x0D
#define RIGHT_RANGED_ENTITY		(BYTE) 0x0E
#define USE_ENTITY				(BYTE) 0x13
#define GAMECHAT				(BYTE) 0x15
#define PICKUP					(BYTE) 0x16
#define DROP					(BYTE) 0x17
#define MOVE_TO_BUFFER			(BYTE) 0x18
#define INV_TO_CURSOR			(BYTE) 0x19
#define CURSOR_TO_BODY			(BYTE) 0x1A
#define BODY_TO_CURSOR			(BYTE) 0x1C
#define SWAP_EQUIPPED_ITEM		(BYTE) 0x1D
#define SWAP_EQUIPPED_FULL		(BYTE) 0x1E
#define SWAP_INV_ITEM			(BYTE) 0x1F
#define USE						(BYTE) 0x20
#define BELT_EXPLICIT_ADD		(BYTE) 0x23
#define BELT_TO_CURSOR			(BYTE) 0x24
#define BELT_USE				(BYTE) 0x26
#define ITEM_ON_ITEM			(BYTE) 0x27
#define SOCKET_ITEM				(BYTE) 0x28
#define STACK					(BYTE) 0x29
#define CURSOR_TO_CUBE			(BYTE) 0x2A
#define NPCTALK					(BYTE) 0x2F
#define CLOSE_NPCTALK			(BYTE) 0x30
#define COMPLETE_QUEST_NPC		(BYTE) 0x31
#define BUY_FROM_STORE			(BYTE) 0x32
#define SELL_ITEM				(BYTE) 0x33
#define IDENTIFY_ALL			(BYTE) 0x34
#define ID_GAMBLE				(BYTE) 0x37
#define REQUEST_BUY				(BYTE) 0x38
#define INCREASE_STAT			(BYTE) 0x3A
#define INCREASE_SKILL			(BYTE) 0x3B
#define CHANGE_SKILL			(BYTE) 0x3C
#define SHOW_QUESTS				(BYTE) 0x40
#define RESURRECT				(BYTE) 0x41
#define USE_WAYPOINT			(BYTE) 0x49
#define FINISH_WP				(BYTE) 0x4B
#define PUSH_BUTTON				(BYTE) 0x4F
#define DROP_GOLD				(BYTE) 0x50
#define COMPLETE_QUEST			(BYTE) 0x58
#define ENTITY_WALK				(BYTE) 0x59
#define UPDATE_POSITION			(BYTE) 0x5F
#define SWAP					(BYTE) 0x60
#define EQUIP_MERC				(BYTE) 0x61
#define RESURRECT_MERC			(BYTE) 0x62
#define MOVE_TO_BELT			(BYTE) 0x63
#define CREATE					(BYTE) 0x67
#define LEAVE					(BYTE) 0x68
#define CREATE2					(BYTE) 0x6A
#define KEEPALIVE				(BYTE) 0x6C


// The prototype for this one MUST match the prototype for winsock.send().
int __stdcall sendData(SOCKET s, BYTE *buf, int length, int flags);
char *getLocation(DWORD num);
BOOL toggleVerbose();
void leaveGame();
void sendPacket(Buffer packet);
