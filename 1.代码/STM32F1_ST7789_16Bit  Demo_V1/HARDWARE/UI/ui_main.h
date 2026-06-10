#ifndef __UI_MAIN_H
#define __UI_MAIN_H

#include "sys.h"

// RGB565 custom colors
#define UI_BG           0xEF7B  // page background #F2F5F9
#define UI_TITLEBAR     0x2A4F  // title bar #3A5F8B
#define UI_CARD         0xFFFF  // card normal WHITE
#define UI_CARD_PRESS   0xD7DF  // card pressed #E6F0FF
#define UI_TEXT         0x18E3  // main text #333333
#define UI_SUBTEXT      0x2949  // sub text #555555
#define UI_RED_TEXT     0xE007  // clear key red #E03030
#define UI_ORANGE_TEXT  0xF9A0  // OK key orange #FF9800
#define UI_ORANGE       0xFA20  // orange fill used by main menu
#define UI_SHADOW       0xD69A  // soft shadow gray

// Layout constants (320x240 landscape)
#define TITLEBAR_Y      0
#define TITLEBAR_H      36
#define CARDS_Y         36
#define CARDS_H         164
#define BTNBAR_Y        200
#define BTNBAR_H        40

// Card IDs
#define CARD_TIMING     0
#define CARD_MECHANIC   1
#define CARD_SIGNAL     2
#define CARD_SETTINGS   3
#define CARD_NONE       0xFF

// Button IDs
#define BTN_CLEAR       0
#define BTN_BACK        1
#define BTN_STOP        2
#define BTN_SWITCH      3
#define BTN_OK          4
#define BTN_NONE        0xFF

void UI_Main_Draw(void);
u8   UI_Main_Scan(void);   // returns CARD_x or BTN_x or 0xFF if nothing

#endif
