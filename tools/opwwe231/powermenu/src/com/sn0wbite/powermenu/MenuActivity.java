package com.sn0wbite.powermenu;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Power menu for the OnePlus Watch 2: restart, recovery, fastboot, power off, as four tiles. Installed as a priv-app by the
 * sn0wbite_powermenu module it holds REBOOT, RECOVERY and SHUTDOWN and needs no root; installed as a normal app it falls back to su.
 */
public class MenuActivity extends Activity implements View.OnClickListener {
    private static final String TAG = "sn0wpower";

    @Override
    protected void onCreate(Bundle b) {
        super.onCreate(b);
        DisplayMetrics m = getResources().getDisplayMetrics();
        int side = Math.min(m.widthPixels, m.heightPixels), tile = (int) (side * 0.32f), gap = (int) (side * 0.03f);
        LinearLayout col = new LinearLayout(this);
        col.setOrientation(LinearLayout.VERTICAL);
        col.setGravity(Gravity.CENTER_HORIZONTAL);
        TextView title = new TextView(this);
        title.setText("sn0wbite");
        title.setTextColor(0xFF8CE63C);
        title.setTypeface(Typeface.MONOSPACE, Typeface.BOLD);
        title.setTextSize(15);
        title.setGravity(Gravity.CENTER);
        col.addView(title);
        col.addView(row(tile, gap, Tile.RESTART, "Restart", "reboot", Tile.RECOVERY, "Recovery", "recovery"));
        col.addView(row(tile, gap, Tile.FASTBOOT, "Fastboot", "bootloader", Tile.POWER, "Power off", "shutdown"));
        FrameLayout root = new FrameLayout(this);
        root.setBackgroundColor(Color.BLACK);
        root.addView(col, new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT, Gravity.CENTER));
        setContentView(root);
    }

    private LinearLayout row(int tile, int gap, int k1, String l1, String c1, int k2, String l2, String c2) {
        LinearLayout r = new LinearLayout(this);
        r.setOrientation(LinearLayout.HORIZONTAL);
        r.addView(tile(tile, gap, k1, l1, c1));
        r.addView(tile(tile, gap, k2, l2, c2));
        return r;
    }

    private Tile tile(int size, int gap, int kind, String label, String cmd) {
        Tile t = new Tile(this, kind, label);
        t.setTag(cmd);
        t.setOnClickListener(this);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(size, size);
        lp.setMargins(gap, gap, gap, gap);
        t.setLayoutParams(lp);
        return t;
    }

    @Override
    public void onClick(View v) {
        String cmd = (String) v.getTag();
        try {
            if (cmd.equals("shutdown")) {
                Intent i = new Intent("com.android.internal.intent.action.REQUEST_SHUTDOWN");
                i.putExtra("android.intent.extra.KEY_CONFIRM", false);
                i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(i);
            } else {
                ((PowerManager) getSystemService(POWER_SERVICE)).reboot(cmd.equals("reboot") ? null : cmd);
            }
        } catch (SecurityException e) { // not allowed here (normal install, or a permission missing): try root and say why
            Log.e(TAG, cmd + ": " + e);
            Toast.makeText(this, cmd + ": " + e.getMessage(), Toast.LENGTH_LONG).show();
            new Thread(new Root(this, cmd.equals("reboot") ? "reboot" : cmd.equals("shutdown") ? "reboot -p" : "reboot " + cmd)).start();
        }
    }

    /** Runs one command as root and reports a failure on screen. */
    private static final class Root implements Runnable {
        private final MenuActivity act;
        private final String cmd;
        Root(MenuActivity act, String cmd) { this.act = act; this.cmd = cmd; }
        @Override public void run() {
            int rc = -1;
            try { rc = new ProcessBuilder("su", "-c", cmd).redirectErrorStream(true).start().waitFor(); } catch (Exception e) { /* no su */ }
            Log.e(TAG, "su -c " + cmd + " -> " + rc);
        }
    }
}
