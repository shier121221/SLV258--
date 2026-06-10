#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "touch.h"
#include "test code.h"
#include "ui_main.h"
#include "ui_timing_menu.h"

#define RTP_RAW_TEST 0

// LCD data bus (16-bit parallel):
// DB0~DB6 = PA0~PA6
// DB7     = PA12
// DB8~DB15= PB3~PB10
// RS=PA8  WR=PA9  RD=PA10  RST=PA11  CS=GND  LED=PC14
//
// Touch (XPT2046):
// TPEN=PB15  TSDO=PB14  TSDI=PB13  TCLK=PB11  TCS=PB12
//
// Pulse test:
// PA7 = 10ms pulse output
// PB0, PB1, PA15 = digital input

// PA7: trigger pulse output (to external device, 10ms HIGH)
// PB0, PB1, PA15: photogate inputs (NPN: no object=HIGH, object=LOW)
static void pulse_io_init(void)
{
    RCC->APB2ENR |= (1<<0)|(1<<2)|(1<<3);

    // PA15 defaults to JTAG. Disable JTAG and keep SWD so PA15 can be GPIO.
    AFIO->MAPR &= ~(7u << 24);
    AFIO->MAPR |=  (2u << 24);

    // PA7 output push-pull, idle LOW
    GPIOA->CRL &= ~(0xFu << 28);
    GPIOA->CRL |=  (0x3u << 28);
    GPIOA->BRR  =  (1<<7);

    // PB0, PB1 input pull-up
    GPIOB->CRL &= ~(0xFFu << 0);
    GPIOB->CRL |=  (0x88u << 0);
    GPIOB->ODR |=  (1<<0)|(1<<1);

    // PA15 input pull-up
    GPIOA->CRH &= ~(0xFu << 28);
    GPIOA->CRH |=  (0x8u << 28);
    GPIOA->ODR |=  (1<<15);
}

static void wait_touch_release(void)
{
    do {
        tp_dev.scan(0);
        delay_ms(20);
    } while (tp_dev.sta & TP_PRES_DOWN);
}

static void rtp_gpio_init(void)
{
    RCC->APB2ENR |= (1<<0)|(1<<3);

    // PB11=TCLK, PB12=TCS, PB13=TSDI output; PB14=TSDO, PB15=TPEN input pull-up.
    GPIOB->CRH = (GPIOB->CRH & 0x00000FFFu) | 0x88333000u;
    GPIOB->ODR |= (1<<11)|(1<<12)|(1<<13)|(1<<14)|(1<<15);
}

static void rtp_map_raw_to_screen(u16 rx, u16 ry, u16 *sx, u16 *sy)
{
    u16 x;
    u16 y;

    if (USE_LCM_DIR == 0) {
        x = rx;
        y = ry;
    } else if (USE_LCM_DIR == 1) {
        x = 4095 - rx;
        y = 4095 - ry;
    } else if (USE_LCM_DIR == 2) {
        x = ry;
        y = 4095 - rx;
    } else {
        x = 4095 - ry;
        y = 4095 - rx;
    }

    *sx = (u16)(((u32)x * (lcddev.width - 1)) / 4095);
    *sy = (u16)(((u32)y * (lcddev.height - 1)) / 4095);
}

static void rtp_raw_test(void)
{
    u16 rx = 0;
    u16 ry = 0;
    u16 sx;
    u16 sy;
    u8 ok;
    u16 last_sx = 0xFFFF;
    u16 last_sy = 0xFFFF;

    rtp_gpio_init();

    LCD_Clear(WHITE);
    POINT_COLOR = BLACK;
    BACK_COLOR = WHITE;
    LCD_ShowString(8, 8, 160, 16, 16, (u8*)"RTP RAW TEST");
    LCD_ShowString(8, 28, 64, 16, 16, (u8*)"PEN:");
    LCD_ShowString(110, 28, 32, 16, 16, (u8*)"X:");
    LCD_ShowString(220, 28, 32, 16, 16, (u8*)"Y:");

    while (1) {
        LCD_Fill(56, 28, 319, 44, WHITE);
        LCD_ShowNum(56, 28, TPEN ? 1 : 0, 1, 16);

        if (TPEN == 0) {
            ok = TP_Read_XY2(&rx, &ry);
            if (ok) {
                LCD_ShowNum(134, 28, rx, 4, 16);
                LCD_ShowNum(244, 28, ry, 4, 16);
                rtp_map_raw_to_screen(rx, ry, &sx, &sy);
                if (last_sx != 0xFFFF && last_sy != 0xFFFF) {
                    gui_fill_circle(last_sx, last_sy, 4, WHITE);
                }
                gui_fill_circle(sx, sy, 4, RED);
                last_sx = sx;
                last_sy = sy;
            } else {
                LCD_ShowString(134, 28, 48, 16, 16, (u8*)"ERR");
                LCD_ShowString(244, 28, 48, 16, 16, (u8*)"ERR");
            }
        } else {
            LCD_ShowString(134, 28, 48, 16, 16, (u8*)"----");
            LCD_ShowString(244, 28, 48, 16, 16, (u8*)"----");
        }

        delay_ms(80);
    }
}


int main(void)
{
    u8 page = 0;

    Stm32_Clock_Init(9);
    delay_init(72);

    LCD_Init();
    tp_dev.touchtype |= USE_TP_TYPE;
    LCD_Display_Dir(USE_LCM_DIR);
    LCD_Clear(WHITE);
    LCD_LED = 1;

#if RTP_RAW_TEST
    rtp_raw_test();
#endif

    tp_dev.init();

    pulse_io_init();

    UI_Main_Draw();

    while(1) {
        if (page == 0) {
            u8 act = UI_Main_Scan();
            if (act == CARD_TIMING) {
                page = 1;
                wait_touch_release();
                UI_TimingMenu_SetMeasuring(0);
                UI_TimingMenu_Draw();
            }
            else if (act == CARD_MECHANIC) { /* TODO: mechanics screen   */ }
            else if (act == CARD_SIGNAL)   { /* TODO: signal screen      */ }
            else if (act == CARD_SETTINGS) { /* TODO: settings screen    */ }
            else if ((act & 0xF0) == 0x10) {
                u8 btn = act & 0x0F;
                if (btn == BTN_SWITCH) { /* TODO: switch display mode    */ }
            }
        } else if (page == 1) {
            u8 act = UI_TimingMenu_Scan();
            if (act == TIMING_ACT_BACK) {
                page = 0;
                UI_Main_Draw();
            } else if (act == TIMING_ACT_OK) {
                /* TODO: jump to timing operation page, UI_TimingMenu_GetSelected() gives mode. */
            } else if (act == TIMING_ACT_SWITCH) {
                /* TODO: switch display mode */
            }
        }
    }
}
