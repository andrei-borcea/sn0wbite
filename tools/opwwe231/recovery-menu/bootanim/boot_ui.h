/* Boot screen drawing, shared by render_base (host) and bootlog (watch): a small logo at the top and, under it, the console written straight
 * on the screen, a straight block as wide as the last row allows inside the circle. The console is rendered into its own layer
 * (CON_NONE = no text) and shows only on black. Include font9.h and logo_boot.h before this file. */
#define CON_NONE 0xFF000000u
#define BOX_BG 0x000000    /* the only colour the text may replace */
#define CON_ROWS 24
#define CON_PITCH 11
#define CON_COLS 45      /* fixed width: the last row must still fit the circle with the margin */
#define CON_FIRST 160      /* y of the centre of the first row */

static const uint32_t con_shade[6] = {0x3D6B2B, 0x4F8A38, 0x62A645, 0x7FBF5A, 0xA6DB86, 0xE6F7DC}; /* old lines dim, newest bright */

static void blit_logo(const unsigned *px, int w, int h, int x0, int y0) {
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++) {
            unsigned p = px[j * w + i];
            if (p) fb[(y0 + j) * pitch_px + x0 + i] = p;
        }
}

static void draw_logo(void) {
    rect(0, 0, W, H, 0x000000);
    blit_logo(apple_px, APPLE_W, APPLE_H, (W - APPLE_W) / 2, 12);
    blit_logo(word_px, WORD_W, WORD_H, (W - WORD_W) / 2, 118);
}

static void con_span(int row, int *x0, int *cols) { /* a straight block, centred: every row has the width the last row can take */
    (void)row;
    *x0 = (W - CON_COLS * 6) / 2;
    *cols = CON_COLS;
}

/* the newest line last; the block is bottom aligned */
static void draw_console(const char *const *ln, int n) {
    if (n > CON_ROWS) { ln += n - CON_ROWS; n = CON_ROWS; }
    for (int i = 0; i < n; i++) {
        int row = CON_ROWS - n + i, x0, cols;
        char t[CON_COLS + 1];
        con_span(row, &x0, &cols);
        snprintf(t, sizeof t, "%.*s", cols, ln[i]);
        text_left(t, x0, CON_FIRST + row * CON_PITCH, 1, i == n - 1 ? con_shade[5] : con_shade[row * 5 / CON_ROWS]);
    }
}
