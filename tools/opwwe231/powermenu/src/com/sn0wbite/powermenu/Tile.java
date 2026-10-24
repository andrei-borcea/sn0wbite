package com.sn0wbite.powermenu;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.graphics.Typeface;
import android.view.MotionEvent;
import android.view.View;

/** One square tile of the menu: a rounded frame, an icon drawn with paths and a label. */
final class Tile extends View {
    static final int RESTART = 0, RECOVERY = 1, FASTBOOT = 2, POWER = 3;
    private static final int ACCENT = 0xFF8CE63C, FILL = 0xFF0B0F0B, PRESSED = 0xFF1F3A12, TEXT = 0xFFE6F7DC;
    private final int kind;
    private final String label;
    private final Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Path path = new Path();
    private final RectF r = new RectF();
    private boolean down;

    Tile(Context c, int kind, String label) {
        super(c);
        this.kind = kind;
        this.label = label;
        setClickable(true);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        int a = e.getActionMasked();
        if (a == MotionEvent.ACTION_DOWN) { down = true; invalidate(); }
        else if (a == MotionEvent.ACTION_UP || a == MotionEvent.ACTION_CANCEL) { down = false; invalidate(); }
        return super.onTouchEvent(e);
    }

    @Override
    protected void onDraw(Canvas c) {
        float w = getWidth(), h = getHeight(), s = Math.min(w, h), lw = s * 0.045f;
        p.setStyle(Paint.Style.FILL);
        p.setColor(down ? PRESSED : FILL);
        r.set(lw / 2, lw / 2, w - lw / 2, h - lw / 2);
        c.drawRoundRect(r, s * 0.2f, s * 0.2f, p);
        p.setStyle(Paint.Style.STROKE);
        p.setStrokeWidth(lw * 0.6f);
        p.setColor(ACCENT);
        c.drawRoundRect(r, s * 0.2f, s * 0.2f, p);
        float cx = w / 2, cy = h * 0.42f, ir = s * 0.21f; // icon centre and radius
        p.setStrokeWidth(lw);
        p.setStrokeCap(Paint.Cap.ROUND);
        p.setStrokeJoin(Paint.Join.ROUND);
        r.set(cx - ir, cy - ir, cx + ir, cy + ir);
        switch (kind) {
            case POWER:
                c.drawArc(r, -60, 300, false, p);
                c.drawLine(cx, cy - ir * 1.15f, cx, cy - ir * 0.1f, p);
                break;
            case RESTART:
                c.drawArc(r, -40, 290, false, p);
                p.setStyle(Paint.Style.FILL);
                path.reset();
                path.moveTo(cx + ir * 0.77f - ir * 0.55f, cy - ir * 0.64f - ir * 0.4f);
                path.lineTo(cx + ir * 1.15f, cy - ir * 0.95f);
                path.lineTo(cx + ir * 1.0f, cy - ir * 0.2f);
                path.close();
                c.drawPath(path, p);
                break;
            case RECOVERY: // gear: ring and eight teeth
                c.drawCircle(cx, cy, ir * 0.62f, p);
                for (int i = 0; i < 8; i++) {
                    double a = i * Math.PI / 4;
                    c.drawLine(cx + (float) Math.cos(a) * ir * 0.85f, cy + (float) Math.sin(a) * ir * 0.85f,
                               cx + (float) Math.cos(a) * ir * 1.18f, cy + (float) Math.sin(a) * ir * 1.18f, p);
                }
                break;
            default: // FASTBOOT: lightning bolt
                p.setStyle(Paint.Style.FILL);
                path.reset();
                path.moveTo(cx + ir * 0.25f, cy - ir * 1.2f);
                path.lineTo(cx - ir * 0.7f, cy + ir * 0.15f);
                path.lineTo(cx - ir * 0.05f, cy + ir * 0.15f);
                path.lineTo(cx - ir * 0.3f, cy + ir * 1.2f);
                path.lineTo(cx + ir * 0.75f, cy - ir * 0.3f);
                path.lineTo(cx + ir * 0.1f, cy - ir * 0.3f);
                path.close();
                c.drawPath(path, p);
        }
        p.setStyle(Paint.Style.FILL);
        p.setColor(TEXT);
        p.setTypeface(Typeface.MONOSPACE);
        p.setFakeBoldText(true);
        p.setTextAlign(Paint.Align.CENTER);
        p.setTextSize(s * 0.135f);
        c.drawText(label, cx, h * 0.86f, p);
    }
}
