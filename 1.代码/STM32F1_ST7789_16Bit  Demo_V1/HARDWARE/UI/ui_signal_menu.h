#ifndef __UI_SIGNAL_MENU_H
#define __UI_SIGNAL_MENU_H

#include "sys.h"

#define SIGNAL_ITEM_TIMEBASE    0
#define SIGNAL_ITEM_NONE        0xFF

#define SIGNAL_ACT_NONE         0xFF
#define SIGNAL_ACT_CLEAR        0x12
#define SIGNAL_ACT_BACK         0x10
#define SIGNAL_ACT_STOP         0x13
#define SIGNAL_ACT_SWITCH       0x11
#define SIGNAL_ACT_OK           0x20

void UI_SignalMenu_Draw(void);
void UI_SignalMenu_SetOutput(u8 output);
u8   UI_SignalMenu_GetSelected(void);
u8   UI_SignalMenu_Scan(void);

#endif
