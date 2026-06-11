#ifndef __UI_SIGNAL_DETAIL_H
#define __UI_SIGNAL_DETAIL_H

#include "sys.h"

#define SIGNAL_DETAIL_ACT_NONE  0xFF
#define SIGNAL_DETAIL_ACT_BACK  0x10

void UI_SignalDetail_Open(void);
u8   UI_SignalDetail_Scan(void);

#endif
