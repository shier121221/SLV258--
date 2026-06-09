#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "touch.h"
#include "test code.h"

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
// PB0, PB1, PB15 = digital input

// PA7: trigger pulse output (to external device, 10ms HIGH)
// PB0, PB1, PB15: photogate inputs (NPN: no object=HIGH, object=LOW)
static void pulse_io_init(void)
{
    RCC->APB2ENR |= (1<<2)|(1<<3);

    // PA7 output push-pull, idle LOW
    GPIOA->CRL &= ~(0xFu << 28);
    GPIOA->CRL |=  (0x3u << 28);
    GPIOA->BRR  =  (1<<7);

    // PB0, PB1 input pull-up
    GPIOB->CRL &= ~(0xFFu << 0);
    GPIOB->CRL |=  (0x88u << 0);
    GPIOB->ODR |=  (1<<0)|(1<<1);
    // PB15 already input pull-up from touch init
}

static void pulse_test(void)
{
    u8 pb0, pb1, pb15;

    LCD_Clear(WHITE);
    POINT_COLOR = BLACK;
    LCD_ShowString(10, 10, 220, 16, 16, (u8*)"PA7: 10ms pulse every 500ms");
    LCD_ShowString(10, 30, 220, 16, 16, (u8*)"Photogate: LOW=blocked");

    while(1)
    {
        // send 10ms trigger pulse on PA7
        GPIOA->BSRR = (1<<7);
        delay_ms(10);
        GPIOA->BRR  = (1<<7);

        // read photogate states
        pb0  = (GPIOB->IDR >> 0)  & 1;
        pb1  = (GPIOB->IDR >> 1)  & 1;
        pb15 = (GPIOB->IDR >> 15) & 1;

        POINT_COLOR = pb0  ? BLACK : RED;
        LCD_ShowString(10, 80,  200, 16, 16, (u8*)(pb0  ? "PB0 : clear  " : "PB0 : BLOCKED"));
        POINT_COLOR = pb1  ? BLACK : RED;
        LCD_ShowString(10, 110, 200, 16, 16, (u8*)(pb1  ? "PB1 : clear  " : "PB1 : BLOCKED"));
        POINT_COLOR = pb15 ? BLACK : RED;
        LCD_ShowString(10, 140, 200, 16, 16, (u8*)(pb15 ? "PB15: clear  " : "PB15: BLOCKED"));

        delay_ms(490); // total loop ~500ms
    }
}

int main(void)
{
    Stm32_Clock_Init(9);
    delay_init(72);

    LCD_Init();
    tp_dev.touchtype |= USE_TP_TYPE;
    LCD_Display_Dir(USE_LCM_DIR);
    LCD_Clear(WHITE);
    LCD_LED = 1;

    pulse_io_init();

    main_test("IC:ST7789V");
    Color_Test();
    Read_Test();
    FillRec_Test();
    English_Font_test();
    Chinese_Font_test();
    Rotate_Test();

    pulse_test();
}
