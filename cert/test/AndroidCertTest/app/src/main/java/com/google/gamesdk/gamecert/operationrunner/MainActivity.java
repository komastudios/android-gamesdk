/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.gamesdk.gamecert.operationrunner;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ConfigurationInfo;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.gamesdk.R;
import com.google.gamesdk.gamecert.operationrunner.hosts.GLSurfaceViewHostActivity;
import com.google.gamesdk.gamecert.operationrunner.hosts.SwappyGLHostActivity;
import com.google.gamesdk.gamecert.operationrunner.hosts.VulkanHostActivity;
import com.google.gamesdk.gamecert.operationrunner.transport.Configuration;
import com.google.gamesdk.gamecert.operationrunner.util.NativeInvoker;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Objects;

@SuppressLint("ApplySharedPref")
public class MainActivity extends AppCompatActivity {

    /*
     * Result code returned to MainActivity.onActivityResult when this TestActivity
     * ends successfully (via back button press). If the MainActivity is notified that this
     * activity ended, but without this result code, MainActivity can determine that
     * this activity crashed.
     */
    public static final int RESULT_CODE_STRESS_TEST_FINISHED_SUCCESSFULLY = 1002;

    /*
     * Result code returned to MainActivity.onActivityResult when this TestActivity
     * is cancelled (via back button press). If the MainActivity is notified that this
     * activity ended, but without this result code or RESULT_CODE_STRESS_TEST_FINISHED_SUCCESSFULLY,
     * MainActivity can determine that this activity crashed.
     */
    public static final int RESULT_CODE_STRESS_TEST_CANCELED = 1003;


    private static final String RESULT_FILE_NAME = "report.json";
    private static final String TAG = "MainActivity";

    // ---------------------------------------------------------------------------------------------

    private static final int STRESS_TEST_ACTIVITY_REQUEST_CODE = 1001;

    // ---------------------------------------------------------------------------------------------

    private Configuration _configuration;
    private int _currentStressTestIndex;
    private RecyclerView _testsRecyclerView;
    private StressTestAdapter _testsAdapter;
    private Handler _mainThreadPump;
    private String _reportFilePath;
    private int _reportFileDescriptor = -1;
    private boolean _finishAfterTestCompletion;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setSupportActionBar(findViewById(R.id.toolbar));

        _testsRecyclerView = findViewById(R.id.testsRecyclerView);

        _mainThreadPump = new Handler();
        _configuration = Configuration.loadFromJsonResource(this, R.raw.configuration);
        if (_configuration == null) {
            throw new IllegalStateException("Configuration file didn't load");
        }

        _testsRecyclerView.setHasFixedSize(true);
        _testsRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        _testsAdapter = new StressTestAdapter(_configuration, new StressTestTapListener() {
            @Override
            public void onStressTestTapped(Configuration.StressTest stressTest) {
                MainActivity.this.runStressTest(stressTest, false);
            }

            @Override
            public void onStressTestLongPressed(Configuration.StressTest stressTest) {
                new AlertDialog.Builder(MainActivity.this)
                        .setTitle(R.string.dialog_title_exec_then_terminate)
                        .setMessage(getString(R.string.dialog_message_exec_then_terminate, stressTest.getName()))
                        .setPositiveButton(R.string.dialog_button_execute_then_terminate, (dialogInterface, i) -> {
                            MainActivity.this.runStressTest(stressTest, true);
                        })
                        .setNegativeButton(android.R.string.cancel, null)
                        .setCancelable(true)
                        .show();
            }
        });
        _testsRecyclerView.setAdapter(_testsAdapter);
        setTestsRecyclerViewInteractionEnabled(true);

        // this is the default report file; but if we're running in game loop
        // it will be changed to another location provided by firebase
        _reportFilePath = new File(getFilesDir(), RESULT_FILE_NAME).getAbsolutePath();

        // onCreate shouldn't be called if a test is currently executing. This means the test
        // that was launched crashed and MainActivity was restarted!
        // Show a dialog explaining the situation, reset the flag  to false so that on next
        // run everything's fine, and exit. Otherwise, proceed as normal

        if (isRunningTest()) {
            new AlertDialog.Builder(this)
                    .setTitle(R.string.dialog_title_crash_warning)
                    .setMessage(R.string.dialog_message_crash_warning)
                    .setPositiveButton(android.R.string.ok, (dialogInterface, i) -> finish())
                    .setCancelable(false)
                    .show();

            setIsRunningTest(false);
        } else if (!handleGameLoopLaunch()) {
            openReportLog();
            if (_configuration.isAutoRun()) {
                startAutoRun();
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater menuInflater = getMenuInflater();
        menuInflater.inflate(R.menu.activity_main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_run_all:
                startAutoRun();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        onStressTestCompleted();

        if (requestCode == STRESS_TEST_ACTIVITY_REQUEST_CODE) {
            switch (resultCode) {
                case RESULT_CODE_STRESS_TEST_FINISHED_SUCCESSFULLY:
                    Log.d(TAG, "Stress test activity completed and returned with success");
                    if (_configuration.isAutoRun()) {
                        if (_currentStressTestIndex < numTests() - 1) {
                            _mainThreadPump.postDelayed(() -> {
                                nextTest();
                                runCurrentTest();
                            }, _configuration.getDelayBetweenTestsMillis());
                        } else {
                            // we've executed all tests, we're done here
                            Log.d(TAG, "Finished running all " + numTests() + " tests - stop()-ing");
                            finish();
                        }
                    }
                    break;
                case RESULT_CODE_STRESS_TEST_CANCELED:
                    Log.d(TAG, "Stress test activity canceled");
                    break;
                default:
                    Log.e(TAG, "Stress test activity terminated badly");
                    finish();
            }
        }

        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void finish() {
        NativeInvoker.closeReportFile();
        super.finish();
    }

    private void openReportLog() {
        if (_reportFileDescriptor != -1) {
            NativeInvoker.openReportFileDescriptor(_reportFileDescriptor);
        } else if (_reportFilePath != null) {
            NativeInvoker.openReportFile(_reportFilePath);
        } else {
            finish();
            throw new IllegalStateException("_reportFilePath is null and _reportFileDescriptor is -1; no log file to write to");
        }

        try {
            JSONObject build = new JSONObject();
            build.put("ID", Build.ID);
            build.put("DISPLAY", Build.DISPLAY);
            build.put("PRODUCT", Build.PRODUCT);
            build.put("DEVICE", Build.DEVICE);
            build.put("BOARD", Build.BOARD);
            build.put("MANUFACTURER", Build.MANUFACTURER);
            build.put("BRAND", Build.BRAND);
            build.put("MODEL", Build.MODEL);
            build.put("BOOTLOADER", Build.BOOTLOADER);
            build.put("HARDWARE", Build.HARDWARE);
            build.put("BASE_OS", Build.VERSION.BASE_OS);
            build.put("CODENAME", Build.VERSION.CODENAME);
            build.put("INCREMENTAL", Build.VERSION.INCREMENTAL);
            build.put("RELEASE", Build.VERSION.RELEASE);
            build.put("SDK_INT", Build.VERSION.SDK_INT);
            build.put("PREVIEW_SDK_INT", Build.VERSION.PREVIEW_SDK_INT);
            build.put("SECURITY_PATCH", Build.VERSION.SECURITY_PATCH);
            build.put("OPENGLES", getOpenGLVersion());

            NativeInvoker.writeToReportFile(build.toString());

        } catch (JSONException e) {
            Log.e(TAG, "Error building build info JSON, e: " + e);
            e.printStackTrace();
            finish();
        }
    }

    private boolean handleGameLoopLaunch() {
        Intent launchIntent = getIntent();
        if ("com.google.intent.action.TEST_LOOP".equals(launchIntent.getAction())) {
            Log.i(TAG, "launched via TEST_LOOP game loop mode");

            Uri logFileUri = launchIntent.getData();
            if (logFileUri != null) {
                String logPath = logFileUri.getEncodedPath();
                if (logPath != null) {
                    Log.i(TAG, "TEST_LOOP: game-loop logPath: " + logPath);

                    // we need to get the file descriptor for this URI
                    // https://firebase.google.com/docs/test-lab/android/game-loop
                    _reportFileDescriptor = -1;
                    try {
                        _reportFileDescriptor = getContentResolver()
                                .openAssetFileDescriptor(logFileUri, "w")
                                .getParcelFileDescriptor()
                                .getFd();

                        Log.i(TAG, "resolved game-loop _reportFileDescriptor: "
                                + _reportFileDescriptor);
                    } catch (FileNotFoundException e) {
                        Log.e(TAG,
                                "Unable to get report file descriptor, FileNotFound: "
                                + e.getLocalizedMessage());
                        e.printStackTrace();
                        _reportFileDescriptor = -1;
                    } catch (NullPointerException e) {
                        Log.e(TAG, "Unable to get report file descriptor, NPE: "
                                + e.getLocalizedMessage());
                        e.printStackTrace();
                        _reportFileDescriptor = -1;
                    }
                }
            }
            openReportLog();
            startAutoRun();

            return true;
        }
        return false;
    }

    private String getOpenGLVersion() {
        final ActivityManager activityManager =
                (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        final ConfigurationInfo configurationInfo =
                Objects.requireNonNull(activityManager).getDeviceConfigurationInfo();
        return configurationInfo.getGlEsVersion();
    }

    private void startAutoRun() {
        _configuration.setAutoRun(true);
        setTestsRecyclerViewInteractionEnabled(false);
        _mainThreadPump.postDelayed(
                this::runCurrentTest,
                _configuration.getDelayBetweenTestsMillis());
    }


    private int numTests() {
        return _configuration.getStressTests().length;
    }

    private void nextTest() {
        _currentStressTestIndex++;
    }

    private void runCurrentTest() {
        int idx = _currentStressTestIndex % numTests();
        Configuration.StressTest st = _configuration.getStressTests()[idx];
        runStressTest(st, false);
    }

    private void runStressTest(
            Configuration.StressTest stressTest,
            boolean finishOnCompletion) {

        Intent i;
        String host = stressTest.getHost();
        if (TextUtils.isEmpty(host)) {
            host = _configuration.getHost();
            if (TextUtils.isEmpty(host)) {
                host = SwappyGLHostActivity.ID;
            }
        }
        switch (host) {

            case GLSurfaceViewHostActivity.ID: {
                i = GLSurfaceViewHostActivity.createIntent(this, stressTest);
                break;
            }

            case SwappyGLHostActivity.ID: {
                i = SwappyGLHostActivity.createIntent(this, stressTest);
                break;
            }

            case VulkanHostActivity.ID: {
                i = VulkanHostActivity.createIntent(this, stressTest);
                break;
            }

            default:
                throw new IllegalStateException("Unrecognized host \"" + host + "\"");
        }

        _finishAfterTestCompletion = finishOnCompletion;
        onStressTestStarted();
        startActivityForResult(i, STRESS_TEST_ACTIVITY_REQUEST_CODE);
    }

    private void onStressTestStarted() {
        setTestsRecyclerViewInteractionEnabled(false);
    }

    private void onStressTestCompleted() {
        setTestsRecyclerViewInteractionEnabled(!_configuration.isAutoRun());

        if (_finishAfterTestCompletion) {
            finish();
        }
    }

    private void setIsRunningTest(boolean isRunningTest) {
        ((ACTApplication) getApplication()).setIsRunningTest(isRunningTest);
    }

    private boolean isRunningTest() {
        return ((ACTApplication) getApplication()).isRunningTest();
    }

    private void setTestsRecyclerViewInteractionEnabled(boolean interactionEnabled) {
        _testsRecyclerView.setAlpha(interactionEnabled ? 1 : 0.25f);
        _testsAdapter.setInteractionEnabled(interactionEnabled);
    }

    interface StressTestTapListener {
        void onStressTestTapped(Configuration.StressTest stressTest);

        void onStressTestLongPressed(Configuration.StressTest stressTest);
    }

    /*
     * Adapter to feed the RecyclerView
     */

    private static class StressTestAdapter
            extends RecyclerView.Adapter<StressTestAdapter.StressTestViewHolder> {

        class StressTestViewHolder extends RecyclerView.ViewHolder {
            TextView name;
            TextView description;
            TextView host;
            Configuration.StressTest stressTest;

            StressTestViewHolder(@NonNull View itemView, StressTestTapListener listener) {
                super(itemView);
                this.name = itemView.findViewById(R.id.stressTestName);
                this.host = itemView.findViewById(R.id.stressTestHost);
                this.description = itemView.findViewById(R.id.stressTestDescription);

                itemView.setOnClickListener(view -> {
                    if (_interactionEnabled) {
                        listener.onStressTestTapped(stressTest);
                    }
                });

                itemView.setOnLongClickListener(view -> {
                    if (_interactionEnabled) {
                        listener.onStressTestLongPressed(stressTest);
                    }
                    return true;
                });
            }

            void bind(Configuration.StressTest st) {
                stressTest = st;
                name.setText(st.getName());
                host.setText(st.getHost());
                description.setText(st.getDescription());
            }
        }

        private Configuration _configuration;
        private StressTestTapListener _listener;
        private boolean _interactionEnabled;

        StressTestAdapter(Configuration _configuration,
                          StressTestTapListener listener) {
            this._configuration = _configuration;
            this._listener = listener;
        }

        boolean isInteractionEnabled() {
            return _interactionEnabled;
        }

        void setInteractionEnabled(boolean interactionEnabled) {
            this._interactionEnabled = interactionEnabled;
        }

        @NonNull
        @Override
        public StressTestAdapter.StressTestViewHolder
        onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            View v = LayoutInflater.from(parent.getContext())
                    .inflate(R.layout.stress_test_recycler_item, parent, false);

            return new StressTestViewHolder(v, _listener);
        }

        @Override
        public void
        onBindViewHolder(@NonNull StressTestAdapter.StressTestViewHolder holder,
                         int position) {
            Configuration.StressTest st = _configuration.getStressTests()[position];
            holder.bind(st);
        }

        @Override
        public int getItemCount() {
            return _configuration.getStressTests().length;
        }
    }
}
