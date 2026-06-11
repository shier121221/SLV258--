#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "sys.h"
#include "CTP.h"

#define TP_PRES_DOWN 0x80  // Touch is currently pressed
#define TP_CATH_PRES 0x40  // A press event has been captured
#define CT_MAX_TOUCH 5     // Maximum touch points for capacitive mode

// Touch controller abstraction and calibration data
typedef struct
{
    u8 (*init)(void);       // Initialize touch hardware
    u8 (*scan)(u8);         // Scan touch: 0 = screen coords, 1 = raw coords
    void (*adjust)(void);   // Run touch calibration
    u16 x[CT_MAX_TOUCH];    // Current X coordinates
    u16 y[CT_MAX_TOUCH];    // Current Y coordinates
    u8  sta;                // Touch status bitfield
                            // b7: 1 = pressed, 0 = released
                            // b6: 1 = new press captured
                            // b5: reserved
                            // b4..b0: active touch points for capacitive mode

    // Resistive touch calibration parameters
    float xfac;
    float yfac;
    short xoff;
    short yoff;

    // Touch type bitfield
    // b0: axis swap flag for resistive touch
    // b1..b6: reserved
    // b7: 0 = resistive, 1 = capacitive
    u8 touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;   // Global touch device state

// Resistive touch controller pins
#define TPEN  PBin(15)   // PB15 INT / PENIRQ
#define TSDO  PBin(14)   // PB14 MISO
#define TSDI  PBout(13)  // PB13 MOSI
#define TCLK  PBout(11)  // PB11 CLK
#define TCS   PBout(12)  // PB12 CS

void TP_Write_Byte(u8 num);                                              // Write one byte to touch IC
u16 TP_Read_AD(u8 CMD);                                                  // Read one ADC sample
u16 TP_Read_XOY(u8 xy);                                                  // Read filtered raw X or Y
u8 TP_Read_XY(u16 *x,u16 *y);                                            // Read one raw XY sample
u8 TP_Read_XY2(u16 *x,u16 *y);                                           // Read XY twice and validate
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color);                         // Draw calibration crosshair
void TP_Draw_Big_Point(u16 x,u16 y,u16 color);                           // Draw a 2x2 point
void TP_Save_Adjdata(void);                                              // Save calibration data
u8 TP_Get_Adjdata(void);                                                 // Load calibration data
void TP_Adjust(void);                                                    // Run calibration
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac); // Show calibration info
u8 TP_Scan(u8 tp);                                                       // Scan current touch state
u8 TP_Init(void);                                                        // Initialize touch driver

#endif
