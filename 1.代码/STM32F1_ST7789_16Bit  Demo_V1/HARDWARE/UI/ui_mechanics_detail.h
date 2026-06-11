#ifndef __UI_MECHANICS_DETAIL_H
#define __UI_MECHANICS_DETAIL_H

#include "sys.h"

#define MECH_DETAIL_ACT_NONE   0xFF
#define MECH_DETAIL_ACT_BACK   0x10

void UI_MechanicsDetail_Open(u8 mode);
u8   UI_MechanicsDetail_Scan(void);

#endif
