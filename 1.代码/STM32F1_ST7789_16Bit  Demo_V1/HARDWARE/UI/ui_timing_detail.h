#ifndef __UI_TIMING_DETAIL_H
#define __UI_TIMING_DETAIL_H

#include "sys.h"

#define TIMING_DETAIL_ACT_NONE  0xFF
#define TIMING_DETAIL_ACT_BACK  0x10

void UI_TimingDetail_Open(u8 mode);
u8   UI_TimingDetail_Scan(void);

#endif
