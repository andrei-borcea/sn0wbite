/* Host tool: renders the static part of the boot screen (the logo), or with a lines file the whole screen as the watch shows it.
 *   render_base out.ppm [lines.txt]      one line of text per console line, newest last */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "font9.h"
#include "logo_boot.h"
#include "boot_ui.h"
int main(int argc, char **argv) {
    static char buf[64][CON_COLS + 1];
    static uint32_t base[W * H], layer[W * H];
    const char *ln[64];
    int n = 0;
    pitch_px = W; swap_rb = 0;
    fb = base;
    draw_logo();
    if (argc > 2) {
        FILE *f = fopen(argv[2], "r");
        char l[256];
        while (f && n < 64 && fgets(l, sizeof l, f)) {
            l[strcspn(l, "\r\n")] = 0;
            snprintf(buf[n], sizeof buf[n], "%s", l);
            for (char *c = buf[n]; *c; c++) if (*c >= 'a' && *c <= 'z') *c -= 32;
            ln[n] = buf[n]; n++;
        }
        if (f) fclose(f);
        for (int i = 0; i < W * H; i++) layer[i] = CON_NONE;
        fb = layer;
        draw_console(ln, n);
    }
    FILE *o = fopen(argv[1], "wb");
    fprintf(o, "P6\n%d %d\n255\n", W, H);
    for (int i = 0; i < W * H; i++) {
        uint32_t p = base[i] == BOX_BG && n && layer[i] != CON_NONE ? layer[i] : base[i];
        fputc(p >> 16, o); fputc((p >> 8) & 255, o); fputc(p & 255, o);
    }
    fclose(o);
    return 0;
}
