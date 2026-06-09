import re

path = r"e:/YQ Project/SLV258-存贮式数字毫秒计项目/1.代码/STM32F1_ST7789_16Bit  Demo_V1/HARDWARE/LCD/LCD.h"
with open(path, 'rb') as f:
    data = f.read()

# Find start: just before "LCD_RST_SET  GPIOC"
# Find end: after "GPIOB->IDR;   //" line (includes trailing whitespace/tab)
start = data.find(b'LCD_RST_SET  GPIOC->BSRR')
# go back to start of #define line
while start > 0 and data[start-1:start] not in (b'\n', b'\r'):
    start -= 1

end = data.find(b'GPIOB->IDR;', start)
end = data.find(b'\n', end) + 1  # include the newline

print(f"Replacing bytes {start}..{end}")
print("OLD:", repr(data[start:end]))

new_section = (
    b"//-----------------LCD port definitions----------------\r\n"
    b"// RS=PA8, WR=PA9, RD=PA10, RST=PA11, CS=GND(hardwired)\r\n"
    b"// Backlight=PC14\r\n"
    b"// Data bus: DB0~DB6=PA0~PA6, DB7=PA12, DB8~DB15=PB3~PB10\r\n"
    b"\r\n"
    b"#define LCD_RST_SET  GPIOA->BSRR=1<<11\r\n"
    b"#define LCD_RST_CLR  GPIOA->BRR =1<<11\r\n"
    b"#define LCD_RS_SET   GPIOA->BSRR=1<<8\r\n"
    b"#define LCD_RS_CLR   GPIOA->BRR =1<<8\r\n"
    b"#define LCD_WR_SET   GPIOA->BSRR=1<<9\r\n"
    b"#define LCD_WR_CLR   GPIOA->BRR =1<<9\r\n"
    b"#define LCD_RD_SET   GPIOA->BSRR=1<<10\r\n"
    b"#define LCD_RD_CLR   GPIOA->BRR =1<<10\r\n"
    b"// CS is hardwired to GND\r\n"
    b"#define LCD_CS_SET\r\n"
    b"#define LCD_CS_CLR\r\n"
    b"\r\n"
    b"#define LCD_LED PCout(14)  // Backlight PC14\r\n"
    b"\r\n"
    b"// DB[6:0]=PA[6:0], DB7=PA12, DB[15:8]=PB[10:3]\r\n"
    b"#define DATAOUT(x) do { \\\r\n"
    b"    GPIOA->ODR = (GPIOA->ODR & 0xEF80u) | ((u16)(x)&0x7Fu) | (((u16)(x)&0x80u)<<5); \\\r\n"
    b"    GPIOB->ODR = (GPIOB->ODR & 0xF807u) | (((u16)(x)&0xFF00u)>>5); \\\r\n"
    b"} while(0)\r\n"
    b"#define DATAIN ((GPIOA->IDR & 0x7Fu) | ((GPIOA->IDR>>5)&0x80u) | ((GPIOB->IDR<<5)&0xFF00u))\r\n"
)

result = data[:start] + new_section + data[end:]
with open(path, 'wb') as f:
    f.write(result)
print("Done. LCD.h patched.")
