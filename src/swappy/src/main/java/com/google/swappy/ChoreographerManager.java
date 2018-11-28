package com.google.swappy;

import android.os.Handler;
import android.os.Looper;
import android.view.Choreographer;


public class ChoreographerManager implements Choreographer.FrameCallback {

    private static final String LOG_TAG = "ChoreographerManager";
    private long mCookie;
    private LooperThread mLooper;

    private class LooperThread extends Thread {
        public Handler mHandler;

        public void run() {
            Looper.prepare();

            mHandler = new Handler();

            Looper.loop();
        }
    }

    public ChoreographerManager(long cookie) {
        mCookie = cookie;
        mLooper = new LooperThread();
        mLooper.start();
    }

    public void postFrameCallback() {
        mLooper.mHandler.post(new Runnable() {
            @Override
            public void run() {
                Choreographer.getInstance().postFrameCallback(ChoreographerManager.this);
            }
        });
    }

    public void postFrameCallbackDelayed(long delayMillis) {
        Choreographer.getInstance().postFrameCallbackDelayed(this, delayMillis);
    }

    public void Terminate() {
        mLooper.mHandler.getLooper().quit();
    }

    @Override
    public void doFrame(long frameTimeNanos) {
        nOnChoreographer(mCookie, frameTimeNanos);
    }

    public native void nOnChoreographer(long cookie, long frameTimeNanos);

}
