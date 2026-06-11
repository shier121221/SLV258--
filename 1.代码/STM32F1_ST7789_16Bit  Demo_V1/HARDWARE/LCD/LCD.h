#ifndef __LCD_H
#define __LCD_H

#include "sys.h"

// Display rotation:
// 0 = 0 deg, 1 = 180 deg, 2 = 270 deg, 3 = 90 deg
#define USE_LCM_DIR       3

// Touch type selection
#define USE_TP_TYPE       RTP
#define CTP               0x80
#define RTP               0x00

// LCD runtime parameters
typedef struct
{
    u16 width;      // Display width
    u16 height;     // Display height
    u16 id;         // LCD controller ID
    u8  dir;        // 0 = portrait, 1 = landscape
    u16 wramcmd;    // GRAM write command
    u16 setxcmd;    // Column address command
    u16 setycmd;    // Row address command
}_lcd_dev;

extern _lcd_dev lcddev;   // Global LCD state
extern u16 POINT_COLOR;   // Current drawing color
extern u16 BACK_COLOR;    // Current background color

// LCD control pins
#define LCD_RST_SET  GPIOA->BSRR=1<<11    // Reset high
#define LCD_CS_SET                        // Chip select high (CS tied low)
#define LCD_RS_SET   GPIOA->BSRR=1<<8     // Data / command select high
#define LCD_WR_SET   GPIOA->BSRR=1<<9     // Write strobe high
#define LCD_RD_SET   GPIOA->BSRR=1<<10    // Read strobe high

#define LCD_RST_CLR  GPIOA->BRR =1<<11    // Reset low
#define LCD_CS_CLR                        // Chip select low (CS tied low)
#define LCD_RS_CLR   GPIOA->BRR =1<<8     // Data / command select low
#define LCD_WR_CLR   GPIOA->BRR =1<<9     // Write strobe low
#define LCD_RD_CLR   GPIOA->BRR =1<<10    // Read strobe low

#define LCD_LED      PCout(14)            // Backlight enable

// DB[6:0]=PA[6:0], DB7=PA12, DB[15:8]=PB[10:3]
#define DATAOUT(x) do { \
    GPIOA->ODR = (GPIOA->ODR & 0xEF80u) | ((u16)(x)&0x7Fu) | (((u16)(x)&0x80u)<<5); \
    GPIOB->ODR = (GPIOB->ODR & 0xF807u) | (((u16)(x)&0xFF00u)>>5); \
} while(0)

#define DATAIN ((GPIOA->IDR & 0x7Fu) | ((GPIOA->IDR>>5)&0x80u) | ((GPIOB->IDR<<5)&0xFF00u))

// RGB565 colors
#define WHITE         0xFFFF
#define BLACK         0x0000
#define BLUE          0x001F
#define BRED          0xF81F
#define GRED          0xFFE0
#define GBLUE         0x07FF
#define RED           0xF800
#define MAGENTA       0xF81F
#define GREEN         0x07E0
#define CYAN          0x7FFF
#define YELLOW        0xFFE0
#define BROWN         0xBC40   // Brown
#define BRRED         0xFC07   // Brown-red
#define GRAY          0x8430   // Gray

// GUI helper colors
#define DARKBLUE      0x01CF   // Dark blue
#define LIGHTBLUE     0x7D7C   // Light blue
#define GRAYBLUE      0x5458   // Gray-blue
#define LIGHTGREEN    0x841F   // Light green
#define LGRAY         0xC618   // Light gray, panel background
#define LGRAYBLUE     0xA651   // Light gray-blue
#define LBBLUE        0x2B12   // Light blue for selected items

void LCD_Init(void);                                                     // Initialize LCD controller
void LCD_DisplayOn(void);                                                // Turn display on
void LCD_DisplayOff(void);                                               // Turn display off
void LCD_Clear(u16 Color);                                               // Clear full screen
void LCD_SetCursor(u16 Xpos, u16 Ypos);                                  // Set GRAM cursor
void LCD_DrawPoint(u16 x,u16 y);                                         // Draw one pixel with POINT_COLOR
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);                          // Draw one pixel with explicit color
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);                               // Draw a circle outline
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);                       // Draw a line
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);                  // Draw a rectangle outline
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);                    // Fill a solid rectangle
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);             // Fill with a color buffer
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);                   // Draw one ASCII character
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);                    // Draw a decimal number
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);           // Draw a number with format options
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);     // Draw an ASCII string

void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
void Load_Drow_Dialog(void);
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);                 // Set an address window
void Show_Str(u16 x, u16 y,u8 *str,u8 size,u8 mode);                     // Draw mixed Chinese / ASCII text
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p);                  // Draw a 40x40 bitmap
void Gui_StrCenter(u16 x, u16 y,u8 *str,u8 size,u8 mode);                // Draw centered text
void LCD_Display_Dir(u8 dir);                                            // Set display rotation
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color);   // Draw a thick line
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color);                     // Draw a filled circle
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);                    // Draw a horizontal line
u16 LCD_Read_ID(u8 reg);                                                 // Read LCD ID
u16 LCD_ReadPoint(u16 x,u16 y);                                          // Read one pixel color

#endif
