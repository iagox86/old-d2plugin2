#ifndef DISPLAY_H
#define DISPLAY_H

#include <windows.h>
#include <string>

// Display colors
static const int WHITE = 0x0000;
static const int RED = 0x0001;
static const int GREEN = 0x0002;
static const int BLUE = 0x0003;
static const int GREY = 0x0005;
static const int BLACK = 0x0006;
static const int LIGHTYELLOW = 0x0007;
static const int GOLD = 0x0008;
static const int YELLOW = 0x0009;
static const int DARKGREEN = 0x000a;
static const int PURPLE = 0x000b;
static const int NAVY = 0x000E;
static const int DARKCYAN = 0x0011;
static const int DARKBLUE = 0x0012;
static const int BROWN = 0x0013;
static const int DARKRED = 0x0014;
static const int BLUEGREEN = 0x0019;
static const int BRIGHTBLUE = 0x001A;
static const int ORANGE = 0x001c;
static const int CYAN = 0x001d;
static const int LIGHTBROWN = 0x001F;


void display(WORD color, const char *format, ...);

#endif