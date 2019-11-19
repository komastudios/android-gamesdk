package com.google.gamesdk.gamecert.operationrunner.operations;

import android.content.Context;
import android.os.SystemClock;
import android.util.Log;

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
    }

    private Configuration _configuration;
    private Gson _gson;

    int padded_read_size;
    String file_path;
    int file_size;
    int num_threads;
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
            List<ThreadConfig> threadSetup = determineThreadSetups();
            num_threads = threadSetup.size();
            Log.d(TAG, "Num threads: " + num_threads);
            if (greedy.equalsIgnoreCase(_configuration.read_scheme)) {
                greedy_synch = new GreedySync();
            }

            List<Thread> threads = new ArrayList<>();
            for (int t = 0; t < threadSetup.size(); ++t) {
                ThreadConfig threadConfig = threadSetup.get(t);
                Thread workerThread = new Thread(() -> {
                    doWork(threadConfig);
                });
                workerThread.setPriority(Thread.MAX_PRIORITY);
                threads.add(workerThread);
            }

            long startTimeMillis = SystemClock.elapsedRealtime();
            Datum datum = new Datum(startTimeMillis);

            for (int t = 0; t < threads.size(); ++t) {
                threads.get(t).start();
            }

            for (int t = 0; t < threads.size(); ++t) {
                try {
                    threads.get(t).join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            long nowMillis = SystemClock.elapsedRealtime();
            long elapsedMillis = nowMillis - startTimeMillis;
            datum.endTimeMillis = elapsedMillis;
            report(datum);
            deleteFileAndData();
        });
        _thread.start();
    }

    @Override
    public void waitForCompletion() {
        try {
            _thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    class Datum {
        @SerializedName("start_time_millis")
        long startTimeMillis;

        @SerializedName("end_time_millis")
        long endTimeMillis;

        Datum(long startTimeMillis) {
            this.startTimeMillis = startTimeMillis;
        }
    }

    // determine read scheme type
    void doWork(ThreadConfig threadConfiguration) {
        Log.d(TAG, "Implement work: " + threadConfiguration.id);
        if (_configuration.read_scheme.equalsIgnoreCase(divided)) {
            dividedRead(threadConfiguration);
        } else if (_configuration.read_scheme.equalsIgnoreCase(interleaved)) {
            interleavedRead(threadConfiguration);
        } else {
            greedyRead(threadConfiguration);
        }
    }

    // Divided Read
    void dividedRead(ThreadConfig threadConfig) {
        Path path = new File(file_path).toPath();
        int dividedSize = file_size / num_threads;
        int fileOffset = dividedSize * threadConfig.id;
        long endOffset = fileOffset + dividedSize;
        Log.d(TAG, "Thread Id: " + threadConfig.id
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
                readChannel.read(buffer);
            }
        } catch (Exception e) {
            Log.e(TAG, "Unable to read file, error: "
                    + e.getLocalizedMessage());
            e.printStackTrace();
            return;
        }
        Log.v(TAG, "Thread Id: " + threadConfig.id
                + ", Read completed");
    }

    //Interleaved Read
    void interleavedRead(ThreadConfig threadConfig) {
        Path path = new File(file_path).toPath();

        int fileOffset = padded_read_size * threadConfig.id;
        long endOffset = file_size;
        Log.d(TAG, "Thread Id: " + threadConfig.id
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
                readChannel.read(buffer);
                fileOffset += padded_read_size * num_threads;
                readChannel.position(fileOffset);
            }
        } catch (Exception e) {
            Log.e(TAG, "Unable to read file, error: "
                    + e.getLocalizedMessage());
            e.printStackTrace();
            return;
        }
        Log.v(TAG, "Thread Id: " + threadConfig.id
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
    void greedyRead(ThreadConfig threadConfig) {
        Path path = new File(file_path).toPath();

        int fileOffset = greedy_synch.GetNextOffset();
        long endOffset = file_size;
        Log.d(TAG, "Thread Id: " + threadConfig.id
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
                readChannel.read(buffer);
                fileOffset = greedy_synch.GetNextOffset();
                readChannel.position(fileOffset);
            }
        } catch (Exception e) {
            Log.e(TAG, "Unable to read file, error: "
                    + e.getLocalizedMessage());
            e.printStackTrace();
            return;
        }
        Log.v(TAG, "Thread Id: " + threadConfig.id
                + ", Read completed");

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
            Log.e(TAG, "Unable to create file, error: "
                    + e.getLocalizedMessage());
            e.printStackTrace();
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
            Log.e(TAG, "Failed to delete created file, error: "
                    + e.getLocalizedMessage());
            e.printStackTrace();
            return;
        }
    }

    class ThreadConfig {
        ThreadConfig(int _id) {
            id = _id;
        }

        int id;
    }

    List<ThreadConfig> determineThreadSetups() {
        ArrayList<ThreadConfig> threads = new ArrayList<>();
        if (_configuration.thread_setup.equalsIgnoreCase(single_core)) {
            threads.add(new ThreadConfig(0));
        } else if (_configuration.thread_setup.equalsIgnoreCase(all_core)) {
            int cpu_count = getNumberOfProcessors();
            for (int cpu = 0; cpu < cpu_count; ++cpu) {
                threads.add(new ThreadConfig(cpu));
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
