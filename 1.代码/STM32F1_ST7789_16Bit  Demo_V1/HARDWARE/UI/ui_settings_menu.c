#include "ui_settings_menu.h"

#include "ui_main.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"

#define ITEM_X       0
#define ITEM_Y       40
#define ITEM_W       320
#define ITEM_H       26
#define ITEM_R       6

#define DOT_X        18
#define DOT_R        5
#define LABEL_X      34

#define INFO_Y       72
#define BTN_Y        204
#define BTN_H        32
#define BTN_R        13

static const u16 btn_x[5] = {4, 64, 124, 184, 244};
static const u16 btn_w[5] = {54, 54, 54, 54, 72};

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

static void draw_dot(u16 x, u16 y)
{
    POINT_COLOR = UI_TEXT;
    LCD_Draw_Circle(x, y, DOT_R);
    LCD_Draw_Circle(x, y, DOT_R - 1);
    gui_fill_circle(x, y, DOT_R - 2, UI_TEXT);
}

static void draw_titlebar(void)
{
    LCD_Fill(0, TITLEBAR_Y, 319, TITLEBAR_Y + TITLEBAR_H - 1, UI_TITLEBAR);
    POINT_COLOR = WHITE;
    BACK_COLOR = UI_TITLEBAR;

    Show_Str(10, 10, (u8*)"\xCF\xB5\xCD\xB3\xC9\xE8\xD6\xC3\xD3\xEB\xB9\xA4\xBE\xDF", 16, 0);
    Show_Str(146, 10, (u8*)"\xB2\xCB\xB5\xA5\xD1\xA1\xD4\xF1", 16, 0);
    gui_fill_circle(214, 18, 1, WHITE);
    Show_Str(220, 10, (u8*)"\xB4\xFD\xBB\xFA", 16, 0);
}

static void draw_item(void)
{
    fill_round_rect(ITEM_X, ITEM_Y, ITEM_W, ITEM_H, ITEM_R, UI_CARD_PRESS);
    draw_dot(DOT_X, ITEM_Y + ITEM_H / 2);

    POINT_COLOR = UI_TEXT;
    BACK_COLOR = UI_CARD_PRESS;
    Show_Str(LABEL_X, ITEM_Y + 5, (u8*)"\xC9\xE8\xB1\xB8\xD7\xD4\xBC\xEC", 16, 0);
}

static void draw_info(void)
{
    POINT_COLOR = UI_SUBTEXT;
    BACK_COLOR = UI_BG;

    LCD_ShowString(8, INFO_Y, 80, 16, 16, (u8*)"J0201-LCM");
    Show_Str(88, INFO_Y, (u8*)"\xB4\xE6\xD6\xFC\xCA\xBD\xCA\xFD\xD7\xD6\xBA\xC1\xC3\xEB\xBC\xC6", 16, 0);

    Show_Str(8, INFO_Y + 18, (u8*)"\xB9\xCC\xBC\xFE\xB0\xE6\xB1\xBE", 16, 0);
    LCD_ShowString(72, INFO_Y + 18, 64, 16, 16, (u8*)": V1.0");

    Show_Str(8, INFO_Y + 36, (u8*)"\xB5\xE7\xD4\xB4", 16, 0);
    LCD_ShowString(40, INFO_Y + 36, 260, 16, 16, (u8*)": A.C. 220V(+/-10%),50Hz(+/-5%)");

    Show_Str(8, INFO_Y + 54, (u8*)"\xBB\xB7\xBE\xB3\xCE\xC2\xB6\xC8", 16, 0);
    LCD_ShowString(72, INFO_Y + 54, 90, 16, 16, (u8*)": -10C~+40C");
    Show_Str(168, INFO_Y + 54, (u8*)"\xCF\xE0\xB6\xD4\xCA\xAA\xB6\xC8", 16, 0);
    LCD_ShowString(232, INFO_Y + 54, 84, 16, 16, (u8*)": <=85%");

    Show_Str(8, INFO_Y + 72, (u8*)"\xB9\xA4\xD7\xF7\xCA\xB1\xB3\xA4", 16, 0);
    LCD_ShowString(72, INFO_Y + 72, 16, 16, 16, (u8*)":");
    Show_Str(84, INFO_Y + 72, (u8*)"\xD6\xA7\xB3\xD6\xC1\xAC\xD0\xF8\xB9\xA4\xD7\xF7", 16, 0);

    Show_Str(8, INFO_Y + 90, (u8*)"\xCD\xE2\xD0\xCE\xB3\xDF\xB4\xE7", 16, 0);
    LCD_ShowString(72, INFO_Y + 90, 110, 16, 16, (u8*)": 240mmx280mm");
    Show_Str(188, INFO_Y + 90, (u8*)"\xD5\xFB\xBB\xFA\xD6\xD8\xC1\xBF", 16, 0);
    LCD_ShowString(252, INFO_Y + 90, 64, 16, 16, (u8*)": 1.0kg");
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
    else if (id == BTN_CLEAR || id == BTN_STOP) POINT_COLOR = UI_SUBTEXT;
    else POINT_COLOR = UI_TEXT;

    if (id == BTN_OK)
        LCD_ShowString(x + (w - label_w) / 2, BTN_Y + 4, label_w, 24, 24, (u8*)btn_label[id]);
    else
        Show_Str(x + (w - label_w) / 2, BTN_Y + 8, (u8*)btn_label[id], 16, 0);
}

void UI_SettingsMenu_Draw(void)
{
    u8 i;

    draw_titlebar();
    LCD_Fill(0, 36, 319, 199, UI_BG);
    LCD_Fill(0, 200, 319, 239, UI_BG);
    draw_item();
    draw_info();

    for (i = 0; i < 5; i++)
        draw_one_btn(i, UI_CARD);
}

u8 UI_SettingsMenu_GetSelected(void)
{
    return SETTINGS_ITEM_SELFTEST;
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

u8 UI_SettingsMenu_Scan(void)
{
    static u8 touch_ready = 0;
    u8 btn;
    u16 tx, ty;

    tp_dev.scan(0);
    if (!(tp_dev.sta & TP_PRES_DOWN)) {
        touch_ready = 1;
        return SETTINGS_ACT_NONE;
    }

    if (!touch_ready) return SETTINGS_ACT_NONE;
    touch_ready = 0;

    tx = tp_dev.x[0];
    ty = tp_dev.y[0];
    delay_ms(20);

    if (tx < ITEM_W && ty >= ITEM_Y && ty < ITEM_Y + ITEM_H)
        return SETTINGS_ITEM_SELFTEST;

    btn = hit_btn(tx, ty);
    if (btn != BTN_NONE) {
        if (btn == BTN_OK)
            draw_one_btn(btn, UI_OK_PRESS);
        else
            draw_one_btn(btn, UI_BTN_PRESS);
        delay_ms(120);
        draw_one_btn(btn, UI_CARD);

        if (btn == BTN_CLEAR) return SETTINGS_ACT_CLEAR;
        if (btn == BTN_BACK) return SETTINGS_ACT_BACK;
        if (btn == BTN_STOP) return SETTINGS_ACT_STOP;
        if (btn == BTN_SWITCH) return SETTINGS_ACT_SWITCH;
        if (btn == BTN_OK) return SETTINGS_ACT_OK;
    }

    return SETTINGS_ACT_NONE;
}
