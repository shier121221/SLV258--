#include "ui_timing_menu.h"
#include "ui_main.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"

#define LIST_X       0
#define LIST_Y       40
#define LIST_W       320
#define LIST_H       30
#define LIST_GAP     4
#define LIST_R       6

#define DOT_X        18
#define DOT_R        5
#define LABEL_X      34

#define TIMING_LIST_AREA_Y 36
#define TIMING_BTNBAR_Y    180

#define BTN_Y        184
#define BTN_W        62
#define BTN_H        52
#define BTN_GAP      3
#define BTN_R        6

#define UI_BTN_PRESS 0xC618
#define UI_BTN_GRAY  0xDEFB

static const u16 btn_x[5] = {0, 65, 130, 195, 258};
static u8 selected_item = TIMING_ITEM_S1;
static u8 timing_measuring = 0;

static const u8 * const item_label[4] = {
    (u8*)"\xD5\xDA\xB9\xE2\xBC\xC6\xCA\xB1", /* 遮光计时 */
    (u8*)"\xBC\xE4\xB8\xF4\xBC\xC6\xCA\xB1", /* 间隔计时 */
    (u8*)"\xBC\xC6\xCA\xFD\xC4\xA3\xCA\xBD", /* 计数模式 */
    (u8*)"\xCD\xA8\xD3\xC3\xBC\xC6\xCA\xB1"  /* 通用计时 */
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

static void draw_dot(u16 x, u16 y, u8 checked)
{
    POINT_COLOR = UI_TEXT;
    LCD_Draw_Circle(x, y, DOT_R);
    LCD_Draw_Circle(x, y, DOT_R - 1);
    if (checked)
        gui_fill_circle(x, y, DOT_R - 2, UI_TEXT);
}

static void draw_titlebar(void)
{
    LCD_Fill(0, TITLEBAR_Y, 319, TITLEBAR_Y + TITLEBAR_H - 1, UI_TITLEBAR);
    POINT_COLOR = WHITE;
    BACK_COLOR = UI_TITLEBAR;

    Show_Str(10, 10, (u8*)"\xBB\xF9\xB4\xA1\xBC\xC6\xCA\xB1\xC0\xE0", 16, 0);
    Show_Str(110, 10, (u8*)"\xB2\xCB\xB5\xA5\xD1\xA1\xD4\xF1", 16, 0);
    gui_fill_circle(178, 18, 1, WHITE);
    if (timing_measuring)
        Show_Str(184, 10, (u8*)"\xB2\xE2\xC1\xBF\xD6\xD0", 16, 0);
    else
        Show_Str(184, 10, (u8*)"\xB4\xFD\xBB\xFA", 16, 0);
}

static void draw_one_item(u8 id)
{
    u16 y = LIST_Y + id * (LIST_H + LIST_GAP);
    u16 bg = (id == selected_item) ? UI_CARD_PRESS : UI_CARD;

    fill_round_rect(LIST_X, y, LIST_W, LIST_H, LIST_R, bg);
    draw_dot(DOT_X, y + LIST_H / 2, id == selected_item);

    POINT_COLOR = UI_TEXT;
    BACK_COLOR = bg;
    Show_Str(LABEL_X, y + 7, (u8*)item_label[id], 16, 0);
}

static void draw_one_btn(u8 id, u16 bg)
{
    u16 x = btn_x[id];
    u16 label_w = (id == BTN_OK) ? 24 : 32;

    fill_round_rect(x, BTN_Y, BTN_W, BTN_H, BTN_R, bg);

    BACK_COLOR = bg;
    if (id == BTN_CLEAR) POINT_COLOR = UI_RED_TEXT;
    else if (id == BTN_OK) POINT_COLOR = UI_ORANGE_TEXT;
    else if (id == BTN_STOP) POINT_COLOR = UI_SUBTEXT;
    else POINT_COLOR = UI_TEXT;

    if (id == BTN_OK)
        LCD_ShowString(x + (BTN_W - label_w) / 2, BTN_Y + 14, label_w, 16, 16, (u8*)btn_label[id]);
    else
        Show_Str(x + (BTN_W - label_w) / 2, BTN_Y + 18, (u8*)btn_label[id], 16, 0);
}

void UI_TimingMenu_Draw(void)
{
    u8 i;

    draw_titlebar();
    LCD_Fill(0, TIMING_LIST_AREA_Y, 319, TIMING_BTNBAR_Y - 1, UI_BG);
    LCD_Fill(0, TIMING_BTNBAR_Y, 319, 239, UI_BG);

    for (i = 0; i < 4; i++)
        draw_one_item(i);

    for (i = 0; i < 5; i++) {
        if (i == BTN_CLEAR || i == BTN_STOP)
            draw_one_btn(i, UI_BTN_GRAY);
        else
            draw_one_btn(i, UI_CARD);
    }
}

void UI_TimingMenu_SetMeasuring(u8 measuring)
{
    timing_measuring = measuring ? 1 : 0;
    draw_titlebar();
}

u8 UI_TimingMenu_GetSelected(void)
{
    return selected_item;
}

static u8 hit_item(u16 tx, u16 ty)
{
    u8 i;
    for (i = 0; i < 4; i++) {
        u16 y = LIST_Y + i * (LIST_H + LIST_GAP);
        if (tx < LIST_W && ty >= y && ty < y + LIST_H)
            return i;
    }
    return TIMING_ITEM_NONE;
}

static u8 hit_btn(u16 tx, u16 ty)
{
    u8 i;
    if (ty < BTN_Y || ty >= BTN_Y + BTN_H) return BTN_NONE;
    for (i = 0; i < 5; i++) {
        if (tx >= btn_x[i] && tx < btn_x[i] + BTN_W)
            return i;
    }
    return BTN_NONE;
}

u8 UI_TimingMenu_Scan(void)
{
    static u8 touch_ready = 0;
    u8 item, btn;
    u16 tx, ty;

    tp_dev.scan(0);
    if (!(tp_dev.sta & TP_PRES_DOWN)) {
        touch_ready = 1;
        return TIMING_ACT_NONE;
    }

    if (!touch_ready) return TIMING_ACT_NONE;
    touch_ready = 0;

    tx = tp_dev.x[0];
    ty = tp_dev.y[0];
    delay_ms(20);

    item = hit_item(tx, ty);
    if (item != TIMING_ITEM_NONE) {
        if (item != selected_item) {
            u8 old = selected_item;
            selected_item = item;
            draw_one_item(old);
            draw_one_item(selected_item);
        }
        return item;
    }

    btn = hit_btn(tx, ty);
    if (btn == BTN_CLEAR || btn == BTN_STOP)
        return TIMING_ACT_NONE;

    if (btn != BTN_NONE) {
        draw_one_btn(btn, UI_BTN_PRESS);
        delay_ms(80);
        draw_one_btn(btn, UI_CARD);

        if (btn == BTN_BACK) return TIMING_ACT_BACK;
        if (btn == BTN_SWITCH) return TIMING_ACT_SWITCH;
        if (btn == BTN_OK) return TIMING_ACT_OK;
    }

    return TIMING_ACT_NONE;
}
