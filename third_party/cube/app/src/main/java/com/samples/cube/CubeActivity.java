package com.samples.cube;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;

public class CubeActivity extends Activity implements SurfaceHolder.Callback  {

    private static final String NUM_CUBES = "com.samples.NUM_CUBES";
    private static final String CPU_WORKLOAD = "com.samples.CPU_WORKLOAD";
    private static final String APP_NAME = "CubeActivity";

    private LinearLayout settingsLayout;

    private SeekBar cubeSeekBar;
    private TextView numCubesText;
    private SeekBar cpuWorkSeekBar;
    private TextView cpuWorkText;

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
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_cube);
        settingsLayout = findViewById(R.id.settingsLayout);

        setupCubeSeekBar();
        setupCpuWorkSeekBar();

        SurfaceView surfaceView = findViewById(R.id.surface_view);
        surfaceView.getHolder().addCallback(this);
        surfaceView.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                if (event.getActionMasked() == MotionEvent.ACTION_UP) {
                    toggleOptionsPanel();
                }
                return true;
            }
        });

        Intent intent = getIntent();
        String action = intent.getAction();
        String type = intent.getType();
        if (Intent.ACTION_SEND.equals(action)) {
            handleSendIntent(intent);
        } else if (Intent.ACTION_MAIN.equals(action)) {
            updateNumCubes(1);
            updateCpuWork(0);
        } else {
            Log.e(APP_NAME, "Unknown intent received: " + action);
        }
    }

    private void toggleOptionsPanel() {
        if (settingsLayout.getVisibility() == View.GONE) {
            settingsLayout.setVisibility(View.VISIBLE);
        } else {
            settingsLayout.setVisibility(View.GONE);
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
        String cpuWorkStr = intent.getStringExtra(CPU_WORKLOAD);
        if (cpuWorkStr != null) {
            updateCpuWork(Integer.parseInt(cpuWorkStr));
            Log.d(APP_NAME, "CPU work changed by intent. cpu_workload: " + cpuWorkStr);
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

    private void setupCpuWorkSeekBar() {
      cpuWorkSeekBar = findViewById(R.id.seekBarCpuWork);
      cpuWorkText = findViewById(R.id.textViewCpuWork);

      cpuWorkSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
          @Override
          public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
              if (!fromUser) {
                  return;
              }

              int newCpuWork = linearToExponential(progress);
              updateCpuWork(newCpuWork, true);
          }

          @Override
          public void onStartTrackingTouch(SeekBar seekBar) {}
          @Override
          public void onStopTrackingTouch(SeekBar seekBar) {}
      });
    }

    private void updateCpuWork(int newCpuWork) {
        updateCpuWork(newCpuWork, false);
    }

    private void updateCpuWork(int newCpuWork, boolean fromSeekBar) {
        cpuWorkText.setText(String.format("CPU Work: %,d", newCpuWork));
        if (!fromSeekBar) {
            cpuWorkSeekBar.setProgress(exponentialToLinear(newCpuWork));
        }
        nUpdateCpuWorkload(newCpuWork);
    }

    static private int linearToExponential(int linearValue) {
        return linearValue == 0 ? 0 : (int)Math.pow(10, (double)linearValue / 1000.0);
    }

    static private int exponentialToLinear(int exponentialValue) {
        return exponentialValue == 0 ? 0 : (int)Math.log10(exponentialValue) * 1000;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(APP_NAME, "Surface created.");
        Surface surface = holder.getSurface();
        nStartCube(surface);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(APP_NAME, "Surface destroyed.");
        nStopCube();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {}

    /**
     * Native methods that are implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void nStartCube(Surface holder);
    public native void nStopCube();
    public native void nChangeNumCubes(int newNumCubes);
    public native void nUpdateCpuWorkload(int newWorkload);
}
