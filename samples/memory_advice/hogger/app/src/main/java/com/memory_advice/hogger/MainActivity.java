package com.memory_advice.hogger;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("memory_advice");
        System.loadLibrary("native-lib");
    }

    private final class CheckMemoryAdviceTask extends TimerTask {
        @Override public void run() {
            // Post a task to update the memory on the UI thread.
            getWindow().getDecorView().post(() -> {
                    showMemoryAdvice(getMemoryAdvice(), getMemoryState());
                });
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initMemoryAdvice();
        if (initMemoryAdvice()==0) {
            showMemoryAdvice("Error initializing memory_advice", 0);
        } else {
            showMemoryAdvice(getMemoryAdvice(), getMemoryState());
        }

        Timer timer = new Timer();
        timer.scheduleAtFixedRate(new CheckMemoryAdviceTask(), 0, 1000);

        findViewById(R.id.allocate_button).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                onAllocate();
            }
        });
        findViewById(R.id.deallocate_button).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                onDeallocate();
            }
        });
    }

    static final long BYTES_TO_ADD = 100000000; // 100MB
    long memory_allocated;

    public void onAllocate() {
        new Thread(() ->  {
                allocate(BYTES_TO_ADD);
                memory_allocated += BYTES_TO_ADD;
                updateMemoryAllocated();
            }).start();
    }

    public void onDeallocate() {
        new Thread(() -> {
            if (deallocate()) {
                memory_allocated -= BYTES_TO_ADD;
                updateMemoryAllocated();
            }
        }).start();
    }

    private void updateMemoryAllocated() {
        getWindow().getDecorView().post(() -> {
            showMemoryAllocated();
        });
    }

    private void showMemoryAdvice(String advice, int state) {
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(advice);
        String stateString = "UNKNOWN";
        switch(state) {
            case 1:
                stateString = "OK";
                break;
            case 2:
                stateString = "APPROACHING_LIMIT";
                break;
            case 3:
                stateString = "CRITICAL";
                break;
        }
        TextView state_tv = findViewById(R.id.state_textview);
        state_tv.setText("Memory State: " + stateString);
    }

    private void showMemoryAllocated() {
        // Example of a call to a native method
        TextView tv = findViewById(R.id.allocated_textview);
        tv.setText(String.format("%d MB", memory_allocated/1000000));
    }

    public native int initMemoryAdvice();
    public native String getMemoryAdvice();
    public native int getMemoryState();
    public native void allocate(long bytes);
    public native boolean deallocate();

}