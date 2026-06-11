#include "ui_mechanics_menu.h"

#include "ui_main.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"

#define LIST_X       0
#define LIST_Y       40
#define LIST_W       320
#define LIST_H       26
#define LIST_GAP     4
#define LIST_R       6

#define DOT_X        18
#define DOT_R        5
#define LABEL_X      34

#define MECH_LIST_AREA_Y 36
#define MECH_BTNBAR_Y    200

#define BTN_Y        204
#define BTN_H        32
#define BTN_R        13

static const u16 btn_x[5] = {4, 64, 124, 184, 244};
static const u16 btn_w[5] = {54, 54, 54, 54, 72};
static u8 selected_item = MECH_ITEM_PENDULUM;
static u8 mech_measuring = 0;

static const u8 * const item_label[5] = {
    (u8*)"\xD5\xF1\xD7\xD3\xD6\xDC\xC6\xDA\xB2\xE2\xC1\xBF",
    (u8*)"\xC5\xF6\xD7\xB2\xCA\xB5\xD1\xE9",
    (u8*)"\xD6\xD8\xC1\xA6\xBC\xD3\xCB\xD9\xB6\xC8",
    (u8*)"\xD6\xB1\xCF\xDF\xBC\xD3\xCB\xD9\xB6\xC8",
    (u8*)"\xBD\xC7\xBC\xD3\xCB\xD9\xB6\xC8\xCA\xB5\xD1\xE9"
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

    Show_Str(10, 10, (u8*)"\xC1\xA6\xD1\xA7\xCA\xB5\xD1\xE9\xD7\xA8\xCF\xEE", 16, 0);
    Show_Str(126, 10, (u8*)"\xB2\xCB\xB5\xA5\xD1\xA1\xD4\xF1", 16, 0);
    gui_fill_circle(194, 18, 1, WHITE);
    if (mech_measuring)
        Show_Str(200, 10, (u8*)"\xB2\xE2\xC1\xBF\xD6\xD0", 16, 0);
    else
        Show_Str(200, 10, (u8*)"\xB4\xFD\xBB\xFA", 16, 0);
}

static void draw_one_item(u8 id)
{
    u16 y = LIST_Y + id * (LIST_H + LIST_GAP);
    u16 bg = (id == selected_item) ? UI_CARD_PRESS : UI_CARD;

    fill_round_rect(LIST_X, y, LIST_W, LIST_H, LIST_R, bg);
    draw_dot(DOT_X, y + LIST_H / 2, id == selected_item);

    POINT_COLOR = UI_TEXT;
    BACK_COLOR = bg;
    Show_Str(LABEL_X, y + 5, (u8*)item_label[id], 16, 0);
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
    if (id == BTN_CLEAR || id == BTN_STOP) POINT_COLOR = UI_SUBTEXT;
    else if (id == BTN_OK) POINT_COLOR = WHITE;
    else POINT_COLOR = UI_TEXT;

    if (id == BTN_OK)
        LCD_ShowString(x + (w - label_w) / 2, BTN_Y + 4, label_w, 24, 24, (u8*)btn_label[id]);
    else
        Show_Str(x + (w - label_w) / 2, BTN_Y + 8, (u8*)btn_label[id], 16, 0);
}

void UI_MechanicsMenu_Draw(void)
{
    u8 i;

    draw_titlebar();
    LCD_Fill(0, MECH_LIST_AREA_Y, 319, MECH_BTNBAR_Y - 1, UI_BG);
    LCD_Fill(0, MECH_BTNBAR_Y, 319, 239, UI_BG);

    for (i = 0; i < 5; i++)
        draw_one_item(i);

    for (i = 0; i < 5; i++)
        draw_one_btn(i, UI_CARD);
}

void UI_MechanicsMenu_SetMeasuring(u8 measuring)
{
    mech_measuring = measuring ? 1 : 0;
    draw_titlebar();
}

u8 UI_MechanicsMenu_GetSelected(void)
{
    return selected_item;
}

static u8 hit_item(u16 tx, u16 ty)
{
    u8 i;
    for (i = 0; i < 5; i++) {
        u16 y = LIST_Y + i * (LIST_H + LIST_GAP);
        if (tx < LIST_W && ty >= y && ty < y + LIST_H)
            return i;
    }
    return MECH_ITEM_NONE;
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

u8 UI_MechanicsMenu_Scan(void)
{
    static u8 touch_ready = 0;
    u8 item, btn;
    u16 tx, ty;

    tp_dev.scan(0);
    if (!(tp_dev.sta & TP_PRES_DOWN)) {
        touch_ready = 1;
        return MECH_ACT_NONE;
    }

    if (!touch_ready) return MECH_ACT_NONE;
    touch_ready = 0;

    tx = tp_dev.x[0];
    ty = tp_dev.y[0];
    delay_ms(20);

    item = hit_item(tx, ty);
    if (item != MECH_ITEM_NONE) {
        if (item != selected_item) {
            u8 old = selected_item;
            selected_item = item;
            draw_one_item(old);
            draw_one_item(selected_item);
        }
        return item;
    }

    btn = hit_btn(tx, ty);
    if (btn != BTN_NONE) {
        if (btn == BTN_OK)
            draw_one_btn(btn, UI_ORANGE);
        else
            draw_one_btn(btn, UI_CARD_PRESS);
        delay_ms(120);
        draw_one_btn(btn, UI_CARD);

        if (btn == BTN_CLEAR) return MECH_ACT_CLEAR;
        if (btn == BTN_BACK) return MECH_ACT_BACK;
        if (btn == BTN_STOP) return MECH_ACT_STOP;
        if (btn == BTN_SWITCH) return MECH_ACT_SWITCH;
        if (btn == BTN_OK) return MECH_ACT_OK;
    }

    return MECH_ACT_NONE;
}
