package com.example.root3d;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Handler;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.content.res.AssetManager;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    // Load native libraries
    static {
        System.loadLibrary("native-lib");
    }

    private Handler handler = new Handler();
    private Runnable renderTask = new Runnable() {
        @Override
        public void run() {
            nativeTick();
            handler.post(this); // No postDelayed; FPS controlled in native code
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Set full-screen mode
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        // Hide action bar
        if (getSupportActionBar() != null) {
            getSupportActionBar().hide();
        }

        // Set immersive mode for UI
        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE |
                        View.SYSTEM_UI_FLAG_FULLSCREEN |
                        View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                        View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                        View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
        );

        // Setup SurfaceView for rendering
        SurfaceView surfaceView = new SurfaceView(this);
        surfaceView.setLayoutParams(new WindowManager.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.MATCH_PARENT));

        setContentView(surfaceView);
        surfaceView.getHolder().addCallback(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        handler.post(renderTask);
    }

    @Override
    protected void onPause() {
        super.onPause();
        handler.removeCallbacks(renderTask);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        nativeInitialize(holder.getSurface(), getAssets());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        // Nothing here at the moment
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        handler.removeCallbacks(renderTask);
        nativeInitialize(null, null);
    }

    // Native methods for lifecycle and rendering
    private native void nativeOnStart();
    private native void nativeOnResume();
    private native void nativeOnPause();
    private native void nativeOnStop();

    // ToDo - Split init of surface and assetManager
    private native void nativeInitialize(Surface surface, AssetManager assetManager);
    private native void nativeTick();
}
