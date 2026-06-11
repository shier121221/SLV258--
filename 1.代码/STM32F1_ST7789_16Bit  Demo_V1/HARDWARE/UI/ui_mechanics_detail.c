#include "ui_mechanics_detail.h"

#include "ui_main.h"
#include "ui_mechanics_menu.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"

#include <stdio.h>
#include <string.h>

#define BODY_TOP          36
#define BODY_BOTTOM       199
#define BTN_Y             204
#define BTN_H             32
#define BTN_R             13

#define INFO_Y            58
#define VALUE_Y           98
#define FOOTER_Y          156

#define POPUP_X           40
#define POPUP_Y           54
#define POPUP_W           240
#define POPUP_H           124
#define POPUP_ITEM_W      88
#define POPUP_ITEM_H      28

#define VIEW_MAX          8

typedef struct
{
    u8 mode;
    u8 stopped;
    u8 running;
    u8 view;
    u8 view_count;
    u8 seq_no;
    u8 popup_active;
    u8 popup_kind;
    u8 popup_choice;
    u8 width_choice;
    u8 angle_choice;
} mechanics_detail_state_t;

static mechanics_detail_state_t g_mech;

static const u16 btn_x[5] = {4, 64, 124, 184, 244};
static const u16 btn_w[5] = {54, 54, 54, 54, 72};
static const u16 popup_x[4] = {60, 172, 60, 172};
static const u16 popup_y[4] = {92, 92, 128, 128};

static const u8 * const btn_label[5] = {
    (u8*)"\xC7\xE5\xC1\xE3",
    (u8*)"\xB7\xB5\xBB\xD8",
    (u8*)"\xCD\xA3\xD6\xB9",
    (u8*)"\xC7\xD0\xBB\xBB",
    (u8*)"OK"
};

static const u8 * const title_label[5] = {
    (u8*)"\xD5\xF1\xD7\xD3\xD6\xDC\xC6\xDA\xB2\xE2\xC1\xBF",
    (u8*)"\xC5\xF6\xD7\xB2\xCA\xB5\xD1\xE9",
    (u8*)"\xD6\xD8\xC1\xA6\xBC\xD3\xCB\xD9\xB6\xC8",
    (u8*)"\xD6\xB1\xCF\xDF\xBC\xD3\xCB\xD9\xB6\xC8",
    (u8*)"\xBD\xC7\xBC\xD3\xCB\xD9\xB6\xC8\xCA\xB5\xD1\xE9"
};

static const u8 * const state_label[2] = {
    (u8*)"\xB4\xFD\xBB\xFA",
    (u8*)"\xB2\xE2\xC1\xBF\xD6\xD0"
};

static const u8 * const info_label[5] = {
    (u8*)"\xD5\xF1\xD7\xD3\xD6\xDC\xC6\xDA",
    (u8*)"\xB5\xAF\xD0\xD4\xC5\xF6\xD7\xB2",
    (u8*)"\xD7\xD4\xD3\xC9\xC2\xE4\xCC\xE5",
    (u8*)"\xD6\xB1\xCF\xDF\xD4\xCB\xB6\xAF",
    (u8*)"\xD7\xAA\xB6\xAF\xBD\xC7\xB6\xC8"
};

static const u8 * const popup_title_width = (u8*)"\xB5\xB2\xC6\xAC\xD1\xA1\xD4\xF1";
static const u8 * const popup_title_angle = (u8*)"\xBD\xC7\xB6\xC8\xD1\xA1\xD4\xF1";
static const u8 * const footer_label_times = (u8*)"\xB4\xCE\xCA\xFD";

static const char * const width_text[4] = {"1cm", "3cm", "5cm", "10cm"};
static const char * const angle_text[2] = {"180deg", "360deg"};

static const char * const pendulum_view[3] = {"T = 0.00 ms", "f = 0.00 Hz", "SUM = 0.00 ms"};
static const char * const collision_view[8] = {
    "T11 = 0.00 ms", "T12 = 0.00 ms", "T21 = 0.00 ms", "T22 = 0.00 ms",
    "V11 = 0.00 cm/s", "V12 = 0.00 cm/s", "V21 = 0.00 cm/s", "V22 = 0.00 cm/s"
};
static const char * const gravity_view[3] = {"T1 = 0.00 ms", "T2 = 0.00 ms", "T3 = 0.00 ms"};
static const char * const linear_view[6] = {
    "T1 = 0.00 ms", "T2 = 0.00 ms", "T3 = 0.00 ms",
    "V1 = 0.00 cm/s", "V2 = 0.00 cm/s", "a = 0.00 cm/s2"
};
static const char * const angular_view[2] = {"T = 0.00 ms", "a = 0.00 rad/s2"};

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

static void draw_ascii_center(u16 y, u16 font_h, const char *text, u16 color)
{
    u16 char_w = (font_h == 24) ? 12 : 8;
    u16 width = (u16)strlen(text) * char_w;
    u16 x = (320 - width) / 2;

    BACK_COLOR = WHITE;
    POINT_COLOR = color;
    LCD_ShowString(x, y, width + 2, font_h, font_h, (u8*)text);
}

static void draw_gb_center(u16 y, const u8 *text, u16 color)
{
    u16 width = ((u16)strlen((const char *)text) / 2) * 16;
    u16 x = (320 - width) / 2;

    BACK_COLOR = WHITE;
    POINT_COLOR = color;
    Show_Str(x, y, (u8*)text, 16, 0);
}

static const char *current_value_text(void)
{
    if (g_mech.mode == MECH_ITEM_PENDULUM) return pendulum_view[g_mech.view % 3];
    if (g_mech.mode == MECH_ITEM_COLLISION) return collision_view[g_mech.view % 8];
    if (g_mech.mode == MECH_ITEM_GRAVITY) return gravity_view[g_mech.view % 3];
    if (g_mech.mode == MECH_ITEM_LINEAR) return linear_view[g_mech.view % 6];
    return angular_view[g_mech.view % 2];
}

static void draw_titlebar(void)
{
    const u8 *state = g_mech.running ? state_label[1] : state_label[0];
    u16 state_w = ((u16)strlen((const char *)state) / 2) * 16;

    LCD_Fill(0, TITLEBAR_Y, 319, TITLEBAR_Y + TITLEBAR_H - 1, UI_TITLEBAR);
    POINT_COLOR = WHITE;
    BACK_COLOR = UI_TITLEBAR;

    Show_Str(8, 10, (u8*)title_label[g_mech.mode], 16, 0);
    Show_Str((320 - state_w) / 2, 10, (u8*)state, 16, 0);
}

static void draw_value_area(void)
{
    LCD_Fill(0, VALUE_Y, 319, VALUE_Y + 28, WHITE);
    draw_ascii_center(VALUE_Y, 24, current_value_text(), UI_TEXT);
}

static void draw_footer(void)
{
    char seq_buf[8];
    u16 label_w = 32;
    u16 value_w;
    u16 x;

    snprintf(seq_buf, sizeof(seq_buf), "%02u", (unsigned)g_mech.seq_no);
    value_w = (u16)strlen(seq_buf) * 8;
    x = 320 - label_w - value_w - 14;

    LCD_Fill(210, FOOTER_Y, 319, FOOTER_Y + 18, WHITE);
    BACK_COLOR = WHITE;
    POINT_COLOR = UI_SUBTEXT;
    Show_Str(x, FOOTER_Y, (u8*)footer_label_times, 16, 0);
    LCD_ShowString(x + label_w + 4, FOOTER_Y, value_w + 2, 16, 16, (u8*)seq_buf);
}

static void draw_body(void)
{
    LCD_Fill(0, BODY_TOP, 319, BODY_BOTTOM, WHITE);
    draw_gb_center(INFO_Y, info_label[g_mech.mode], UI_SUBTEXT);
    if (g_mech.mode == MECH_ITEM_ANGULAR) {
        draw_ascii_center(INFO_Y + 20, 16, angle_text[g_mech.angle_choice], UI_SUBTEXT);
    } else if (g_mech.mode == MECH_ITEM_COLLISION || g_mech.mode == MECH_ITEM_LINEAR) {
        draw_ascii_center(INFO_Y + 20, 16, width_text[g_mech.width_choice], UI_SUBTEXT);
    }
    draw_value_area();
    draw_footer();
}

static void draw_btn(u8 id, u16 bg)
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

static void draw_button_bar(void)
{
    u8 i;

    LCD_Fill(0, 200, 319, 239, UI_BG);
    for (i = 0; i < 5; i++)
        draw_btn(i, UI_CARD);
}

static void draw_popup(void)
{
    u8 i;
    u8 count = (g_mech.popup_kind == MECH_ITEM_ANGULAR) ? 2 : 4;
    const u8 *title = (g_mech.popup_kind == MECH_ITEM_ANGULAR) ? popup_title_angle : popup_title_width;

    draw_panel(POPUP_X, POPUP_Y, POPUP_W, POPUP_H, 8, UI_CARD);
    BACK_COLOR = UI_CARD;
    POINT_COLOR = UI_TEXT;
    draw_gb_center(POPUP_Y + 12, title, UI_TEXT);

    for (i = 0; i < count; i++) {
        u16 bg = (i == g_mech.popup_choice) ? UI_CARD_PRESS : WHITE;
        const char *label = (g_mech.popup_kind == MECH_ITEM_ANGULAR) ? angle_text[i] : width_text[i];
        fill_round_rect(popup_x[i], popup_y[i], POPUP_ITEM_W, POPUP_ITEM_H, 6, bg);
        BACK_COLOR = bg;
        POINT_COLOR = UI_TEXT;
        LCD_ShowString(popup_x[i] + 16, popup_y[i] + 6, 64, 16, 16, (u8*)label);
    }
}

static void draw_full_page(void)
{
    draw_titlebar();
    draw_body();
    draw_button_bar();
    if (g_mech.popup_active)
        draw_popup();
}

static u8 hit_btn(u16 tx, u16 ty)
{
    u8 i;

    if (ty < BTN_Y || ty >= BTN_Y + BTN_H)
        return BTN_NONE;

    for (i = 0; i < 5; i++) {
        if (tx >= btn_x[i] && tx < btn_x[i] + btn_w[i])
            return i;
    }
    return BTN_NONE;
}

static u8 hit_popup_option(u16 tx, u16 ty)
{
    u8 i;
    u8 count = (g_mech.popup_kind == MECH_ITEM_ANGULAR) ? 2 : 4;

    for (i = 0; i < count; i++) {
        if (tx >= popup_x[i] && tx < popup_x[i] + POPUP_ITEM_W &&
            ty >= popup_y[i] && ty < popup_y[i] + POPUP_ITEM_H)
            return i;
    }
    return 0xFF;
}

static void reset_page(void)
{
    g_mech.stopped = 0;
    g_mech.running = 0;
    g_mech.view = 0;
    g_mech.seq_no = 1;
    draw_full_page();
}

static void switch_view(void)
{
    g_mech.view = (u8)((g_mech.view + 1) % g_mech.view_count);
    draw_value_area();
}

static void confirm_popup(void)
{
    if (!g_mech.popup_active)
        return;

    if (g_mech.popup_kind == MECH_ITEM_ANGULAR)
        g_mech.angle_choice = g_mech.popup_choice;
    else
        g_mech.width_choice = g_mech.popup_choice;

    g_mech.popup_active = 0;
    draw_full_page();
}

void UI_MechanicsDetail_Open(u8 mode)
{
    memset(&g_mech, 0, sizeof(g_mech));
    g_mech.mode = mode;
    g_mech.seq_no = 1;
    g_mech.width_choice = 1;
    g_mech.angle_choice = 0;
    g_mech.popup_choice = 1;
    g_mech.view_count = 3;

    if (mode == MECH_ITEM_COLLISION) {
        g_mech.view_count = 8;
        g_mech.popup_active = 1;
        g_mech.popup_kind = MECH_ITEM_COLLISION;
    } else if (mode == MECH_ITEM_LINEAR) {
        g_mech.view_count = 6;
        g_mech.popup_active = 1;
        g_mech.popup_kind = MECH_ITEM_LINEAR;
    } else if (mode == MECH_ITEM_ANGULAR) {
        g_mech.view_count = 2;
        g_mech.popup_active = 1;
        g_mech.popup_kind = MECH_ITEM_ANGULAR;
        g_mech.popup_choice = 0;
    }

    draw_full_page();
}

u8 UI_MechanicsDetail_Scan(void)
{
    static u8 touch_ready = 0;
    u8 btn;
    u8 option;
    u16 tx;
    u16 ty;

    tp_dev.scan(0);
    if (!(tp_dev.sta & TP_PRES_DOWN)) {
        touch_ready = 1;
        return MECH_DETAIL_ACT_NONE;
    }
    if (!touch_ready)
        return MECH_DETAIL_ACT_NONE;
    touch_ready = 0;

    tx = tp_dev.x[0];
    ty = tp_dev.y[0];
    delay_ms(20);

    if (g_mech.popup_active) {
        option = hit_popup_option(tx, ty);
        if (option != 0xFF) {
            g_mech.popup_choice = option;
            draw_popup();
            return MECH_DETAIL_ACT_NONE;
        }
    }

    btn = hit_btn(tx, ty);
    if (btn == BTN_NONE)
        return MECH_DETAIL_ACT_NONE;

    if (btn == BTN_OK)
        draw_btn(btn, UI_OK_PRESS);
    else
        draw_btn(btn, UI_BTN_PRESS);
    delay_ms(120);
    draw_btn(btn, UI_CARD);

    if (btn == BTN_BACK)
        return MECH_DETAIL_ACT_BACK;
    if (btn == BTN_CLEAR) {
        if (!g_mech.popup_active)
            reset_page();
        return MECH_DETAIL_ACT_NONE;
    }
    if (btn == BTN_STOP) {
        if (!g_mech.popup_active) {
            g_mech.stopped = 1;
            g_mech.running = 0;
            draw_titlebar();
        }
        return MECH_DETAIL_ACT_NONE;
    }
    if (btn == BTN_SWITCH) {
        if (!g_mech.popup_active)
            switch_view();
        return MECH_DETAIL_ACT_NONE;
    }
    if (btn == BTN_OK) {
        if (g_mech.popup_active)
            confirm_popup();
        else {
            g_mech.stopped = 0;
            g_mech.running = 1;
            draw_titlebar();
            draw_value_area();
        }
        return MECH_DETAIL_ACT_NONE;
    }
    return MECH_DETAIL_ACT_NONE;
}
