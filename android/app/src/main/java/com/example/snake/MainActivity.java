package com.example.snake;

import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;

import org.libsdl.app.SDLActivity;

/**
 * MainActivity for Snake SDL2 Game
 * 
 * This activity extends SDLActivity to provide SDL2 integration for Android.
 * It handles touch input, keyboard input, and Android lifecycle management.
 */
public class MainActivity extends SDLActivity {
    private static final String TAG = "SnakeSDL2";
    
    // Touch input constants for converting touch to directional input
    private static final float MIN_SWIPE_DISTANCE = 50;
    private float lastTouchX = 0;
    private float lastTouchY = 0;
    private boolean touchStarted = false;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        Log.d(TAG, "Snake SDL2 MainActivity created");
        
        // Keep screen on during gameplay
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        // Hide navigation bar for immersive experience
        hideSystemUI();
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        hideSystemUI();
    }
    
    /**
     * Hide system UI for immersive fullscreen experience
     */
    private void hideSystemUI() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }
    
    /**
     * Get the libraries to load in order
     * This is required by SDLActivity
     */
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "SDL2_mixer", 
            "SDL2_ttf",
            "snake"  // Our main game library
        };
    }
    
    /**
     * Handle touch events and convert them to directional input
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();
        
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                lastTouchX = x;
                lastTouchY = y;
                touchStarted = true;
                return true;
                
            case MotionEvent.ACTION_UP:
                if (touchStarted) {
                    handleSwipeGesture(x, y);
                    touchStarted = false;
                }
                return true;
                
            case MotionEvent.ACTION_MOVE:
                // Optionally handle drag gestures here
                return true;
        }
        
        return super.onTouchEvent(event);
    }
    
    /**
     * Convert swipe gestures to keyboard events for the game
     */
    private void handleSwipeGesture(float endX, float endY) {
        float deltaX = endX - lastTouchX;
        float deltaY = endY - lastTouchY;
        
        // Check if the swipe distance is significant enough
        if (Math.abs(deltaX) < MIN_SWIPE_DISTANCE && Math.abs(deltaY) < MIN_SWIPE_DISTANCE) {
            // Short tap - treat as space key (pause/start)
            sendKeyEvent(KeyEvent.KEYCODE_SPACE, true);
            sendKeyEvent(KeyEvent.KEYCODE_SPACE, false);
            return;
        }
        
        // Determine primary direction
        if (Math.abs(deltaX) > Math.abs(deltaY)) {
            // Horizontal swipe
            if (deltaX > 0) {
                // Right swipe
                sendKeyEvent(KeyEvent.KEYCODE_DPAD_RIGHT, true);
                sendKeyEvent(KeyEvent.KEYCODE_DPAD_RIGHT, false);
                Log.d(TAG, "Swipe RIGHT detected");
            } else {
                // Left swipe  
                sendKeyEvent(KeyEvent.KEYCODE_DPAD_LEFT, true);
                sendKeyEvent(KeyEvent.KEYCODE_DPAD_LEFT, false);
                Log.d(TAG, "Swipe LEFT detected");
            }
        } else {
            // Vertical swipe
            if (deltaY > 0) {
                // Down swipe
                sendKeyEvent(KeyEvent.KEYCODE_DPAD_DOWN, true);
                sendKeyEvent(KeyEvent.KEYCODE_DPAD_DOWN, false);
                Log.d(TAG, "Swipe DOWN detected");
            } else {
                // Up swipe
                sendKeyEvent(KeyEvent.KEYCODE_DPAD_UP, true);
                sendKeyEvent(KeyEvent.KEYCODE_DPAD_UP, false);
                Log.d(TAG, "Swipe UP detected");
            }
        }
    }
    
    /**
     * Send keyboard event to SDL
     */
    private void sendKeyEvent(int keycode, boolean down) {
        // Create synthetic KeyEvent and send to SDL
        KeyEvent keyEvent = new KeyEvent(
            down ? KeyEvent.ACTION_DOWN : KeyEvent.ACTION_UP,
            keycode
        );
        
        // Let SDL handle the key event
        super.onKeyDown(keycode, keyEvent);
    }
    
    /**
     * Handle hardware key events
     */
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        // Handle back button to exit game
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            // Send ESC key to game for graceful exit
            sendKeyEvent(KeyEvent.KEYCODE_ESCAPE, true);
            sendKeyEvent(KeyEvent.KEYCODE_ESCAPE, false);
            return true;
        }
        
        return super.onKeyDown(keyCode, event);
    }
    
    @Override
    protected void onDestroy() {
        Log.d(TAG, "Snake SDL2 MainActivity destroyed");
        super.onDestroy();
    }
}