#ifndef __UI_TIMING_MENU_H
#define __UI_TIMING_MENU_H

#include "sys.h"

#define TIMING_ITEM_S1       0
#define TIMING_ITEM_S2       1
#define TIMING_ITEM_COUNT    2
#define TIMING_ITEM_GENERAL  3
#define TIMING_ITEM_NONE     0xFF

#define TIMING_ACT_NONE      0xFF
#define TIMING_ACT_CLEAR     0x12
#define TIMING_ACT_BACK      0x10
#define TIMING_ACT_STOP      0x13
#define TIMING_ACT_SWITCH    0x11
#define TIMING_ACT_OK        0x20

void UI_TimingMenu_Draw(void);
void UI_TimingMenu_SetMeasuring(u8 measuring);
u8   UI_TimingMenu_GetSelected(void);
u8   UI_TimingMenu_Scan(void);

#endif
