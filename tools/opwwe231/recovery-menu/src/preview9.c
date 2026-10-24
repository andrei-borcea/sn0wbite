/* Host preview of the v9 UI: ./preview9 <scenario 0..4> <out.ppm>   (see BUILD.md; the PPM gets the round-panel mask) */
#include <stdio.h>
#include <stdlib.h>
#include "ui9.h"
static void present(void) {}
int main(int argc, char **argv) {
    static const struct item main_items[] = {
        {"WEAR OS", 0, 0, 1}, {"DUAL BOOT", 1, 0, 1}, {"RECOVERY STOCK", 2, 0, 1},
        {"FASTBOOT", 3, 0, 1}, {"INFO", 4, 0, 1}, {"SPEGNI", 5, 0, 1}};
    static const struct item dual_items[] = {{"WATCHOS TEST", 0, 0, 0}, {"INDIETRO", 1, 0, 1}};
    static const char *const info[] = {"BATT 87% A BATTERIA", "UP 123S  BUILD V9", "BCB IN: BOOT-RECOVERY", "DISPLAY OK TENT 2", "TASTI: 2 DISPOSITIVI", "DUAL BOOT: OK"};
    static const char *const log[] = {
        "0 === SN0WMENU V9 PID=205 SIM=0 IDLE=120", "0 BCB AT ENTRY: 'BOOT-RECOVERY'", "0 ADB: SYS.USB.CONFIG=ADB -> 0",
        "0 DISPLAY 1: CONNECTOR 32 CRTC 80 MODE", "  466X466, CRTC FB=0 MODE_VALID=1", "0 DISPLAY 1: SETCRTC ERRNO=22",
        "0 DISPLAY 2: CONNECTOR 32 CRTC 80 MODE", "  466X466, CRTC FB=0 MODE_VALID=0", "0 DISPLAY: OK AFTER 2 ATTEMPT(S)",
        "0 INPUT EVENT0 'GPIO-KEYS' KEY=1 TOUCH=0", "0 INPUT EVENT1 'QPNP_PON' KEY=1 TOUCH=0",
        "0 READY: 2 KEY DEVICE(S), 1 TARGET(S)", "12 KEY QPNP_PON CODE=114", "13 KEY QPNP_PON CODE=114", "16 KEY QPNP_PON CODE=116"};
    int sc = argc > 1 ? atoi(argv[1]) : 0;
    pitch_px = W; fb = calloc(W * H, 4); swap_rb = 0;
    if (sc == 0) draw_menu(main_items, 6, 0, 0, "PWR=OK  ALTRI=AVANTI", log, 15);
    else if (sc == 1) draw_menu(main_items, 6, 5, 3, "PWR=OK  ALTRI=AVANTI", log, 15);
    else if (sc == 2) draw_menu(dual_items, 2, 0, 0, "NESSUN PAYLOAD", log + 9, 6);
    else if (sc == 3) draw_lines(info, 6, "OGNI TASTO = INDIETRO");
    else draw_menu(main_items, 6, 3, 1, "RIAVVIO AUTO TRA 25S", log, 15);
    for (int j = 0; j < H; j++) for (int i = 0; i < W; i++) { /* round panel mask */
        float dx = i - 232.5f, dy = j - 232.5f; if (dx * dx + dy * dy > 233.f * 233.f) fb[j * W + i] = 0x202020; }
    FILE *f = fopen(argc > 2 ? argv[2] : "preview.ppm", "wb"); fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int i = 0; i < W * H; i++) { unsigned p = fb[i]; fputc(p >> 16, f); fputc((p >> 8) & 255, f); fputc(p & 255, f); }
    fclose(f); return 0;
}
