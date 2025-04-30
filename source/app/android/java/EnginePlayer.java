package com.root3d.player;

import android.app.Activity;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class EnginePlayer extends SurfaceView implements SurfaceHolder.Callback
{
    static
    {
        try
        {
            System.loadLibrary("native-lib");
        }
        catch (UnsatisfiedLinkError e)
        {
            Log.e("Root3D", "Failed to load native library: " + e.getMessage());
        }
    }

    private final Activity activity;
    private SurfaceHolder surfaceHolder;
    private boolean isInitialized = false;
    private final Handler handler = new Handler(Looper.getMainLooper());
    private final Runnable renderTask = new Runnable()
    {
        @Override
        public void run()
        {
            nativeTick();
            handler.post(this); // No postDelayed; FPS controlled in native code
        }
    };

    public EnginePlayer(Activity activity)
    {
        super(activity);
        this.activity = activity;
        nativeSetAssetManager(activity.getAssets());
        initialize();
    }

    public void resume()
    {
        if (surfaceHolder != null && surfaceHolder.getSurface().isValid())
        {
            handler.post(renderTask);
        }

        nativeResume();
    }

    public void pause()
    {
        nativePause();
        handler.removeCallbacks(renderTask);
    }

    public void destroy()
    {
        if (surfaceHolder != null && surfaceHolder.getSurface().isValid())
        {
            handler.removeCallbacks(renderTask);
            nativeUnInitialize();
            isInitialized = false;
        }
    }

    public void configurationChanged(Configuration newConfig)
    {
        // Handle configuration changes if necessary
    }

    public void windowFocusChanged(boolean hasFocus)
    {
        if (hasFocus)
        {
            setImmersiveMode();
        }
    }

    private void initialize()
    {
        hideActionBar();
        setImmersiveMode();
        setupSurfaceView();
    }

    private void hideActionBar()
    {
        if (activity.getActionBar() != null)
        {
            activity.getActionBar().hide();
        }
    }

    private void setImmersiveMode()
    {
        activity.getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        );
    }

    private void setupSurfaceView()
    {
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        surfaceHolder = holder;
        Surface surface = holder.getSurface();
        nativeSetSurface(surface);

        if (!isInitialized)
        {
            nativeInitialize();
            isInitialized = true;
        }
        else
        {
            nativeResize(holder.getSurfaceFrame().width(), holder.getSurfaceFrame().height());
        }

        nativeResume();
        handler.post(renderTask);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        nativeResize(width, height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        handler.removeCallbacks(renderTask);
    }

    // Native methods
    private native void nativeSetSurface(Surface surface);
    private native void nativeSetAssetManager(AssetManager assetManager);
    private native void nativeInitialize();
    private native void nativeUnInitialize();
    private native void nativeResume();
    private native void nativePause();
    private native void nativeTick();
    private native void nativeResize(int width, int height);
}