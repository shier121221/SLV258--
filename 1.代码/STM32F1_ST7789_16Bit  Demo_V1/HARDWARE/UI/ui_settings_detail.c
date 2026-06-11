#include "ui_settings_detail.h"

#include "ui_main.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"

#define CONTENT_Y    36
#define CONTENT_H    164
#define BTN_Y        204
#define BTN_H        32
#define BTN_R        13

#define SELFTEST_OK        0
#define SELFTEST_GM1_ERR   1
#define SELFTEST_GM2_ERR   2
#define SELFTEST_STOPPED   3

typedef struct {
    u8 running;
    u8 result;
} SettingsDetailState;

static SettingsDetailState g_settings;

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

static void draw_titlebar(void)
{
    LCD_Fill(0, TITLEBAR_Y, 319, TITLEBAR_Y + TITLEBAR_H - 1, UI_TITLEBAR);
    POINT_COLOR = WHITE;
    BACK_COLOR = UI_TITLEBAR;

    Show_Str(10, 10, (u8*)"\xC9\xE8\xB1\xB8\xD7\xD4\xBC\xEC", 16, 0);
    if (g_settings.running)
        Show_Str(174, 10, (u8*)"\xBC\xEC\xB2\xE2\xD6\xD0", 16, 0);
    else
        Show_Str(166, 10, (u8*)"\xBC\xEC\xB2\xE2\xCD\xEA\xB3\xC9", 16, 0);
}

static u8 run_selftest(void)
{
    return SELFTEST_OK;
}

static void draw_content(void)
{
    LCD_Fill(0, CONTENT_Y, 319, CONTENT_Y + CONTENT_H - 1, WHITE);

    POINT_COLOR = UI_SUBTEXT;
    BACK_COLOR = WHITE;
    Show_Str(68, 58, (u8*)"\xB9\xE2\xB5\xE7\xC3\xC5", 16, 0);
    LCD_ShowString(116, 58, 16, 16, 16, (u8*)"&");
    Show_Str(134, 58, (u8*)"\xD5\xFB\xBB\xFA\xD7\xD4\xBC\xEC", 16, 0);

    POINT_COLOR = (g_settings.result == SELFTEST_OK) ? UI_TEXT : UI_RED_TEXT;
    if (g_settings.result == SELFTEST_GM1_ERR)
        LCD_ShowString(106, 100, 120, 24, 24, (u8*)"GM1 ERROR");
    else if (g_settings.result == SELFTEST_GM2_ERR)
        LCD_ShowString(106, 100, 120, 24, 24, (u8*)"GM2 ERROR");
    else if (g_settings.result == SELFTEST_STOPPED)
        LCD_ShowString(144, 100, 48, 24, 24, (u8*)"--");
    else
        LCD_ShowString(146, 100, 48, 24, 24, (u8*)"OK");

    if (g_settings.result == SELFTEST_GM1_ERR || g_settings.result == SELFTEST_GM2_ERR) {
        POINT_COLOR = UI_RED_TEXT;
        Show_Str(88, 146, (u8*)"\xC7\xEB\xBC\xEC\xB2\xE9\xB4\xAB\xB8\xD0\xC6\xF7\xC1\xAC\xBD\xD3", 16, 0);
    }
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

static void draw_all(void)
{
    u8 i;

    draw_titlebar();
    draw_content();
    LCD_Fill(0, 200, 319, 239, UI_BG);
    for (i = 0; i < 5; i++)
        draw_one_btn(i, UI_CARD);
}

static void start_selftest(void)
{
    g_settings.running = 1;
    draw_titlebar();
    g_settings.result = run_selftest();
    g_settings.running = 0;
    draw_titlebar();
    draw_content();
}

void UI_SettingsDetail_Open(void)
{
    g_settings.running = 0;
    g_settings.result = SELFTEST_OK;
    draw_all();
    start_selftest();
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

u8 UI_SettingsDetail_Scan(void)
{
    static u8 touch_ready = 0;
    u8 btn;
    u16 tx, ty;

    tp_dev.scan(0);
    if (!(tp_dev.sta & TP_PRES_DOWN)) {
        touch_ready = 1;
        return SETTINGS_DETAIL_ACT_NONE;
    }

    if (!touch_ready) return SETTINGS_DETAIL_ACT_NONE;
    touch_ready = 0;

    tx = tp_dev.x[0];
    ty = tp_dev.y[0];
    delay_ms(20);

    btn = hit_btn(tx, ty);
    if (btn == BTN_NONE) return SETTINGS_DETAIL_ACT_NONE;

    if (btn == BTN_OK)
        draw_one_btn(btn, UI_OK_PRESS);
    else
        draw_one_btn(btn, UI_BTN_PRESS);
    delay_ms(120);
    draw_one_btn(btn, UI_CARD);

    if (btn == BTN_CLEAR || btn == BTN_OK) {
        start_selftest();
    } else if (btn == BTN_BACK) {
        return SETTINGS_DETAIL_ACT_BACK;
    } else if (btn == BTN_STOP) {
        g_settings.running = 0;
        g_settings.result = SELFTEST_STOPPED;
        draw_titlebar();
        draw_content();
    }

    return SETTINGS_DETAIL_ACT_NONE;
}
