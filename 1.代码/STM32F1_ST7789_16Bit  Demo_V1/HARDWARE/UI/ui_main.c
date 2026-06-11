#include "ui_main.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"

#define CARD_W      145
#define CARD_H      70
#define CARD_R      14
#define CARD_TOP    46
#define CARD_GAP_X  10
#define CARD_GAP_Y  8

#define BTN_H       32
#define BTN_Y       204
#define BTN_R       13

static const u16 card_x[4] = {10, 165, 10, 165};
static const u16 card_y[4] = {
    CARD_TOP,
    CARD_TOP,
    CARD_TOP + CARD_H + CARD_GAP_Y,
    CARD_TOP + CARD_H + CARD_GAP_Y
};

static const u16 btn_x[5] = {4, 64, 124, 184, 244};
static const u16 btn_w[5] = {54, 54, 54, 54, 72};

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

static void draw_thick_line(u16 x1, u16 y1, u16 x2, u16 y2, u8 size, u16 color)
{
    lcd_draw_bline(x1, y1, x2, y2, size, color);
}

static void draw_ring(u16 x, u16 y, u8 r, u16 color)
{
    POINT_COLOR = color;
    LCD_Draw_Circle(x, y, r);
    LCD_Draw_Circle(x, y, r - 1);
    LCD_Draw_Circle(x, y, r - 2);
}

static void draw_icon_clock(u16 x, u16 y, u16 color)
{
    draw_ring(x + 18, y + 18, 16, color);
    draw_thick_line(x + 18, y + 8, x + 18, y + 18, 1, color);
    draw_thick_line(x + 19, y + 18, x + 27, y + 12, 1, color);
    gui_fill_circle(x + 18, y + 18, 3, color);

    draw_thick_line(x + 18, y + 4, x + 18, y + 6, 1, color);
    draw_thick_line(x + 18, y + 30, x + 18, y + 32, 1, color);
    draw_thick_line(x + 4, y + 18, x + 6, y + 18, 1, color);
    draw_thick_line(x + 30, y + 18, x + 32, y + 18, 1, color);
}

static void draw_icon_pendulum(u16 x, u16 y, u16 color)
{
    draw_thick_line(x + 24, y + 7, x + 24, y + 32, 2, color);
    draw_thick_line(x + 12, y + 31, x + 35, y + 31, 2, color);
    draw_thick_line(x + 24, y + 8, x + 12, y + 26, 2, color);
    draw_thick_line(x + 22, y + 10, x + 16, y + 25, 1, color);
    draw_thick_line(x + 25, y + 7, x + 33, y + 7, 1, color);
    gui_fill_circle(x + 12, y + 26, 6, color);
    gui_fill_circle(x + 24, y + 7, 5, color);
    gui_fill_circle(x + 24, y + 7, 2, BACK_COLOR);
}

static void draw_icon_wave(u16 x, u16 y, u16 color)
{
    draw_thick_line(x + 0, y + 20, x + 7, y + 20, 1, color);
    draw_thick_line(x + 7, y + 20, x + 11, y + 12, 2, color);
    draw_thick_line(x + 11, y + 12, x + 16, y + 29, 2, color);
    draw_thick_line(x + 16, y + 29, x + 22, y + 6, 2, color);
    draw_thick_line(x + 22, y + 6, x + 28, y + 29, 2, color);
    draw_thick_line(x + 28, y + 29, x + 33, y + 12, 2, color);
    draw_thick_line(x + 33, y + 12, x + 38, y + 20, 2, color);
    draw_thick_line(x + 38, y + 20, x + 45, y + 20, 1, color);
}

static void draw_icon_gear(u16 x, u16 y, u16 color)
{
    gui_fill_circle(x + 18, y + 18, 14, color);
    LCD_Fill(x + 15, y + 0, x + 21, y + 9, color);
    LCD_Fill(x + 15, y + 27, x + 21, y + 36, color);
    LCD_Fill(x + 0, y + 15, x + 9, y + 21, color);
    LCD_Fill(x + 27, y + 15, x + 36, y + 21, color);
    draw_thick_line(x + 6, y + 6, x + 12, y + 12, 3, color);
    draw_thick_line(x + 30, y + 6, x + 24, y + 12, 3, color);
    draw_thick_line(x + 6, y + 30, x + 12, y + 24, 3, color);
    draw_thick_line(x + 30, y + 30, x + 24, y + 24, 3, color);
    gui_fill_circle(x + 18, y + 18, 7, BACK_COLOR);
}

typedef void (*icon_fn)(u16, u16, u16);
static const icon_fn icons[4] = {
    draw_icon_clock,
    draw_icon_pendulum,
    draw_icon_wave,
    draw_icon_gear
};

static const u8 * const card_title[4] = {
    (u8*)"\xBB\xF9\xB4\xA1\xBC\xC6\xCA\xB1\xC0\xE0",        /* basic timing */
    (u8*)"\xC1\xA6\xD1\xA7\xCA\xB5\xD1\xE9\xD7\xA8\xCF\xEE",/* mechanics */
    (u8*)"\xD0\xC5\xBA\xC5\xD3\xEB\xCA\xB1\xB1\xEA",        /* signal */
    (u8*)"\xCF\xB5\xCD\xB3\xC9\xE8\xD6\xC3\xB9\xA4\xBE\xDF" /* settings */
};

static const u8 card_chars[4] = {5, 6, 5, 6};

static const u8 * const btn_label[5] = {
    (u8*)"\xC7\xE5\xC1\xE3",
    (u8*)"\xB7\xB5\xBB\xD8",
    (u8*)"\xCD\xA3\xD6\xB9",
    (u8*)"\xC7\xD0\xBB\xBB",
    (u8*)"OK"
};

static void draw_titlebar(void)
{
    LCD_Fill(0, TITLEBAR_Y, 319, TITLEBAR_Y + TITLEBAR_H - 1, UI_TITLEBAR);
    POINT_COLOR = WHITE;
    BACK_COLOR = UI_TITLEBAR;

    LCD_ShowString(8, 6, 120, 24, 24, (u8*)"J0201-LCM");
    Show_Str(140, 10, (u8*)"\xD6\xF7\xB2\xCB\xB5\xA5", 16, 0);
    LCD_ShowString(188, 10, 8, 16, 16, (u8*)"-");
    Show_Str(198, 10, (u8*)"\xB4\xFD\xBB\xFA", 16, 0);

}

static void draw_one_card(u8 id, u16 bg)
{
    u16 x = card_x[id];
    u16 y = card_y[id];
    u16 tx = x + (CARD_W - (u16)card_chars[id] * 16) / 2;
    u16 fg = (bg == UI_TITLEBAR) ? WHITE : UI_TEXT;
    u16 icon_color = (bg == UI_TITLEBAR) ? WHITE : UI_TITLEBAR;

    draw_panel(x, y, CARD_W, CARD_H, CARD_R, bg);

    POINT_COLOR = fg;
    BACK_COLOR = bg;
    Show_Str(tx, y + 8, (u8*)card_title[id], 16, 0);

    BACK_COLOR = bg;
    icons[id](x + (CARD_W - 45) / 2, y + 28, icon_color);
}

static void draw_one_btn(u8 id, u16 bg)
{
    u16 x = btn_x[id];
    u16 w = btn_w[id];
    u16 label_w = (id == BTN_OK) ? 24 : 32;
    u16 fill = (id == BTN_OK) ? ((bg == UI_OK_PRESS) ? UI_OK_PRESS : UI_ORANGE) : bg;
    u8 pressed = (bg == UI_BTN_PRESS) || (bg == UI_OK_PRESS);

    if (id == BTN_OK)
        fill_round_rect(x, BTN_Y, w, BTN_H, BTN_R, fill);
    else
        draw_panel(x, BTN_Y, w, BTN_H, BTN_R, fill);

    BACK_COLOR = fill;
    if (pressed || id == BTN_OK) POINT_COLOR = WHITE;
    else if (id == BTN_CLEAR) POINT_COLOR = UI_RED_TEXT;
    else POINT_COLOR = UI_TEXT;

    if (id == BTN_OK)
        LCD_ShowString(x + (w - label_w) / 2, BTN_Y + 4, label_w, 24, 24, (u8*)btn_label[id]);
    else
        Show_Str(x + (w - label_w) / 2, BTN_Y + 8, (u8*)btn_label[id], 16, 0);
}

void UI_Main_Draw(void)
{
    u8 i;

    draw_titlebar();
    LCD_Fill(0, CARDS_Y, 319, 239, UI_BG);

    for (i = 0; i < 4; i++)
        draw_one_card(i, UI_CARD);

    for (i = 0; i < 5; i++)
        draw_one_btn(i, UI_CARD);
}

static u8 hit_card(u16 tx, u16 ty)
{
    if (ty >= 38 && ty < 124) {
        if (tx < 160) return CARD_TIMING;
        if (tx < 320) return CARD_MECHANIC;
    }
    if (ty >= 124 && ty < 200) {
        if (tx < 160) return CARD_SIGNAL;
        if (tx < 320) return CARD_SETTINGS;
    }
    return CARD_NONE;
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

u8 UI_Main_Scan(void)
{
    static u8 touch_ready = 0;
    u8 card, btn;
    u16 tx, ty;

    tp_dev.scan(0);
    if (!(tp_dev.sta & TP_PRES_DOWN)) {
        touch_ready = 1;
        return 0xFF;
    }

    if (!touch_ready) return 0xFF;
    touch_ready = 0;

    tx = tp_dev.x[0];
    ty = tp_dev.y[0];
    delay_ms(20);

    card = hit_card(tx, ty);
    if (card != CARD_NONE) {
        draw_one_card(card, UI_TITLEBAR);
        delay_ms(120);
        return card;
    }

    btn = hit_btn(tx, ty);
    if (btn != BTN_NONE) {
        if (btn == BTN_OK)
            draw_one_btn(btn, UI_OK_PRESS);
        else
            draw_one_btn(btn, UI_BTN_PRESS);
        delay_ms(120);
        draw_one_btn(btn, UI_CARD);
        return 0x10 | btn;
    }

    return 0xFF;
}
