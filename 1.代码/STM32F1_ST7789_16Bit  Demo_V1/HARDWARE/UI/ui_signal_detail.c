#include "ui_signal_detail.h"

#include "ui_main.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"

#define CONTENT_Y    36
#define CONTENT_H    164
#define BTN_Y        204
#define BTN_H        32
#define BTN_R        13

typedef struct {
    u8 period_index;
    u8 output_on;
} SignalDetailState;

static SignalDetailState g_signal;

static const u16 btn_x[5] = {4, 64, 124, 184, 244};
static const u16 btn_w[5] = {54, 54, 54, 54, 72};
static const u8 * const period_label[5] = {
    (u8*)"0.1 ms",
    (u8*)"1 ms",
    (u8*)"10 ms",
    (u8*)"100 ms",
    (u8*)"1 s"
};
static const u8 * const btn_label[5] = {
    (u8*)"\xC7\xE5\xC1\xE3",
    (u8*)"\xB7\xB5\xBB\xD8",
    (u8*)"\xCD\xA3\xD6\xB9",
    (u8*)"\xC7\xD0\xBB\xBB",
    (u8*)"OK"
};

static void fill_round_rect(u16 x, u16 y, u16 w, u16 h, u8 r, u16 color)
{
    LCD_Fill(x + r, y, x + w - r - 1, y + h - 1, color);
    LCD_Fill(x, y + r, x + w - 1, y + h - r - 1, color);
    gui_fill_circle(x + r, y + r, r, color);
    gui_fill_circle(x + w - r - 1, y + r, r, color);
    gui_fill_circle(x + r, y + h - r - 1, r, color);
    gui_fill_circle(x + w - r - 1, y + h - r - 1, r, color);
}

static void draw_panel(u16 x, u16 y, u16 w, u16 h, u8 r, u16 color)
{
    fill_round_rect(x + 2, y + 2, w, h, r, UI_SHADOW);
    fill_round_rect(x, y, w, h, r, color);
}

static void draw_titlebar(void)
{
    LCD_Fill(0, TITLEBAR_Y, 319, TITLEBAR_Y + TITLEBAR_H - 1, UI_TITLEBAR);
    POINT_COLOR = WHITE;
    BACK_COLOR = UI_TITLEBAR;

    Show_Str(10, 10, (u8*)"\xCA\xB1\xB1\xEA\xCA\xE4\xB3\xF6", 16, 0);
    LCD_ShowString(82, 10, 48, 16, 16, (u8*)"Sgl");
    if (g_signal.output_on)
        Show_Str(174, 10, (u8*)"\xCA\xE4\xB3\xF6\xD6\xD0", 16, 0);
    else
        Show_Str(190, 10, (u8*)"\xB4\xFD\xBB\xFA", 16, 0);
}

static void draw_content(void)
{
    LCD_Fill(0, CONTENT_Y, 319, CONTENT_Y + CONTENT_H - 1, WHITE);

    POINT_COLOR = UI_SUBTEXT;
    BACK_COLOR = WHITE;
    Show_Str(112, 58, (u8*)"\xB5\xB1\xC7\xB0\xCA\xB1\xB1\xEA\xD6\xDC\xC6\xDA", 16, 0);

    POINT_COLOR = UI_TEXT;
    LCD_ShowString(110, 96, 120, 24, 24, (u8*)period_label[g_signal.period_index]);

    POINT_COLOR = UI_SUBTEXT;
    Show_Str(92, 146, (u8*)"\xCA\xE4\xB3\xF6\xD7\xB4\xCC\xAC", 16, 0);
    LCD_ShowChar(156, 146, ':', 16, 0);
    if (g_signal.output_on)
        Show_Str(168, 146, (u8*)"\xBF\xAA\xC6\xF4", 16, 0);
    else
        Show_Str(168, 146, (u8*)"\xB9\xD8\xB1\xD5", 16, 0);
}

static void draw_one_btn(u8 id, u16 bg)
{
    u16 x = btn_x[id];
    u16 w = btn_w[id];
    u16 label_w = (id == BTN_OK) ? 24 : 32;

    if (id == BTN_OK)
        fill_round_rect(x, BTN_Y, w, BTN_H, BTN_R, UI_ORANGE);
    else
        draw_panel(x, BTN_Y, w, BTN_H, BTN_R, bg);

    BACK_COLOR = (id == BTN_OK) ? UI_ORANGE : bg;
    if (id == BTN_CLEAR) POINT_COLOR = UI_RED_TEXT;
    else if (id == BTN_OK) POINT_COLOR = WHITE;
    else POINT_COLOR = UI_TEXT;

    if (id == BTN_OK)
        LCD_ShowString(x + (w - label_w) / 2, BTN_Y + 4, label_w, 24, 24, (u8*)btn_label[id]);
    else
        Show_Str(x + (w - label_w) / 2, BTN_Y + 8, (u8*)btn_label[id], 16, 0);
}

static void draw_all(void)
{
    u8 i;

    draw_titlebar();
    draw_content();
    LCD_Fill(0, 200, 319, 239, UI_BG);
    for (i = 0; i < 5; i++)
        draw_one_btn(i, UI_CARD);
}

void UI_SignalDetail_Open(void)
{
    g_signal.period_index = 1;
    g_signal.output_on = 0;
    draw_all();
}

static u8 hit_btn(u16 tx, u16 ty)
{
    u8 i;
    if (ty < BTN_Y || ty >= BTN_Y + BTN_H) return BTN_NONE;
    for (i = 0; i < 5; i++) {
        if (tx >= btn_x[i] && tx < btn_x[i] + btn_w[i])
            return i;
    }
    return BTN_NONE;
}

static void signal_detail_clear(void)
{
    g_signal.period_index = 1;
    g_signal.output_on = 0;
    draw_titlebar();
    draw_content();
}

static void signal_detail_switch(void)
{
    g_signal.period_index++;
    if (g_signal.period_index >= 5)
        g_signal.period_index = 0;
    draw_content();
}

u8 UI_SignalDetail_Scan(void)
{
    static u8 touch_ready = 0;
    u8 btn;
    u16 tx, ty;

    tp_dev.scan(0);
    if (!(tp_dev.sta & TP_PRES_DOWN)) {
        touch_ready = 1;
        return SIGNAL_DETAIL_ACT_NONE;
    }

    if (!touch_ready) return SIGNAL_DETAIL_ACT_NONE;
    touch_ready = 0;

    tx = tp_dev.x[0];
    ty = tp_dev.y[0];
    delay_ms(20);

    btn = hit_btn(tx, ty);
    if (btn == BTN_NONE) return SIGNAL_DETAIL_ACT_NONE;

    if (btn == BTN_OK)
        draw_one_btn(btn, UI_ORANGE);
    else
        draw_one_btn(btn, UI_CARD_PRESS);
    delay_ms(120);
    draw_one_btn(btn, UI_CARD);

    if (btn == BTN_CLEAR) {
        signal_detail_clear();
    } else if (btn == BTN_BACK) {
        return SIGNAL_DETAIL_ACT_BACK;
    } else if (btn == BTN_STOP) {
        g_signal.output_on = 0;
        draw_titlebar();
        draw_content();
    } else if (btn == BTN_SWITCH) {
        signal_detail_switch();
    } else if (btn == BTN_OK) {
        g_signal.output_on = 1;
        draw_titlebar();
        draw_content();
    }

    return SIGNAL_DETAIL_ACT_NONE;
}
