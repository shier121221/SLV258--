#ifndef __UI_MECHANICS_MENU_H
#define __UI_MECHANICS_MENU_H

#include "sys.h"

#define MECH_ITEM_PENDULUM      0
#define MECH_ITEM_COLLISION     1
#define MECH_ITEM_GRAVITY       2
#define MECH_ITEM_LINEAR        3
#define MECH_ITEM_ANGULAR       4
#define MECH_ITEM_NONE          0xFF

#define MECH_ACT_NONE           0xFF
#define MECH_ACT_CLEAR          0x12
#define MECH_ACT_BACK           0x10
#define MECH_ACT_STOP           0x13
#define MECH_ACT_SWITCH         0x11
#define MECH_ACT_OK             0x20

void UI_MechanicsMenu_Draw(void);
void UI_MechanicsMenu_SetMeasuring(u8 measuring);
u8   UI_MechanicsMenu_GetSelected(void);
u8   UI_MechanicsMenu_Scan(void);

#endif
