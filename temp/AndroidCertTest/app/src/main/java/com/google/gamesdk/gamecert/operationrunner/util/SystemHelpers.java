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

package com.google.gamesdk.gamecert.operationrunner.util;

import android.app.ActivityManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;
import android.os.Build;
import android.os.Debug;
import android.os.PowerManager;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.microedition.khronos.opengles.GL10;

/*
 * SystemHelpers is a utility for providing access to system elements like
 * thermal data, packaged assets, etc
 */
@SuppressWarnings({"unused", "WeakerAccess"})
public class SystemHelpers {

    private static final String TAG = "SystemHelpers";
    private Context _context;
    private ActivityManager _activityManager;
    private int _pid;
    private Map<String, Long> _memInfo;
    private int _currentThermalStatus = -1;


    public SystemHelpers(Context context) {
        this._context = context;

        _activityManager =
                (ActivityManager) Objects.requireNonNull(
                        context.getSystemService(Context.ACTIVITY_SERVICE));

        this._pid = loadPids().get(0);
        this._memInfo = loadMeminfo();


        // listen for thermal status changes
        PowerManager pm = (PowerManager)
                context.getSystemService(Context.POWER_SERVICE);

        if (pm != null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            _currentThermalStatus = pm.getCurrentThermalStatus();
            _onThermalStatusChanged();
            pm.addThermalStatusListener(i -> {
                _currentThermalStatus = i;
                _onThermalStatusChanged();
            });
        }
    }

    //--------------------------------------------------------------------------

    /**
     * @return the current thermal status as reported by the system PowerManager
     * see: https://developer.android.com/reference/android/os/PowerManager.html#THERMAL_STATUS_NONE
     */
    public int getCurrentThermalStatus() {
        return _currentThermalStatus;
    }

    private void _onThermalStatusChanged() {
        Log.d(TAG, "_onThermalStatusChanged: " + _currentThermalStatus);
    }

    //--------------------------------------------------------------------------

    public static class TextureInformation {
        public boolean ret;
        public boolean alphaChannel;
        public int originalWidth;
        public int originalHeight;
        public Object image;
    }

    /**
     * loads an image into the current gl texture unit. Note: this function is
     * called from native code via JNI.
     *
     * @param path path to an image in assets/
     * @return TextureInformation on the loaded image
     */
    public TextureInformation loadTexture(String path) {
        Bitmap bitmap = null;
        TextureInformation info = new TextureInformation();
        try {
            String str = path;
            if (!path.startsWith("/")) {
                str = "/" + path;
            }

            File file = new File(_context.getExternalFilesDir(null), str);
            if (file.canRead()) {
                bitmap = BitmapFactory.decodeStream(new FileInputStream(file));
            } else {
                bitmap = BitmapFactory.decodeStream(_context.getResources()
                        .getAssets().open(path));
            }
        } catch (Exception e) {
            Log.w(TAG, "Couldn't load file: \"" + path + "\"");
            info.ret = false;
            return info;
        }

        if (bitmap == null) {
            Log.e(TAG, "Couldn't load image: \"" + path + "\"");
            info.ret = false;
            return info;
        }

        GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
        info.ret = true;
        info.alphaChannel = bitmap.hasAlpha();
        info.originalWidth = bitmap.getWidth();
        info.originalHeight = bitmap.getHeight();
        info.image = bitmap;

        return info;
    }

    /**
     * loads a text file. Note: this function is
     * called from native code via JNI.
     *
     * @param path path to a text file in assets/
     * @return text content of the file
     */
    public String loadText(String path) {
        try {
            InputStream stream = _context.getAssets().open(path);
            int size = stream.available();
            byte[] buffer = new byte[size];
            stream.read(buffer);
            stream.close();
            return new String(buffer);
        } catch (IOException e) {
            Log.e(TAG, "loadText - unable to load text from file \""
                    + path + "\"");
        }

        return null;
    }

    // -------------------------------------------------------------------------

    public static class MemoryInformation {
        // heap allocation size in bytes
        public long nativeHeapAllocationSize;

        // current out of memory score, higher the worse
        public int oomScore;

        // commit limit in bytes
        public long commitLimit;

        // total memory in bytes "accessible to kernel"
        // (via ActivityManager.MemoryInfo)
        public long totalMemory;

        // available memory in system via ActivityManager.MemoryInfo
        // (via ActivityManager.MemoryInfo)
        public long availableMemory;

        // memory use threshold at which point system is going to enter a
        // low memory condition (via ActivityManager.MemoryInfo)
        public long threshold;

        // true if system is in a low memory condition
        // (via ActivityManager.MemoryInfo)
        public boolean lowMemory;
    }

    public void runGc() {
        System.gc();
    }

    public MemoryInformation getMemoryInfo() {
        MemoryInformation info = new MemoryInformation();
        info.nativeHeapAllocationSize = getNativeHeapAllocatedSize();
        info.oomScore = getOomScore();

        // commit limit is in kb, so make bytes
        info.commitLimit = getMemInfo("CommitLimit") * 1024;

        ActivityManager.MemoryInfo mi = getMemoryInfo(_activityManager);
        info.totalMemory = mi.totalMem;
        info.availableMemory = mi.availMem;
        info.threshold = mi.threshold;
        info.lowMemory = mi.lowMemory;

        return info;
    }

    public long getNativeHeapAllocatedSize() {
        return Debug.getNativeHeapAllocatedSize();
    }

    public int getOomScore() {
        try {
            return Integer.parseInt(
                    readFile(("/proc/" + _pid) + "/oom_score"));
        } catch (IOException | NumberFormatException e) {
            return 0;
        }
    }

    public List<String> getMemInfoKeys() {
        return new ArrayList<>(_memInfo.keySet());
    }

    public long getMemInfo(String key) {
        return getOrDefault(_memInfo, key, -1);
    }

    // -------------------------------------------------------------------------

    private static ActivityManager.MemoryInfo
    getMemoryInfo(ActivityManager activityManager) {
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memoryInfo);
        return memoryInfo;
    }

    private static long
    getOrDefault(Map<String, Long> map, String key, long def) {
        Long value = map.get(key);
        return value == null ? def : value;
    }

    private List<Integer> loadPids() {
        List<Integer> pids = new ArrayList<>();
        List<ActivityManager.RunningAppProcessInfo> runningAppProcesses =
                _activityManager.getRunningAppProcesses();
        for (ActivityManager.RunningAppProcessInfo
                runningAppProcessInfo : runningAppProcesses) {
            Log.d(TAG, "Pid: " + runningAppProcessInfo.pid);
            pids.add(runningAppProcessInfo.pid);
        }
        return pids;
    }

    private Map<String, Long> loadMeminfo() {
        Map<String, Long> output = new HashMap<>();

        try {
            String meminfoText = readFile("/proc/meminfo");
            Pattern pattern = Pattern.compile("([^:]+)[^\\d]*(\\d+).*\n");
            Matcher matcher = pattern.matcher(meminfoText);
            while (matcher.find()) {
                output.put(matcher.group(1), Long.parseLong(
                        Objects.requireNonNull(matcher.group(2))));
            }
        } catch (IOException e) {
            // Intentionally silenced
        }
        return output;
    }

    private static String execute(String... args) throws IOException {
        return readStream(new ProcessBuilder(args).start().getInputStream());
    }

    private static String readFile(String filename) throws IOException {
        return readStream(new FileInputStream(filename));
    }

    private static String readStream(InputStream inputStream) throws IOException {
        try (
                InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                BufferedReader reader = new BufferedReader(inputStreamReader)) {
            String newline = System.getProperty("line.separator");
            StringBuilder output = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                if (output.length() > 0) {
                    output.append(newline);
                }
                output.append(line);
            }
            return output.toString();
        }
    }

    private static List<Integer> parseList(String string) {
        Scanner scanner = new Scanner(string);
        List<Integer> list = new ArrayList<>();
        while (scanner.hasNextInt()) {
            list.add(scanner.nextInt());
        }
        return list;
    }
}
