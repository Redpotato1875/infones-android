package com.infones.app;

import android.view.Surface;

/**
 * native 库（libinfones.so）的 JNI 桥。
 *
 * 位序与 Win 版 InfoNES_PadState 完全一致：
 * bit0=A, bit1=B, bit2=Select, bit3=Start, bit4..7=上/下/左/右。
 */
public final class NativeBridge {

    static {
        System.loadLibrary("infones");
    }

    public static final int PAD_A      = 0;
    public static final int PAD_B      = 1;
    public static final int PAD_SELECT = 2;
    public static final int PAD_START  = 3;
    public static final int PAD_UP     = 4;
    public static final int PAD_DOWN   = 5;
    public static final int PAD_LEFT   = 6;
    public static final int PAD_RIGHT  = 7;

    /** 设置应用私有目录（存档 .srm 所在），onCreate 时调用一次 */
    public static native void nativeInit(String filesDir);

    /** 绑定 / 解绑渲染表面 */
    public static native void nativeSetSurface(Surface surface);
    public static native void nativeClearSurface();

    /** 注入 ROM 字节并执行 InfoNES_Load（含 Reset 与声音初始化）。0=成功 */
    public static native int nativeLoadRom(byte[] data, String name);

    /** 创建模拟线程执行 InfoNES_Main；停止标志先被清除 */
    public static native int nativeStart();

    /** 请求停止并 join 线程（帧尾优雅退出，内核自行 Fin），随后落盘 SRAM */
    public static native void nativeStop();

    public static native boolean nativeIsRunning();

    /** 单个手柄位的按下 / 抬起 */
    public static native void nativeSetPadBit(int bit, boolean down);

    /** 跳帧数 0..9（FrameSkip） */
    public static native void nativeSetFrameSkip(int frames);

    /** 静音开关（APU_Mute） */
    public static native void nativeSetMute(boolean mute);

    private NativeBridge() {
    }
}
