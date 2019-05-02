package com.samples.cube;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.Layout;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.SeekBar;
import android.widget.TextView;

public class CubeActivity extends Activity implements SurfaceHolder.Callback  {

    private static final String NUM_CUBES = "com.samples.NUM_CUBES";
    private static final String NUM_BUSY_THREADS = "com.samples.NUM_BUSY_THREADS";
    private static final String APP_NAME = "CubeActivity";

    private SeekBar cubeSeekBar;
    private TextView numCubesText;

    private SeekBar threadSeekBar;
    private TextView numThreadsText;

    // Used to load the 'native-lib' library on application startup.
    static {
        try {
            System.loadLibrary("native-lib");
        } catch (Exception e) {
            Log.e(APP_NAME, "Native code library failed to load.\n" + e);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cube);

        setupCubeSeekBar();
        setupThreadSeekBar();

        SurfaceView surfaceView = findViewById(R.id.surface_view);
        surfaceView.getHolder().addCallback(this);

        Intent intent = getIntent();
        String action = intent.getAction();
        String type = intent.getType();
        if (Intent.ACTION_SEND.equals(action)) {
            handleSendIntent(intent);
        } else if (Intent.ACTION_MAIN.equals(action)) {
            updateNumCubes(1);
            updateNumThreads(0);
        } else {
            Log.e(APP_NAME, "Unknown intent received: " + action);
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        String action = intent.getAction();
        if (Intent.ACTION_SEND.equals(action)) {
            handleSendIntent(intent);
        } else {
            // Do nothing.
        }
    }

    private void handleSendIntent(Intent intent) {
        String numCubesStr = intent.getStringExtra(NUM_CUBES);
        if (numCubesStr != null) {
            updateNumCubes(Integer.parseInt(numCubesStr));
            Log.d(APP_NAME, "Num cubes changed by intent. num_cubes: " + numCubesStr);
        }

        String numThreadsStr = intent.getStringExtra(NUM_BUSY_THREADS);
        if (numThreadsStr != null) {
            updateNumThreads(Integer.parseInt(numThreadsStr));
            Log.d(APP_NAME, "Num threads changed by intent. num_threads: " + numThreadsStr);
        }
    }

    private void setupCubeSeekBar() {
        cubeSeekBar = findViewById(R.id.seekBarCubes);
        numCubesText = findViewById(R.id.textViewCubes);

        cubeSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (!fromUser) {
                    return;
                }

                int newNumCubes = linearToExponential(progress);
                updateNumCubes(newNumCubes, true);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    private void updateNumCubes(int newNumCubes) {
        updateNumCubes(newNumCubes, false);
    }

    private void updateNumCubes(int newNumCubes, boolean fromSeekBar) {
        numCubesText.setText(String.format("Num cubes: %,d", newNumCubes));
        if (!fromSeekBar) {
            cubeSeekBar.setProgress(exponentialToLinear(newNumCubes));
        }
        nChangeNumCubes(newNumCubes);
    }

    static private int linearToExponential(int linearValue) {
        return linearValue == 0 ? 0 : (int)Math.pow(10, (double)linearValue / 1000.0);
    }

    static private int exponentialToLinear(int exponentialValue) {
        return exponentialValue == 0 ? 0 : (int)Math.log10(exponentialValue) * 1000;
    }

    private void setupThreadSeekBar() {
        threadSeekBar = findViewById(R.id.seekBarThreads);
        numThreadsText = findViewById(R.id.textViewThreads);

        threadSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (!fromUser) {
                    return;
                }

                updateNumThreads(progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    private void updateNumThreads(int newNumThreads) {
        updateNumThreads(newNumThreads, false);
    }

    private void updateNumThreads(int newNumThreads, boolean fromSeekBar) {
        numThreadsText.setText(String.format("Num busy threads: %,d", newNumThreads));
        if (!fromSeekBar) {
            threadSeekBar.setProgress(newNumThreads);
        }
        nChangeNumBusyThreads(newNumThreads);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(APP_NAME, "Surface created.");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        nStopCube();
        Log.d(APP_NAME, "Surface destroyed.");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        nStopCube();
        Surface surface = holder.getSurface();
        nStartCube(surface);
    }

    /**
     * Native methods that are implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void nStartCube(Surface holder);
    public native void nStopCube();
    public native void nChangeNumCubes(int newNumCubes);
    public native void nChangeNumBusyThreads(int newNumThreads);
}
