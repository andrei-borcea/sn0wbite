/* sn0wbite boot menu v9: the recovery screens (logo header, menu rows, status line, verbose window).
 * Primitives (framebuffer, font, rect, text) are in font9.h. Needs from the includer: present() and the globals set up there. */
#include "font9.h"
#include "logo9.h"

#define ROWS 3          /* visible menu rows */
#define ROW_TOP 100
#define ROW_PITCH 40
#define BH 34
#define BX 70
#define BW 326
#define STATUS_Y 229    /* hint / notice line between the menu and the log window */
#define LOG_X 100       /* log window: the widest rectangle that still fits the round panel at its bottom edge */
#define LOG_W 266
#define LOG_COLS 42     /* characters per log line (scale 1: 6 px per character) */
#define LOG_PITCH 11
#define LOG_Y 243
#define LOG_H 177
#define LOG_LINES 15
#define ACCENT 0x8CE63C
#define DIM 0x6FA84A

static void present(void);

static void header(void) {
    rect(0, 0, W, H, 0x000000);
    blit(apple_px, APPLE_W, APPLE_H, (W - APPLE_W) / 2, 10);
    blit(word_px, WORD_W, WORD_H, (W - WORD_W) / 2, 70);
}

static void status_line(const char *s, int y) { /* at most 22 characters at scale 2 stay inside the round panel */
    int sc = text_w(s, 2) <= 262 ? 2 : 1;
    text(s, W / 2, y, sc, DIM);
}

struct item { const char *label; int act, arg, enabled; };

/* verbose window: the newest line last, at most LOG_LINES lines of LOG_COLS characters, already upper case */
static void draw_log(const char *const *ln, int n) {
    rrect(LOG_X, LOG_Y, LOG_W, LOG_H, 10, 0x2F5A1F);
    rrect(LOG_X + 1, LOG_Y + 1, LOG_W - 2, LOG_H - 2, 9, 0x050805);
    if (n > LOG_LINES) { ln += n - LOG_LINES; n = LOG_LINES; }
    for (int i = 0; i < n; i++)
        text_left(ln[i], LOG_X + 7, LOG_Y + 9 + i * LOG_PITCH + LOG_PITCH / 2, 1, i == n - 1 ? 0xE6F7DC : 0x7FBF5A);
}

static void draw_menu(const struct item *it, int n, int sel, int top, const char *status, const char *const *log, int nlog) {
    header();
    for (int k = 0; k < ROWS && top + k < n; k++) {
        int i = top + k, on = (i == sel), y = ROW_TOP + k * ROW_PITCH;
        rrect(BX, y, BW, BH, 13, on ? ACCENT : 0x2F5A1F);
        if (!on) rrect(BX + 2, y + 2, BW - 4, BH - 4, 11, 0x0B0F0B);
        text(it[i].label, W / 2, y + BH / 2, 3, on ? 0x000000 : it[i].enabled ? 0xE6F7DC : 0x4B5A44);
    }
    if (n > ROWS) { /* scroll bar: track + thumb, right of the buttons */
        int th = ROWS * ROW_PITCH - 6, bh = th * ROWS / n, by = ROW_TOP + (th - bh) * top / (n - ROWS);
        rect(BX + BW + 12, ROW_TOP, 4, th, 0x1B2A14);
        rect(BX + BW + 12, by, 4, bh, ACCENT);
    }
    status_line(status, STATUS_Y);
    draw_log(log, nlog);
    present();
}

static void draw_lines(const char *const *ln, int n, const char *status) { /* info page: n lines of at most 22 characters */
    header();
    for (int i = 0; i < n; i++) text(ln[i], W / 2, 126 + i * 30, 2, 0xE6F7DC);
    status_line(status, 396);
    present();
}
