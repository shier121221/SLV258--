#include "ui_timing_detail.h"

#include "ui_main.h"
#include "ui_timing_menu.h"
#include "lcd.h"
#include "touch.h"
#include "delay.h"
#include "sys.h"

#include <stdio.h>
#include <string.h>

#define BODY_TOP          36
#define BODY_BOTTOM       199
#define BTN_Y             204
#define BTN_H             32
#define BTN_R             13

#define VALUE_Y           98
#define INFO_Y            58
#define FOOTER_Y          156

#define POPUP_X           40
#define POPUP_Y           54
#define POPUP_W           240
#define POPUP_H           124
#define POPUP_ITEM_W      88
#define POPUP_ITEM_H      28

#define HISTORY_MAX       64
#define S1_GATE_MASK      0x03
#define S2_GATE_MASK      0x03
#define EDGE_QUEUE_SIZE   16
#define EDGE_QUEUE_MASK   (EDGE_QUEUE_SIZE - 1)
#define GATE_EXTI_LINES   ((1u << 0) | (1u << 1) | (1u << 15))
#define TIMER_TICK_HZ     6000000u
#define TIMER_TICKS_001MS (TIMER_TICK_HZ / 100000u)
#define TIMER_TICKS_001S  (TIMER_TICK_HZ / 100u)
#define TIMER_TICKS_50MS  (TIMER_TICK_HZ / 20u)
#define S1_MIN_VALID_TICKS (TIMER_TICK_HZ / 10000u)

typedef struct
{
    u8 gate;
    u8 active;
    u32 tick;
} timing_edge_event_t;

typedef struct
{
    u8 mode;
    u8 stopped;
    u8 running;
    u8 browse;
    u8 popup_active;
    u8 popup_choice;
    u8 width_choice;
    u8 active_gate;
    u8 history_count;
    u8 history_view;
    u8 seq_no;

    u32 start_tick;
    u32 last_display_tick;
    u32 elapsed_ticks;
    u32 history[HISTORY_MAX];
    u32 count_value;
} timing_detail_state_t;

static timing_detail_state_t g_detail;
static volatile timing_edge_event_t g_edge_queue[EDGE_QUEUE_SIZE];
static volatile u8 g_edge_head;
static volatile u8 g_edge_tail;
static volatile u32 g_timer_high_us;

static const u16 btn_x[5] = {4, 64, 124, 184, 244};
static const u16 btn_w[5] = {54, 54, 54, 54, 72};
static const u16 width_x[4] = {60, 172, 60, 172};
static const u16 width_y[4] = {92, 92, 128, 128};

static const u8 * const btn_label[5] = {
    (u8*)"\xC7\xE5\xC1\xE3",
    (u8*)"\xB7\xB5\xBB\xD8",
    (u8*)"\xCD\xA3\xD6\xB9",
    (u8*)"\xC7\xD0\xBB\xBB",
    (u8*)"OK"
};

static const u8 * const title_label[3] = {
    (u8*)"\xD5\xDA\xB9\xE2\xBC\xC6\xCA\xB1",
    (u8*)"\xBC\xE4\xB8\xF4\xBC\xC6\xCA\xB1",
    (u8*)"\xBC\xC6\xCA\xFD\xC4\xA3\xCA\xBD"
};

static const u8 * const info_label_s1 = (u8*)"\xBC\xEC\xB2\xE2\xB9\xE2\xB5\xE7\xC3\xC5";
static const u8 * const info_label_s2 = (u8*)"\xB5\xB2\xB9\xE2\xBC\xE4\xB8\xF4";
static const u8 * const info_label_count = (u8*)"\xBC\xEC\xB2\xE2\xB4\xCE\xCA\xFD";
static const u8 * const footer_label_count = (u8*)"\xB5\xB1\xC7\xB0\xB4\xCE\xCA\xFD";
static const u8 * const footer_label_times = (u8*)"\xB4\xCE\xCA\xFD";

static const u8 * const status_text[3] = {
    (u8*)"\xB4\xFD\xBB\xFA",
    (u8*)"\xBC\xC6\xCA\xB1",
    (u8*)"\xCD\xA3\xD6\xB9"
};

static const char * const width_text[4] = {
    "1cm",
    "3cm",
    "5cm",
    "10cm"
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

static void timer_base_init(void)
{
    RCC->APB1ENR |= (1 << 0);
    TIM2->CR1 = 0;
    TIM2->PSC = 12 - 1;
    TIM2->ARR = 0xFFFF;
    TIM2->CNT = 0;
    TIM2->SR = 0;
    TIM2->DIER = 1;
    g_timer_high_us = 0;
    TIM2->EGR = 1;
    TIM2->SR = 0;
    MY_NVIC_Init(0, 0, TIM2_IRQn, 2);
    TIM2->CR1 = 1;
}

static u32 timer_base_now(void)
{
    u32 high = g_timer_high_us;
    u16 cnt = (u16)TIM2->CNT;

    if ((TIM2->SR & 1) && cnt < 0x8000)
        high += 0x10000u;
    return high + cnt;
}

static void timer_base_start(u32 tick)
{
    g_detail.start_tick = tick;
    g_detail.last_display_tick = tick;
    g_detail.elapsed_ticks = 0;
}

static void timer_base_stop(u32 tick)
{
    g_detail.elapsed_ticks = tick - g_detail.start_tick;
}

static void edge_queue_reset(void)
{
    INTX_DISABLE();
    g_edge_head = 0;
    g_edge_tail = 0;
    INTX_ENABLE();
}

static void edge_queue_push(u8 gate, u8 active)
{
    u8 next = (u8)((g_edge_head + 1) & EDGE_QUEUE_MASK);
    u32 tick = timer_base_now();

    if (next == g_edge_tail)
        return;

    g_edge_queue[g_edge_head].gate = gate;
    g_edge_queue[g_edge_head].active = active;
    g_edge_queue[g_edge_head].tick = tick;
    g_edge_head = next;
}

static u8 edge_queue_pop(timing_edge_event_t *event)
{
    u8 tail;

    INTX_DISABLE();
    if (g_edge_tail == g_edge_head) {
        INTX_ENABLE();
        return 0;
    }

    tail = g_edge_tail;
    *event = g_edge_queue[tail];
    g_edge_tail = (u8)((tail + 1) & EDGE_QUEUE_MASK);
    INTX_ENABLE();
    return 1;
}

static void timing_exti_init(void)
{
    RCC->APB2ENR |= (1 << 0);

    AFIO->EXTICR[0] &= ~0x00FFu;
    AFIO->EXTICR[0] |=  0x0011u;
    AFIO->EXTICR[3] &= ~0xF000u;

    EXTI->IMR  |= GATE_EXTI_LINES;
    EXTI->EMR  &= ~GATE_EXTI_LINES;
    EXTI->RTSR |= GATE_EXTI_LINES;
    EXTI->FTSR |= GATE_EXTI_LINES;
    EXTI->PR    = GATE_EXTI_LINES;

    MY_NVIC_Init(1, 1, EXTI0_IRQn, 2);
    MY_NVIC_Init(1, 1, EXTI1_IRQn, 2);
    MY_NVIC_Init(1, 1, EXTI15_10_IRQn, 2);
}

static void timing_exti_enable(u8 enabled)
{
    if (enabled)
        EXTI->IMR |= GATE_EXTI_LINES;
    else
        EXTI->IMR &= ~GATE_EXTI_LINES;
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & (1u << 0)) {
        edge_queue_push(0, (PBin(0) == 0) ? 1 : 0);
        EXTI->PR = (1u << 0);
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI->PR & (1u << 1)) {
        edge_queue_push(1, (PBin(1) == 0) ? 1 : 0);
        EXTI->PR = (1u << 1);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1u << 15)) {
        edge_queue_push(2, (PAin(15) == 0) ? 1 : 0);
        EXTI->PR = (1u << 15);
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & 1) {
        TIM2->SR &= ~1u;
        g_timer_high_us += 0x10000u;
    }
}

static void save_history(u32 value_ticks)
{
    u8 i;

    if (g_detail.history_count < HISTORY_MAX) {
        g_detail.history[g_detail.history_count++] = value_ticks;
    } else {
        for (i = 1; i < HISTORY_MAX; i++)
            g_detail.history[i - 1] = g_detail.history[i];
        g_detail.history[HISTORY_MAX - 1] = value_ticks;
    }

    g_detail.history_view = g_detail.history_count - 1;
    g_detail.browse = 0;
    if (g_detail.seq_no < 255)
        g_detail.seq_no++;
}

static u32 displayed_value_ticks(void)
{
    if (g_detail.browse && g_detail.history_count)
        return g_detail.history[g_detail.history_view];
    if (g_detail.mode == TIMING_ITEM_COUNT)
        return g_detail.count_value;
    if (g_detail.running)
        return timer_base_now() - g_detail.start_tick;
    return g_detail.elapsed_ticks;
}

static const u8 *current_status_text(void)
{
    if (g_detail.stopped) return status_text[2];
    if (g_detail.running) return status_text[1];
    return status_text[0];
}

static void format_main_value(char *buf, size_t size)
{
    if (g_detail.mode == TIMING_ITEM_COUNT) {
        snprintf(buf, size, "%lu", (unsigned long)displayed_value_ticks());
    } else {
        u32 ticks = displayed_value_ticks();
        if (ticks < TIMER_TICK_HZ) {
            u32 centi_ms = (ticks + (TIMER_TICKS_001MS / 2u)) / TIMER_TICKS_001MS;
            snprintf(buf, size, "%lu.%02lu ms",
                     (unsigned long)(centi_ms / 100u), (unsigned long)(centi_ms % 100u));
        } else {
            u32 centi_s = (ticks + (TIMER_TICKS_001S / 2u)) / TIMER_TICKS_001S;
            snprintf(buf, size, "%lu.%02lu S",
                     (unsigned long)(centi_s / 100u), (unsigned long)(centi_s % 100u));
        }
    }
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

static void draw_gb_ascii_right(u16 y, const u8 *label, const char *value, u16 color)
{
    u16 label_w = ((u16)strlen((const char *)label) / 2) * 16;
    u16 value_w = (u16)strlen(value) * 8;
    u16 x = 320 - label_w - value_w - 14;

    BACK_COLOR = WHITE;
    POINT_COLOR = color;
    Show_Str(x, y, (u8*)label, 16, 0);
    LCD_ShowString(x + label_w + 4, y, value_w + 2, 16, 16, (u8*)value);
}

static void draw_value_area(void)
{
    char value_buf[24];

    LCD_Fill(0, VALUE_Y, 319, VALUE_Y + 28, WHITE);
    format_main_value(value_buf, sizeof(value_buf));
    draw_ascii_center(VALUE_Y, 24, value_buf, UI_TEXT);
}

static void draw_titlebar(void)
{
    char suffix[8] = "";
    const u8 *status = current_status_text();
    u16 status_w = ((u16)strlen((const char *)status) / 2) * 16;

    LCD_Fill(0, TITLEBAR_Y, 319, TITLEBAR_Y + TITLEBAR_H - 1, UI_TITLEBAR);
    POINT_COLOR = WHITE;
    BACK_COLOR = UI_TITLEBAR;

    Show_Str(8, 10, (u8*)title_label[g_detail.mode], 16, 0);
    if (g_detail.mode == TIMING_ITEM_S1) strcpy(suffix, " S1");
    else if (g_detail.mode == TIMING_ITEM_S2) strcpy(suffix, " S2");
    else if (g_detail.mode == TIMING_ITEM_COUNT) strcpy(suffix, " C");

    if (suffix[0] != '\0')
        LCD_ShowString(86, 10, 40, 16, 16, (u8*)suffix);

    gui_fill_circle(176, 18, 1, WHITE);
    Show_Str((320 - status_w) / 2, 10, (u8*)status, 16, 0);
}

static void draw_body(void)
{
    char footer_buf[24];
    char state_buf[16];

    LCD_Fill(0, BODY_TOP, 319, BODY_BOTTOM, WHITE);
    BACK_COLOR = WHITE;

    if (g_detail.mode == TIMING_ITEM_S1) {
        if (g_detail.active_gate == 0xFF)
            strcpy(state_buf, "GM1 / GM2");
        else
            snprintf(state_buf, sizeof(state_buf), "GM%u", (unsigned)(g_detail.active_gate + 1));
        draw_gb_center(INFO_Y, info_label_s1, UI_SUBTEXT);
        draw_ascii_center(INFO_Y + 20, 16, state_buf, UI_SUBTEXT);
    } else if (g_detail.mode == TIMING_ITEM_S2) {
        draw_gb_center(INFO_Y, info_label_s2, UI_SUBTEXT);
        draw_ascii_center(INFO_Y + 20, 16, width_text[g_detail.width_choice], UI_SUBTEXT);
    } else if (g_detail.mode == TIMING_ITEM_COUNT) {
        draw_gb_center(INFO_Y, info_label_count, UI_SUBTEXT);
    }

    draw_value_area();

    if (g_detail.mode == TIMING_ITEM_COUNT) {
        draw_gb_center(FOOTER_Y, footer_label_count, UI_SUBTEXT);
    } else if (g_detail.browse && g_detail.history_count) {
        snprintf(footer_buf, sizeof(footer_buf), "%02u/%02u",
                 (unsigned)(g_detail.history_view + 1), (unsigned)g_detail.history_count);
        draw_gb_ascii_right(FOOTER_Y, footer_label_times, footer_buf, UI_SUBTEXT);
    } else {
        snprintf(footer_buf, sizeof(footer_buf), "%02u", (unsigned)g_detail.seq_no);
        draw_gb_ascii_right(FOOTER_Y, footer_label_times, footer_buf, UI_SUBTEXT);
    }
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
    char label[8];

    draw_panel(POPUP_X, POPUP_Y, POPUP_W, POPUP_H, 8, UI_CARD);
    BACK_COLOR = UI_CARD;
    POINT_COLOR = UI_TEXT;
    draw_gb_center(POPUP_Y + 12, info_label_s2, UI_TEXT);

    for (i = 0; i < 4; i++) {
        u16 bg = (i == g_detail.popup_choice) ? UI_CARD_PRESS : WHITE;
        fill_round_rect(width_x[i], width_y[i], POPUP_ITEM_W, POPUP_ITEM_H, 6, bg);
        BACK_COLOR = bg;
        POINT_COLOR = UI_TEXT;
        strcpy(label, width_text[i]);
        LCD_ShowString(width_x[i] + 22, width_y[i] + 6, 50, 16, 16, (u8*)label);
    }
}

static void draw_full_page(void)
{
    draw_titlebar();
    draw_body();
    draw_button_bar();
    if (g_detail.popup_active)
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

    for (i = 0; i < 4; i++) {
        if (tx >= width_x[i] && tx < width_x[i] + POPUP_ITEM_W &&
            ty >= width_y[i] && ty < width_y[i] + POPUP_ITEM_H)
            return i;
    }
    return 0xFF;
}

static void set_stopped(u8 stopped)
{
    g_detail.stopped = stopped ? 1 : 0;
    if (g_detail.stopped) {
        g_detail.running = 0;
        edge_queue_reset();
    }
    draw_titlebar();
}

static void handle_clear(void)
{
    g_detail.stopped = 0;
    g_detail.running = 0;
    g_detail.browse = 0;
    g_detail.history_count = 0;
    g_detail.history_view = 0;
    g_detail.seq_no = 1;
    g_detail.elapsed_ticks = 0;
    g_detail.count_value = 0;
    g_detail.active_gate = 0xFF;
    edge_queue_reset();
    draw_full_page();
}

static void handle_switch(void)
{
    if ((g_detail.mode == TIMING_ITEM_S1 || g_detail.mode == TIMING_ITEM_S2) &&
        g_detail.history_count > 0) {
        if (!g_detail.browse)
            g_detail.history_view = 0;
        else
            g_detail.history_view = (g_detail.history_view + 1) % g_detail.history_count;
        g_detail.browse = 1;
        g_detail.running = 0;
        draw_body();
        draw_titlebar();
    }
}

static void handle_ok(void)
{
    if (g_detail.popup_active) {
        g_detail.width_choice = g_detail.popup_choice;
        g_detail.popup_active = 0;
        edge_queue_reset();
        draw_full_page();
        return;
    }

    g_detail.browse = 0;

    g_detail.stopped = 0;
    edge_queue_reset();

    draw_titlebar();
    draw_body();
}

static void process_count_event(const timing_edge_event_t *event)
{
    if (g_detail.stopped)
        return;

    if (!event->active)
        return;

    g_detail.count_value++;
    if (!g_detail.running) {
        g_detail.running = 1;
        draw_titlebar();
    }
    draw_value_area();
}

static void process_s1_event(const timing_edge_event_t *event)
{
    if (g_detail.stopped)
        return;
    if (event->gate > 1)
        return;

    if (!g_detail.running) {
        if (!event->active)
            return;
        g_detail.active_gate = event->gate;
        g_detail.running = 1;
        g_detail.browse = 0;
        timer_base_start(event->tick);
        draw_titlebar();
        draw_body();
        return;
    }

    if (event->gate == g_detail.active_gate && !event->active) {
        u32 duration_ticks = event->tick - g_detail.start_tick;

        g_detail.running = 0;
        g_detail.active_gate = 0xFF;
        if (duration_ticks >= S1_MIN_VALID_TICKS) {
            g_detail.elapsed_ticks = duration_ticks;
            save_history(g_detail.elapsed_ticks);
        }
        draw_titlebar();
        draw_body();
    }
}

static void process_s2_event(const timing_edge_event_t *event)
{
    if (g_detail.popup_active || g_detail.stopped)
        return;
    if (!(S2_GATE_MASK & (1u << event->gate)))
        return;
    if (!event->active)
        return;

    if (!g_detail.running) {
        g_detail.running = 1;
        g_detail.active_gate = event->gate;
        g_detail.browse = 0;
        timer_base_start(event->tick);
        draw_titlebar();
        draw_body();
    } else {
        if (event->gate == g_detail.active_gate)
            return;
        timer_base_stop(event->tick);
        save_history(g_detail.elapsed_ticks);
        g_detail.running = 0;
        g_detail.active_gate = event->gate;
        draw_titlebar();
        draw_body();
    }
}

void UI_TimingDetail_Open(u8 mode)
{
    memset(&g_detail, 0, sizeof(g_detail));
    g_detail.mode = mode;
    g_detail.seq_no = 1;
    g_detail.popup_choice = 1;
    g_detail.width_choice = 1;
    g_detail.active_gate = 0xFF;
    g_detail.popup_active = (mode == TIMING_ITEM_S2) ? 1 : 0;

    timer_base_init();
    edge_queue_reset();
    timing_exti_init();
    timing_exti_enable(1);
    draw_full_page();
}

u8 UI_TimingDetail_Scan(void)
{
    static u8 touch_ready = 0;
    timing_edge_event_t event;
    u32 now_tick;
    u8 btn;
    u8 option;
    u16 tx;
    u16 ty;

    while (edge_queue_pop(&event)) {
        if (g_detail.mode == TIMING_ITEM_S1)
            process_s1_event(&event);
        else if (g_detail.mode == TIMING_ITEM_S2)
            process_s2_event(&event);
        else
            process_count_event(&event);
    }

    if (g_detail.running && !g_detail.browse && !g_detail.popup_active &&
        g_detail.mode != TIMING_ITEM_COUNT) {
        now_tick = timer_base_now();
        if ((u32)(now_tick - g_detail.last_display_tick) >= TIMER_TICKS_50MS) {
            g_detail.last_display_tick = now_tick;
            draw_value_area();
        }
    }

    tp_dev.scan(0);
    if (!(tp_dev.sta & TP_PRES_DOWN)) {
        touch_ready = 1;
        return TIMING_DETAIL_ACT_NONE;
    }
    if (!touch_ready)
        return TIMING_DETAIL_ACT_NONE;
    touch_ready = 0;

    tx = tp_dev.x[0];
    ty = tp_dev.y[0];
    delay_ms(20);

    if (g_detail.popup_active) {
        option = hit_popup_option(tx, ty);
        if (option != 0xFF) {
            g_detail.popup_choice = option;
            draw_popup();
            return TIMING_DETAIL_ACT_NONE;
        }
    }

    btn = hit_btn(tx, ty);
    if (btn == BTN_NONE)
        return TIMING_DETAIL_ACT_NONE;

    if (btn == BTN_OK)
        draw_btn(btn, UI_OK_PRESS);
    else
        draw_btn(btn, UI_BTN_PRESS);
    delay_ms(120);
    draw_btn(btn, UI_CARD);

    if (btn == BTN_BACK) {
        timing_exti_enable(0);
        edge_queue_reset();
        return TIMING_DETAIL_ACT_BACK;
    }
    if (btn == BTN_CLEAR) {
        if (!g_detail.popup_active)
            handle_clear();
        return TIMING_DETAIL_ACT_NONE;
    }
    if (btn == BTN_STOP) {
        if (!g_detail.popup_active)
            set_stopped(1);
        return TIMING_DETAIL_ACT_NONE;
    }
    if (btn == BTN_SWITCH) {
        if (!g_detail.popup_active)
            handle_switch();
        return TIMING_DETAIL_ACT_NONE;
    }
    if (btn == BTN_OK) {
        handle_ok();
        return TIMING_DETAIL_ACT_NONE;
    }
    return TIMING_DETAIL_ACT_NONE;
}
