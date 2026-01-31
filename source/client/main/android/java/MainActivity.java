package com.root3d.player;

import android.content.res.Configuration;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import com.root3d.player.EnginePlayer;

public class MainActivity extends AppCompatActivity
{
    private EnginePlayer enginePlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        enginePlayer = new EnginePlayer(this);
        setContentView(enginePlayer);
        enginePlayer.requestFocus();
    }

    @Override
    protected void onDestroy()
    {
        enginePlayer.destroy();
        super.onDestroy();
    }

    @Override
    protected void onStop()
    {
        super.onStop();
        enginePlayer.stop();
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        enginePlayer.start();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        enginePlayer.pause();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        enginePlayer.resume();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {
        super.onConfigurationChanged(newConfig);
        enginePlayer.configurationChanged(newConfig);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);
        enginePlayer.windowFocusChanged(hasFocus);
    }
}