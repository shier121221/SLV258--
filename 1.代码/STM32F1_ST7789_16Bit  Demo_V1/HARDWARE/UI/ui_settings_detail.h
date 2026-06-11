#ifndef __UI_SETTINGS_DETAIL_H
#define __UI_SETTINGS_DETAIL_H

#include "sys.h"

#define SETTINGS_DETAIL_ACT_NONE  0xFF
#define SETTINGS_DETAIL_ACT_BACK  0x10

void UI_SettingsDetail_Open(void);
u8   UI_SettingsDetail_Scan(void);

#endif
