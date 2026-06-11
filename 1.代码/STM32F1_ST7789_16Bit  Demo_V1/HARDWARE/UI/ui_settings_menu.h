#ifndef __UI_SETTINGS_MENU_H
#define __UI_SETTINGS_MENU_H

#include "sys.h"

#define SETTINGS_ITEM_SELFTEST  0
#define SETTINGS_ITEM_NONE      0xFF

#define SETTINGS_ACT_NONE       0xFF
#define SETTINGS_ACT_CLEAR      0x12
#define SETTINGS_ACT_BACK       0x10
#define SETTINGS_ACT_STOP       0x13
#define SETTINGS_ACT_SWITCH     0x11
#define SETTINGS_ACT_OK         0x20

void UI_SettingsMenu_Draw(void);
u8   UI_SettingsMenu_GetSelected(void);
u8   UI_SettingsMenu_Scan(void);

#endif
