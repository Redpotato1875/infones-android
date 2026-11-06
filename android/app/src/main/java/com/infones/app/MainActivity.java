package com.infones.app;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.provider.OpenableColumns;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;

/**
 * InfoNES Android 外壳层（键盘版）。
 *
 * 控制方案（物理键盘）：
 *   方向     W A S D        ← 上 左 下 右
 *   A 键     J              ← 主攻击 / 确认
 *   B 键     K              ← 副攻击 / 取消
 *   Select   H
 *   Start    L
 *
 * 结构：顶栏（打开 / 重置 / 停止 / 跳帧 / 静音）+ 状态
 *       SurfaceView（256x240，硬件缩放，保持 16:15 比例，占满剩余空间）
 *
 * 线程模型：模拟运行在 native 线程（InfoNES_Main），UI 线程只负责
 * surface 生命周期与按键采样；停止用 PAD_SYS_QUIT 标志在帧尾优雅退出。
 */
public class MainActivity extends Activity
        implements SurfaceHolder.Callback {

    private static final int REQ_OPEN_ROM = 1001;

    private SurfaceView mSurfaceView;
    private TextView mStatus;
    private Button mBtnSkip;
    private Button mBtnMute;

    private byte[] mRomBytes;
    private String mRomName = "";
    private boolean mWantRun;       /* 用户已开始：回前台 / surface 重建后自动续跑 */
    private boolean mSurfaceReady;
    private int mFrameSkip;
    private boolean mMuted;

    private int dp(float v) {
        return Math.round(v * getResources().getDisplayMetrics().density);
    }

    /*-----------------------------------------------------------------*/
    /*  生命周期                                                        */
    /*-----------------------------------------------------------------*/

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        NativeBridge.nativeInit(getFilesDir().getAbsolutePath());

        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(Color.BLACK);

        root.addView(buildTopBar(), new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));

        mSurfaceView = new SurfaceView(this);
        mSurfaceView.getHolder().addCallback(this);
        mSurfaceView.setKeepScreenOn(true);
        root.addView(mSurfaceView, new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f));

        setContentView(root);

        /* 确保根视图能接收键盘事件（Activity 的 onKeyDown/onKeyUp 才会被派发到） */
        root.setFocusable(true);
        root.setFocusableInTouchMode(true);
        root.requestFocus();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mSurfaceReady && mWantRun && mRomBytes != null
                && !NativeBridge.nativeIsRunning()) {
            NativeBridge.nativeStart();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        /* 保存 SRAM；mWantRun 保留，回来时自动续跑 */
        NativeBridge.nativeStop();
    }

    @Override
    protected void onDestroy() {
        NativeBridge.nativeStop();
        super.onDestroy();
    }

    /*-----------------------------------------------------------------*/
    /*  ROM 选择（SAF）                                                 */
    /*-----------------------------------------------------------------*/

    private void openRom() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        startActivityForResult(intent, REQ_OPEN_ROM);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode != REQ_OPEN_ROM || resultCode != RESULT_OK || data == null) {
            return;
        }
        Uri uri = data.getData();
        if (uri == null) {
            return;
        }

        byte[] bytes = readAll(uri);
        if (bytes == null) {
            mStatus.setText("读取 ROM 失败");
            return;
        }

        String name = queryDisplayName(uri);
        if (name.toLowerCase().endsWith(".nes")) {
            name = name.substring(0, name.length() - 4);
        }

        mRomBytes = bytes;
        mRomName = name;
        loadAndStart();
    }

    private byte[] readAll(Uri uri) {
        try (InputStream in = getContentResolver().openInputStream(uri)) {
            if (in == null) {
                return null;
            }
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] buf = new byte[65536];
            int n;
            while ((n = in.read(buf)) > 0) {
                out.write(buf, 0, n);
            }
            return out.toByteArray();
        } catch (Exception e) {
            return null;
        }
    }

    private String queryDisplayName(Uri uri) {
        try (Cursor c = getContentResolver().query(uri, null, null, null, null)) {
            if (c != null && c.moveToFirst()) {
                int idx = c.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                if (idx >= 0) {
                    String n = c.getString(idx);
                    if (n != null) {
                        return n;
                    }
                }
            }
        } catch (Exception ignored) {
        }
        return "rom";
    }

    /*-----------------------------------------------------------------*/
    /*  装载 / 停止 / 重置                                              */
    /*-----------------------------------------------------------------*/

    private void loadAndStart() {
        NativeBridge.nativeStop();
        int r = NativeBridge.nativeLoadRom(mRomBytes, mRomName);
        if (r != 0) {
            mWantRun = false;
            mStatus.setText("ROM 载入失败 (" + r + ")");
            return;
        }
        NativeBridge.nativeSetFrameSkip(mFrameSkip);
        NativeBridge.nativeSetMute(mMuted);
        NativeBridge.nativeStart();
        mWantRun = true;
        mStatus.setText(mRomName);
    }

    private void resetRom() {
        if (mRomBytes != null) {
            loadAndStart();
        }
    }

    private void stopEmulation() {
        NativeBridge.nativeStop();
        mWantRun = false;
        mStatus.setText("已停止");
    }

    private void cycleFrameSkip() {
        mFrameSkip = (mFrameSkip + 1) % 4;
        NativeBridge.nativeSetFrameSkip(mFrameSkip);
        mBtnSkip.setText("跳帧:" + mFrameSkip);
    }

    private void toggleMute() {
        mMuted = !mMuted;
        NativeBridge.nativeSetMute(mMuted);
        mBtnMute.setText(mMuted ? "静音:开" : "静音:关");
    }

    /*-----------------------------------------------------------------*/
    /*  Surface 回调                                                    */
    /*-----------------------------------------------------------------*/

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        NativeBridge.nativeSetSurface(holder.getSurface());
        mSurfaceReady = true;
        if (mRomBytes != null && mWantRun && !NativeBridge.nativeIsRunning()) {
            NativeBridge.nativeStart();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        resizeSurface();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mSurfaceReady = false;
        NativeBridge.nativeStop();          /* 帧尾退出 + 保存 SRAM */
        NativeBridge.nativeClearSurface();
    }

    private void resizeSurface() {
        if (mSurfaceView == null) {
            return;
        }
        /* 顶栏高度约 dp(52)，下方全部留给画面 */
        int availW = getResources().getDisplayMetrics().widthPixels;
        int availH = getResources().getDisplayMetrics().heightPixels - dp(52);
        if (availW <= 0 || availH <= 0) {
            return;
        }

        final float ratio = 256f / 240f;
        int w = availW;
        int h = Math.round(availW / ratio);
        if (h > availH) {
            h = availH;
            w = Math.round(availH * ratio);
        }
        if (mSurfaceView.getWidth() == w && mSurfaceView.getHeight() == h) {
            return;
        }

        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(w, h);
        lp.gravity = Gravity.CENTER_HORIZONTAL;
        mSurfaceView.setLayoutParams(lp);
    }

    /*-----------------------------------------------------------------*/
    /*  键盘输入（WASDHJKL + 物理手柄兼容）                              */
    /*-----------------------------------------------------------------*/

    private static int keyCodeToPad(int keyCode) {
        switch (keyCode) {
            /* 方向：W A S D */
            case KeyEvent.KEYCODE_W:             return NativeBridge.PAD_UP;
            case KeyEvent.KEYCODE_S:             return NativeBridge.PAD_DOWN;
            case KeyEvent.KEYCODE_A:             return NativeBridge.PAD_LEFT;
            case KeyEvent.KEYCODE_D:             return NativeBridge.PAD_RIGHT;
            /* 动作：J = A 键，K = B 键 */
            case KeyEvent.KEYCODE_J:             return NativeBridge.PAD_A;
            case KeyEvent.KEYCODE_K:             return NativeBridge.PAD_B;
            /* Select / Start：H / L */
            case KeyEvent.KEYCODE_H:             return NativeBridge.PAD_SELECT;
            case KeyEvent.KEYCODE_L:             return NativeBridge.PAD_START;
            /* 物理方向键与手柄按键兼容（外接手柄 / 车载等） */
            case KeyEvent.KEYCODE_DPAD_UP:       return NativeBridge.PAD_UP;
            case KeyEvent.KEYCODE_DPAD_DOWN:     return NativeBridge.PAD_DOWN;
            case KeyEvent.KEYCODE_DPAD_LEFT:     return NativeBridge.PAD_LEFT;
            case KeyEvent.KEYCODE_DPAD_RIGHT:    return NativeBridge.PAD_RIGHT;
            case KeyEvent.KEYCODE_ENTER:
            case KeyEvent.KEYCODE_BUTTON_START:  return NativeBridge.PAD_START;
            case KeyEvent.KEYCODE_SHIFT_LEFT:
            case KeyEvent.KEYCODE_BUTTON_SELECT: return NativeBridge.PAD_SELECT;
            case KeyEvent.KEYCODE_BUTTON_A:      return NativeBridge.PAD_A;
            case KeyEvent.KEYCODE_BUTTON_B:      return NativeBridge.PAD_B;
            default:                             return -1;
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        int bit = keyCodeToPad(keyCode);
        if (bit >= 0) {
            NativeBridge.nativeSetPadBit(bit, true);
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        int bit = keyCodeToPad(keyCode);
        if (bit >= 0) {
            NativeBridge.nativeSetPadBit(bit, false);
            return true;
        }
        return super.onKeyUp(keyCode, event);
    }

    /*-----------------------------------------------------------------*/
    /*  界面构建（纯代码，无 XML 依赖）                                  */
    /*-----------------------------------------------------------------*/

    private View buildTopBar() {
        LinearLayout bar = new LinearLayout(this);
        bar.setOrientation(LinearLayout.HORIZONTAL);

        bar.addView(barButton("打开ROM", v -> openRom()));
        bar.addView(barButton("重置", v -> resetRom()));
        bar.addView(barButton("停止", v -> stopEmulation()));

        mBtnSkip = barButton("跳帧:0", v -> cycleFrameSkip());
        bar.addView(mBtnSkip);

        mBtnMute = barButton("静音:关", v -> toggleMute());
        bar.addView(mBtnMute);

        mStatus = new TextView(this);
        mStatus.setTextColor(0xFFB0B0B0);
        mStatus.setPadding(dp(8), dp(10), dp(8), dp(8));
        mStatus.setSingleLine(true);
        mStatus.setText("WASD方向 J=A K=B H=Sel L=Start");
        bar.addView(mStatus, new LinearLayout.LayoutParams(
                0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f));

        return bar;
    }

    private Button barButton(String label, View.OnClickListener listener) {
        Button b = new Button(this);
        b.setText(label);
        b.setTextSize(12);
        b.setPadding(dp(6), dp(2), dp(6), dp(2));
        b.setOnClickListener(listener);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        lp.rightMargin = dp(4);
        b.setLayoutParams(lp);
        return b;
    }
}
