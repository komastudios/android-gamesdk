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

package com.google.gamesdk.gamecert.operationrunner.operations;

import android.content.Context;
import android.os.SystemClock;
import android.util.Log;

import com.google.gamesdk.gamecert.operationrunner.util.NativeInvoker;
import com.google.gamesdk.gamecert.operationrunner.util.TimeParsing;
import com.google.gson.Gson;
import com.google.gson.annotations.SerializedName;

import java.io.File;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.channels.SeekableByteChannel;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.List;

public class WorkLoadConfigurationTest extends BaseOperation {

    private static final String TAG = "WorkLoadConfigurationTest";

    private static class Configuration {
        String file_location; // "Base APK", "Split APK", "OBB", "Create File"
        String thread_setup; // "Single Core", "All Cores"
        String read_scheme; // "Divided", "Interleaved", "Greedy"
        int data_size;      // size of data to read
        int buffer_size;       // Read buffer size
        int read_align; // Pad the file so reads start at this alignment.
        String performance_sample_period = "250ms";
    }

    private Configuration _configuration;
    private Gson _gson;

    int padded_read_size;
    String file_path;
    int file_size;
    int num_threads;
    private long performanceSamplePeriodMillis;
    GreedySync greedy_synch;

    private static final String create_file = "CREATE_FILE";
    private static final String single_core = "SINGLE_CORE";
    private static final String all_core = "ALL_CORE";
    private static final String divided = "DIVIDED";
    private static final String interleaved = "INTERLEAVED";
    private static final String greedy = "GREEDY";

    Thread _thread;

    public WorkLoadConfigurationTest(String suiteId, String configurationJson,
                                     Context context, Mode mode) {
        super(TAG, suiteId, configurationJson, context, mode);

        _gson = new Gson();
        _configuration = _gson.fromJson(configurationJson, Configuration.class);
    }

    @Override
    public void start(long runForDurationMillis) {
        validateConfig();
        _thread = new Thread(() -> {
            createFileAndData();

            // Thread Setup
            List<ThreadState> threadsState = determineThreadSetup();
            num_threads = threadsState.size();
            Log.d(TAG, "Num threads: " + num_threads);
            if (greedy.equalsIgnoreCase(_configuration.read_scheme)) {
                greedy_synch = new GreedySync();
            }

            List<Thread> threads = new ArrayList<>();
            for (int t = 0; t < threadsState.size(); ++t) {
                ThreadState threadState = threadsState.get(t);
                Thread workerThread = new Thread(() -> {
                    doWork(threadState);
                });
                workerThread.setPriority(Thread.MAX_PRIORITY);
                threads.add(workerThread);
            }

            for (int t = 0; t < threads.size(); ++t) {
                threads.get(t).start();
            }

            for (int t = 0; t < threads.size(); ++t) {
                try {
                    threads.get(t).join();
                } catch (InterruptedException e) {
                    NativeInvoker.fatalError(TAG, "Interrupted Exception");
                }
            }
            deleteFileAndData();
        });
        _thread.start();
    }

    @Override
    public void waitForCompletion() {
        try {
            _thread.join();
        } catch (InterruptedException e) {
            NativeInvoker.fatalError(TAG, "Interrupted Exception");
        }
    }

    class Datum {
        @SerializedName("start_time_millis")
        long startTimeMillis;

        @SerializedName("end_time_millis")
        long endTimeMillis;

        @SerializedName("data_read")
        long data_read;

        @SerializedName("thread_id")
        long thread_id;

        Datum(long startTimeMillis, long thread_id) {
            this.startTimeMillis = startTimeMillis;
            this.thread_id = thread_id;
            this.data_read = 0;
        }
    }

    // determine read scheme type
    void doWork(ThreadState threadState) {
        Log.d(TAG, "Implement work: " + threadState.id);

        long startTimeMillis = SystemClock.elapsedRealtime();
        threadState.datum = new Datum(startTimeMillis, threadState.id);

        if (_configuration.read_scheme.equalsIgnoreCase(divided)) {
            dividedRead(threadState);
        } else if (_configuration.read_scheme.equalsIgnoreCase(interleaved)) {
            interleavedRead(threadState);
        } else {
            greedyRead(threadState);
        }

        reportProgress(threadState);
    }

    // Divided Read
    void dividedRead(ThreadState threadState) {
        Path path = new File(file_path).toPath();
        int dividedSize = file_size / num_threads;
        int fileOffset = dividedSize * threadState.id;
        long endOffset = fileOffset + dividedSize;
        Log.d(TAG, "Thread Id: " + threadState.id
                + ", Start position: " + fileOffset + ", End position = "
                + (fileOffset + dividedSize));

        try {
            SeekableByteChannel readChannel = Files.newByteChannel(path,
                    StandardOpenOption.READ);
            readChannel.position(fileOffset);
            while (!isStopped() && readChannel.position() < endOffset) {
                long bytesLeft = endOffset - readChannel.position();
                int readSize = bytesLeft > padded_read_size ?
                        padded_read_size : (int) bytesLeft;
                ByteBuffer buffer = ByteBuffer.allocate(readSize);
                int bytesRead = readChannel.read(buffer);
                reportProgressIfRequired(threadState, bytesRead);
            }
        } catch (Exception e) {
            NativeInvoker.fatalError(TAG, "Unable to read file, error: "
                    + e.getLocalizedMessage());
            return;
        }
        Log.v(TAG, "Thread Id: " + threadState.id
                + ", Read completed");
    }

    //Interleaved Read
    void interleavedRead(ThreadState threadState) {
        Path path = new File(file_path).toPath();

        int fileOffset = padded_read_size * threadState.id;
        long endOffset = file_size;
        Log.d(TAG, "Thread Id: " + threadState.id
                + ", Start position: " + fileOffset + ", End position = "
                + endOffset);

        try {
            SeekableByteChannel readChannel = Files.newByteChannel(path,
                    StandardOpenOption.READ);
            readChannel.position(fileOffset);
            while (!isStopped() && readChannel.position() < endOffset) {
                long bytesLeft = endOffset - readChannel.position();
                int readSize = bytesLeft > padded_read_size ?
                        padded_read_size : (int) bytesLeft;
                ByteBuffer buffer = ByteBuffer.allocate(readSize);
                int bytesRead = readChannel.read(buffer);
                reportProgressIfRequired(threadState, bytesRead);
                fileOffset += padded_read_size * num_threads;
                readChannel.position(fileOffset);
            }
        } catch (Exception e) {
            NativeInvoker.fatalError(TAG, "Unable to read file, error: "
                    + e.getLocalizedMessage());
            return;
        }
        Log.v(TAG, "Thread Id: " + threadState.id
                + ", Read completed");
    }

    // find next offset for Greedy Read
    class GreedySync {
        int fileOffset;

        GreedySync() {
            fileOffset = 0;
        }

        synchronized int GetNextOffset() {
            int offset = fileOffset;
            fileOffset += padded_read_size;
            return offset;
        }
    }

    // Greedy Read
    void greedyRead(ThreadState threadState) {
        Path path = new File(file_path).toPath();

        int fileOffset = greedy_synch.GetNextOffset();
        long endOffset = file_size;
        Log.d(TAG, "Thread Id: " + threadState.id
                + ", Start position: " + fileOffset + ", End position = "
                + endOffset);

        try {
            SeekableByteChannel readChannel = Files.newByteChannel(path,
                    StandardOpenOption.READ);
            readChannel.position(fileOffset);

            while (!isStopped() && readChannel.position() < endOffset) {
                long bytesLeft = endOffset - readChannel.position();
                int readSize = bytesLeft > padded_read_size ?
                        padded_read_size : (int) bytesLeft;
                ByteBuffer buffer = ByteBuffer.allocate(readSize);
                int bytesRead = readChannel.read(buffer);
                reportProgressIfRequired(threadState, bytesRead);

                fileOffset = greedy_synch.GetNextOffset();
                readChannel.position(fileOffset);
            }
        } catch (Exception e) {
            NativeInvoker.fatalError(TAG, "Unable to read file, error: "
                    + e.getLocalizedMessage());
            return;
        }
        Log.v(TAG, "Thread Id: " + threadState.id
                + ", Read completed");

    }

    void reportProgressIfRequired(ThreadState threadState, int bytesRead) {
        if (bytesRead > 0) {
            threadState.datum.data_read += bytesRead;
        }

        long nowMillis = SystemClock.elapsedRealtime();
        long elapsedMillis = nowMillis - threadState.datum.startTimeMillis;

        if (elapsedMillis >= performanceSamplePeriodMillis) {
            reportProgress(threadState);
            threadState.datum = new Datum(nowMillis, threadState.id);
        }
    }

    void reportProgress(ThreadState threadState) {
        threadState.datum.endTimeMillis = SystemClock.elapsedRealtime();
        report(threadState.datum);
    }

    //validate file, thread & read scheme configuration from configuration.json
    void validateConfig() {
        if (!create_file.equalsIgnoreCase(_configuration.file_location)) {
            throw new IllegalArgumentException(
                    "File location not supported \""
                            + _configuration.file_location + "\"");
        }

        if (!single_core.equalsIgnoreCase(_configuration.thread_setup) &&
                !all_core.equalsIgnoreCase(_configuration.thread_setup)) {
            throw new IllegalArgumentException(
                    "Thread Setup not supported \""
                            + _configuration.thread_setup + "\"");
        }

        if (!divided.equalsIgnoreCase(_configuration.read_scheme) &&
                !interleaved.equalsIgnoreCase(_configuration.read_scheme) &&
                !greedy.equalsIgnoreCase(_configuration.read_scheme)) {
            throw new IllegalArgumentException(
                    "Read scheme not supported \""
                            + _configuration.read_scheme + "\"");
        }

        // Support only balance loads
        if (_configuration.data_size % _configuration.buffer_size != 0) {
            throw new IllegalArgumentException(
                    "Unbalanced read size not supported \""
                            + "data size="
                            + _configuration.data_size + ", "
                            + "buffer size="
                            + _configuration.buffer_size + "\"");
        }

        try {
            performanceSamplePeriodMillis = (long) TimeParsing.parseDurationString(
                    _configuration.performance_sample_period,
                    TimeParsing.Unit.Milliseconds
            );
        } catch (TimeParsing.BadFormatException e) {
            Log.e(TAG, "Unable to parse performance_sample_period, error: " +
                    e.getLocalizedMessage());
            e.printStackTrace();
            return;
        }
    }

    //Create file with size mentioned in configuration.json
    void createFileAndData() {
        int num_reads = _configuration.data_size / _configuration.buffer_size;
        padded_read_size =
                getNextAlignedValue(_configuration.buffer_size,
                        _configuration.read_align);
        file_size = padded_read_size * num_reads;
        int totalDataSize = 0;

        try {
            // create file and write data
            File file = new File(getContext().getFilesDir(), TAG + ".bin");
            file_path = file.getPath();
            file.createNewFile();
            FileOutputStream outputStream = new FileOutputStream(file);

            // "Real" data will increment & cycle with every byte, padding is
            // always 0.
            int pad_size =
                    padded_read_size - _configuration.buffer_size;
            for (int i = 0; i < _configuration.data_size; ) {
                for (int j = 0; j < _configuration.buffer_size; ++j) {
                    outputStream.write((byte) i++);
                    totalDataSize++;
                }
                for (int j = 0; j < pad_size; ++j) {
                    outputStream.write(0);
                    totalDataSize++;
                }
            }
        } catch (Exception e) {
            NativeInvoker.fatalError(TAG, "Unable to create file, error: "
                    + e.getLocalizedMessage());
            return;
        }

        Log.d(TAG, "Created file of size: " + file_size + " with "
                + totalDataSize + " bytes.");
    }

    void deleteFileAndData() {
        try {
            File file = new File(file_path);
            file.delete();
        } catch (Exception e) {
            NativeInvoker.fatalError(TAG, "Failed to delete created file, error: "
                    + e.getLocalizedMessage());
            return;
        }
    }

    class ThreadState {
        ThreadState(int _id) {
            id = _id;
        }

        Datum datum;

        int id;
    }

    List<ThreadState> determineThreadSetup() {
        ArrayList<ThreadState> threads = new ArrayList<>();
        if (_configuration.thread_setup.equalsIgnoreCase(single_core)) {
            threads.add(new ThreadState(0));
        } else if (_configuration.thread_setup.equalsIgnoreCase(all_core)) {
            int cpu_count = getNumberOfProcessors();
            for (int cpu = 0; cpu < cpu_count; ++cpu) {
                threads.add(new ThreadState(cpu));
            }
        }
        return threads;
    }

    int getNextAlignedValue(int value, int align) {
        int offset = align != 0 ? value % align : 0;
        return value + (offset != 0 ? align - offset : 0);
    }

    public int getNumberOfProcessors() {
        // includes hyper threaded logical processors
        int processors = Runtime.getRuntime().availableProcessors();
        return processors;
    }
}
