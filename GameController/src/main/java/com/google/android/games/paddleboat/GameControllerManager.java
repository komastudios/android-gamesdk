// Copyright (C) 2021 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.google.android.games.paddleboat;

import android.app.Activity;
import android.content.Context;
import android.hardware.input.InputManager;
import android.os.Build;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.MotionEvent;

import java.util.ArrayList;
import java.util.List;

public class GameControllerManager {
    public static final int MAX_GAMECONTROLLERS = 8;
    // MAX_MICE is for actual physical mice/touchpads, game controller
    // devices that have MOUSE sources for simulating a mouse are not included. We support
    // 2 for the use case of a laptop with a touchpad and an external mouse connected.
    public static final int MAX_MICE = 2;
    public static final int VIBRATION_EFFECT_MIN_API = 26;
    private static final String TAG = "GameControllerManager";
    private static final int GAMECONTROLLER_SOURCE_MASK =
        InputDevice.SOURCE_DPAD | InputDevice.SOURCE_JOYSTICK | InputDevice.SOURCE_GAMEPAD;
    private static final int MOUSE_SOURCE_MASK = InputDevice.SOURCE_MOUSE;
    private static final int VIBRATOR_MANAGER_MIN_API = 31;
    private boolean nativeReady;
    private boolean printControllerInfo;
    private InputManager inputManager;
    private ArrayList<Integer> mouseDeviceIds;
    private ArrayList<Integer> pendingControllerDeviceIds;
    private ArrayList<Integer> pendingMouseDeviceIds;
    private ArrayList<GameControllerInfo> gameControllers;
    private GameControllerThread gameControllerThread;

    public GameControllerManager(Activity appActivity, String libraryName,
        boolean appPrintControllerInfo) {
        if (libraryName == null) {
            // If we aren't passed a shared library name, we assume that our application is
            // using the libpaddleboat.so shared library instead of statically linking
            // paddleboat into their own shared library
            System.loadLibrary("paddleboat");
        } else {
            System.loadLibrary(libraryName);
        }

        if (appPrintControllerInfo) {
            Log.d(TAG, "device Info:" +
                    "\n  BRAND: " + Build.BRAND +
                    "\n DEVICE: " + Build.DEVICE +
                    "\n  MANUF: " + Build.MANUFACTURER +
                    "\n  MODEL: " + Build.MODEL +
                    "\nPRODUCT: " + Build.PRODUCT +
                    "\n    API: " + Build.VERSION.SDK_INT);
        }

        nativeReady = false;
        inputManager = (InputManager) appActivity.getSystemService(Context.INPUT_SERVICE);
        printControllerInfo = appPrintControllerInfo;
        mouseDeviceIds = new ArrayList<Integer>(MAX_GAMECONTROLLERS);
        pendingControllerDeviceIds = new ArrayList<Integer>(MAX_GAMECONTROLLERS);
        pendingMouseDeviceIds = new ArrayList<Integer>(MAX_GAMECONTROLLERS);
        gameControllers = new ArrayList<GameControllerInfo>(MAX_GAMECONTROLLERS);

        // Queue up initially connected devices to be processed when
        // the native side signals it is ready
        scanDevices();
    }

    static public boolean isVibrationSupportedForDevice(Vibrator deviceVibrator) {
        if (Build.VERSION.SDK_INT >= GameControllerManager.VIBRATION_EFFECT_MIN_API) {
            if (deviceVibrator != null) {
                if (deviceVibrator.hasVibrator()) {
                    return true;
                }
            }
        }
        return false;
    }

    public InputManager getAppInputManager() {
        return inputManager;
    }

    public void onPause() {
        if (gameControllerThread != null) {
            gameControllerThread.onPause();
        }
    }

    public void onResume() {
        if (gameControllerThread != null) {
            scanDevices();
            gameControllerThread.onResume();
        } else {
            gameControllerThread = new GameControllerThread();
            gameControllerThread.setGameControllerManager(this);
            gameControllerThread.start();
        }
    }

    void checkForControllerRemovals(int[] deviceIds) {
        if (!nativeReady) {
            for (int index = 0; index < pendingControllerDeviceIds.size(); ++index) {
                boolean foundDevice = false;
                for (int deviceId : deviceIds) {
                    if (pendingControllerDeviceIds.get(index) == deviceId) {
                        foundDevice = true;
                        break;
                    }
                }
                if (!foundDevice) {
                    pendingControllerDeviceIds.remove(index--);
                }
            }
        }
        for (int index = 0; index < gameControllers.size(); ++index) {
            boolean foundDevice = false;
            for (int deviceId : deviceIds) {
                if (gameControllers.get(index).GetGameControllerDeviceId() == deviceId) {
                    foundDevice = true;
                    break;
                }
            }
            if (!foundDevice) {
                onInputDeviceRemoved(gameControllers.get(index).GetGameControllerDeviceId());
            }
        }
    }

    void checkForMouseRemovals(int[] deviceIds) {
        if (!nativeReady) {
            for (int index = 0; index < pendingMouseDeviceIds.size(); ++index) {
                boolean foundDevice = false;
                for (int deviceId : deviceIds) {
                    if (pendingMouseDeviceIds.get(index) == deviceId) {
                        foundDevice = true;
                        break;
                    }
                }
                if (!foundDevice) {
                    pendingMouseDeviceIds.remove(index--);
                }
            }
        }
        for (int index = 0; index < mouseDeviceIds.size(); ++index) {
            int mouseDeviceId = mouseDeviceIds.get(index);
            boolean foundDevice = false;
            for (int deviceId : deviceIds) {
                if (mouseDeviceId == deviceId) {
                    foundDevice = true;
                    break;
                }
            }
            if (!foundDevice) {
                onInputDeviceRemoved(mouseDeviceId);
            }
        }
    }

    void processControllerAddition(int deviceId) {
        Log.d(TAG, "processControllerAddition deviceId: " + deviceId);
        boolean foundDevice = false;
        if (!nativeReady) {
            for (int index = 0; index < pendingControllerDeviceIds.size(); ++index) {
                if (pendingControllerDeviceIds.get(index) == deviceId) {
                    foundDevice = true;
                    break;
                }
            }
            if (!foundDevice) {
                pendingControllerDeviceIds.add(deviceId);
            }
        } else {
            for (int index = 0; index < gameControllers.size(); ++index) {
                if (gameControllers.get(index).GetGameControllerDeviceId() == deviceId) {
                    foundDevice = true;
                    break;
                }
            }
            if (!foundDevice) {
                onGameControllerAdded(deviceId, isDeviceOfSource(deviceId, MOUSE_SOURCE_MASK));
            }
        }
    }

    void processMouseAddition(int deviceId) {
        boolean foundDevice = false;
        if (!nativeReady) {
            for (int index = 0; index < pendingMouseDeviceIds.size(); ++index) {
                if (pendingMouseDeviceIds.get(index) == deviceId) {
                    foundDevice = true;
                    break;
                }
            }
            if (!foundDevice) {
                pendingMouseDeviceIds.add(deviceId);
            }
        } else {
            for (int index = 0; index < mouseDeviceIds.size(); ++index) {
                if (mouseDeviceIds.get(index) == deviceId) {
                    foundDevice = true;
                    break;
                }
                if (!foundDevice) {
                    onMouseAdded(deviceId);
                }
            }
        }
    }

    void scanDevices() {
        // We don't get connection/disconnection messages for devices while in the background.
        // On resume, scan the device list to see if we missed any connections or disconnections.
        int[] deviceIds = inputManager.getInputDeviceIds();

        // Scan for additions
        for (int deviceId : deviceIds) {
            boolean isGameController = isDeviceOfSource(deviceId, GAMECONTROLLER_SOURCE_MASK);
            boolean isMouse = isDeviceOfSource(deviceId, MOUSE_SOURCE_MASK);

            if (isMouse && !isGameController) {
                processMouseAddition(deviceId);
            } else if (isGameController) {
                processControllerAddition(deviceId);
            }
        }

        // Scan for controller and mouse removals
        checkForControllerRemovals(deviceIds);
        checkForMouseRemovals(deviceIds);
    }

    GameControllerInfo onGameControllerAdded(int deviceId, boolean hasVirtualMouse) {
        GameControllerInfo gameControllerInfo = null;
        if (gameControllers.size() < MAX_GAMECONTROLLERS) {
            if (printControllerInfo) {
                Log.d(TAG, "onGameControllerDeviceAdded");
                logControllerInfo(deviceId);
            }
            gameControllerInfo = new GameControllerInfo(InputDevice.getDevice(deviceId),
                hasVirtualMouse);
            gameControllers.add(gameControllerInfo);
            notifyNativeConnection(gameControllerInfo);
        }
        return gameControllerInfo;
    }

    void onMouseAdded(int deviceId) {
        if (mouseDeviceIds.size() < MAX_MICE) {
            if (printControllerInfo) {
                Log.d(TAG, "onMouseDeviceAdded id: " + deviceId + " name: " +
                    InputDevice.getDevice(deviceId).getName());
                logControllerInfo(deviceId);
            }
            mouseDeviceIds.add(deviceId);
            onMouseConnected(deviceId);
        }
    }

    void onGameControllerDeviceRemoved(int deviceId) {
        // Remove from pending connected if it hadn't been processed yet
        for (int index = 0; index < pendingControllerDeviceIds.size(); ++index) {
            if (pendingControllerDeviceIds.get(index) == deviceId) {
                pendingControllerDeviceIds.remove(index--);
                break;
            }
        }

        for (int index = 0; index < gameControllers.size(); ++index) {
            if (gameControllers.get(index).GetGameControllerDeviceId() == deviceId) {
                if (nativeReady) {
                    onControllerDisconnected(deviceId);
                }
                gameControllers.remove(index);
                break;
            }
        }
    }

    boolean onMouseDeviceRemoved(int deviceId) {
        boolean removed = false;
        // Remove from pending connected if it hadn't been processed yet
        for (int index = 0; index < pendingMouseDeviceIds.size(); ++index) {
            if (pendingMouseDeviceIds.get(index) == deviceId) {
                pendingMouseDeviceIds.remove(index--);
                removed = true;
                break;
            }
        }

        for (int index = 0; index < mouseDeviceIds.size(); ++index) {
            if (mouseDeviceIds.get(index) == deviceId) {
                if (nativeReady) {
                    onMouseDisconnected(deviceId);
                }
                mouseDeviceIds.remove(index);
                removed = true;
                break;
            }
        }
        return removed;
    }

    public void onInputDeviceAdded(int deviceId) {
        boolean isGameController = isDeviceOfSource(deviceId, GAMECONTROLLER_SOURCE_MASK);
        boolean isMouse = isDeviceOfSource(deviceId, MOUSE_SOURCE_MASK);

        if (isMouse && !isGameController) {
            processMouseAddition(deviceId);
        } else if (isGameController) {
            processControllerAddition(deviceId);
        }
    }

    public void onInputDeviceRemoved(int deviceId) {
        if (!onMouseDeviceRemoved(deviceId) ) {
            onGameControllerDeviceRemoved(deviceId);
        }
    }

    public void onInputDeviceChanged(int deviceId) {
    }

    public int getApiLevel() {
        return Build.VERSION.SDK_INT;
    }

    public void setNativeReady() {
        nativeReady = true;
        Log.d(TAG, "setNativeReady");

        // Send any pending notifications about connected devices now that native side is ready
        for (int deviceId : pendingControllerDeviceIds) {
            GameControllerInfo gcInfo = onGameControllerAdded(deviceId,
                isDeviceOfSource(deviceId, MOUSE_SOURCE_MASK));
            if (gcInfo != null) {
                if (printControllerInfo) {
                    Log.d(TAG, "setNativeReady notifyNativeConnection for deviceId: " + deviceId);
                }
            }
        }
        pendingControllerDeviceIds.clear();

        for (int deviceId : pendingMouseDeviceIds) {
            onMouseAdded(deviceId);
        }
    }

    public void setVibration(int deviceId, int leftIntensity, int leftDuration, int rightIntensity,
        int rightDuration) {
        InputDevice inputDevice = inputManager.getInputDevice(deviceId);
        if (inputDevice != null) {
            Vibrator deviceVibrator = inputDevice.getVibrator();
            if (isVibrationSupportedForDevice(deviceVibrator)) {
                if (leftIntensity == 0) {
                    deviceVibrator.cancel();
                } else {
                    deviceVibrator.vibrate(VibrationEffect.createOneShot((long) leftDuration,
                        leftIntensity));
                }
            }
        }
    }

    public String getDeviceNameById(int deviceId) {
        InputDevice inputDevice = inputManager.getInputDevice(deviceId);
        if (inputDevice != null) {
            return inputDevice.getName();
        }
        return "";
    }

    private void notifyNativeConnection(GameControllerInfo gcInfo) {
        onControllerConnected(gcInfo.GetGameControllerDeviceInfoArray(),
                gcInfo.GetGameControllerAxisMinArray(),
                gcInfo.GetGameControllerAxisMaxArray(),
                gcInfo.GetGameControllerAxisFlatArray(),
                gcInfo.GetGameControllerAxisFuzzArray());
    }

    private boolean isDeviceOfSource(int deviceId, int matchingSourceMask) {
        boolean isSource = false;
        InputDevice inputDevice = InputDevice.getDevice(deviceId);
        int inputDeviceSources = inputDevice.getSources();
        int sourceMask = InputDevice.SOURCE_ANY & matchingSourceMask;

        if (inputDevice.isVirtual() == false) {
            if ((inputDeviceSources & sourceMask) != 0) {
                List<InputDevice.MotionRange> motionRanges = inputDevice.getMotionRanges();
                if (motionRanges.size() > 0) {
                    isSource = true;
                }
            }
        }

        return isSource;
    }

    private String generateSourceString(int source) {
        String sourceString = "Source Classes: ";
        int sourceMasked = source & InputDevice.SOURCE_ANY;
        int sourceClass = source & InputDevice.SOURCE_CLASS_MASK;

        if ((sourceClass & InputDevice.SOURCE_CLASS_BUTTON) != 0)
            sourceString += "BUTTON ";
        if ((sourceClass & InputDevice.SOURCE_CLASS_JOYSTICK) != 0)
            sourceString += "JOYSTICK ";
        if ((sourceClass & InputDevice.SOURCE_CLASS_POINTER) != 0)
            sourceString += "POINTER ";
        if ((sourceClass & InputDevice.SOURCE_CLASS_POSITION) != 0)
            sourceString += "POSITION ";
        if ((sourceClass & InputDevice.SOURCE_CLASS_TRACKBALL) != 0)
            sourceString += "TRACKBALL ";

        sourceString += "\nSources: ";

        if ((sourceMasked & InputDevice.SOURCE_BLUETOOTH_STYLUS) != 0)
            sourceString += "BLUETOOTH_STYLUS ";
        if ((sourceMasked & InputDevice.SOURCE_DPAD) != 0)
            sourceString += "DPAD ";
        if ((sourceMasked & InputDevice.SOURCE_HDMI) != 0)
            sourceString += "HDMI ";
        if ((sourceMasked & InputDevice.SOURCE_JOYSTICK) != 0)
            sourceString += "JOYSTICK ";
        if ((sourceMasked & InputDevice.SOURCE_KEYBOARD) != 0)
            sourceString += "KEYBOARD ";
        if ((sourceMasked & InputDevice.SOURCE_MOUSE) != 0)
            sourceString += "MOUSE ";
        if ((sourceMasked & InputDevice.SOURCE_MOUSE_RELATIVE) != 0)
            sourceString += "MOUSE_RELATIVE ";
        if ((sourceMasked & InputDevice.SOURCE_ROTARY_ENCODER) != 0)
            sourceString += "ROTARY_ENCODER ";
        if ((sourceMasked & InputDevice.SOURCE_STYLUS) != 0)
            sourceString += "STYLUS ";
        if ((sourceMasked & InputDevice.SOURCE_TOUCHPAD) != 0)
            sourceString += "TOUCHPAD ";
        if ((sourceMasked & InputDevice.SOURCE_TOUCHSCREEN) != 0)
            sourceString += "TOUCHSCREEN ";
        if ((sourceMasked & InputDevice.SOURCE_TOUCH_NAVIGATION) != 0)
            sourceString += "TOUCH_NAVIGATION ";
        if ((sourceMasked & InputDevice.SOURCE_TRACKBALL) != 0)
            sourceString += "TRACKBALL ";

        return sourceString;
    }

    private String getAxisString(int axis) {
        switch (axis) {
            case MotionEvent.AXIS_BRAKE:
                return "AXIS_BRAKE";
            case MotionEvent.AXIS_DISTANCE:
                return "AXIS_DISTANCE";
            case MotionEvent.AXIS_GAS:
                return "AXIS_GAS";
            case MotionEvent.AXIS_GENERIC_1:
                return "AXIS_GENERIC_1";
            case MotionEvent.AXIS_GENERIC_2:
                return "AXIS_GENERIC_2";
            case MotionEvent.AXIS_GENERIC_3:
                return "AXIS_GENERIC_3";
            case MotionEvent.AXIS_GENERIC_4:
                return "AXIS_GENERIC_4";
            case MotionEvent.AXIS_GENERIC_5:
                return "AXIS_GENERIC_5";
            case MotionEvent.AXIS_GENERIC_6:
                return "AXIS_GENERIC_6";
            case MotionEvent.AXIS_GENERIC_7:
                return "AXIS_GENERIC_7";
            case MotionEvent.AXIS_GENERIC_8:
                return "AXIS_GENERIC_8";
            case MotionEvent.AXIS_GENERIC_9:
                return "AXIS_GENERIC_9";
            case MotionEvent.AXIS_GENERIC_10:
                return "AXIS_GENERIC_10";
            case MotionEvent.AXIS_GENERIC_11:
                return "AXIS_GENERIC_11";
            case MotionEvent.AXIS_GENERIC_12:
                return "AXIS_GENERIC_12";
            case MotionEvent.AXIS_GENERIC_13:
                return "AXIS_GENERIC_13";
            case MotionEvent.AXIS_GENERIC_14:
                return "AXIS_GENERIC_14";
            case MotionEvent.AXIS_GENERIC_15:
                return "AXIS_GENERIC_15";
            case MotionEvent.AXIS_GENERIC_16:
                return "AXIS_GENERIC_16";
            case MotionEvent.AXIS_HAT_X:
                return "AXIS_HAT_X";
            case MotionEvent.AXIS_HAT_Y:
                return "AXIS_HAT_Y";
            case MotionEvent.AXIS_HSCROLL:
                return "AXIS_HSCROLL";
            case MotionEvent.AXIS_LTRIGGER:
                return "AXIS_LTRIGGER";
            case MotionEvent.AXIS_ORIENTATION:
                return "AXIS_ORIENTATION";
            case MotionEvent.AXIS_PRESSURE:
                return "AXIS_PRESSURE";
            case MotionEvent.AXIS_RELATIVE_X:
                return "AXIS_RELATIVE_X";
            case MotionEvent.AXIS_RELATIVE_Y:
                return "AXIS_RELATIVE_Y";
            case MotionEvent.AXIS_RTRIGGER:
                return "AXIS_RTRIGGER";
            case MotionEvent.AXIS_RUDDER:
                return "AXIS_RUDDER";
            case MotionEvent.AXIS_RX:
                return "AXIS_RX";
            case MotionEvent.AXIS_RY:
                return "AXIS_RY";
            case MotionEvent.AXIS_RZ:
                return "AXIS_RZ";
            case MotionEvent.AXIS_SCROLL:
                return "AXIS_SCROLL";
            case MotionEvent.AXIS_SIZE:
                return "AXIS_SIZE";
            case MotionEvent.AXIS_THROTTLE:
                return "AXIS_THROTTLE";
            case MotionEvent.AXIS_TILT:
                return "AXIS_TILT";
            case MotionEvent.AXIS_TOOL_MAJOR:
                return "AXIS_TOOL_MAJOR";
            case MotionEvent.AXIS_TOOL_MINOR:
                return "AXIS_TOOL_MINOR";
            case MotionEvent.AXIS_TOUCH_MAJOR:
                return "AXIS_TOUCH_MAJOR";
            case MotionEvent.AXIS_TOUCH_MINOR:
                return "AXIS_TOUCH_MINOR";
            case MotionEvent.AXIS_VSCROLL:
                return "AXIS_VSCROLL";
            case MotionEvent.AXIS_WHEEL:
                return "AXIS_WHEEL";
            case MotionEvent.AXIS_X:
                return "AXIS_X";
            case MotionEvent.AXIS_Y:
                return "AXIS_Y";
            case MotionEvent.AXIS_Z:
                return "AXIS_Z";
            default:
                break;
        }
        return "AXIS_NONE";
    }

    private void logMotionRange(InputDevice.MotionRange motionRange) {
        String axisString = getAxisString(motionRange.getAxis());
        String axisSourceString = generateSourceString(motionRange.getSource());
        float axisFlat = motionRange.getFlat();
        float axisFuzz = motionRange.getFuzz();
        float axisMax = motionRange.getMax();
        float axisMin = motionRange.getMin();
        float axisRange = motionRange.getRange();
        float axisResolution = motionRange.getResolution();

        Log.d(TAG, "MotionRange:" +
                "\n" + axisString +
                "\n" + axisSourceString +
                "\n   Axis Min   : " + axisMin +
                "\n   Axis Max   : " + axisMax +
                "\n   Axis Range : " + axisRange +
                "\n   Axis Flat  : " + axisFlat +
                "\n   Axis Fuzz  : " + axisFuzz +
                "\n   Axis Res   : " + axisResolution);
    }

    private void logControllerInfo(int deviceId) {
        InputDevice inputDevice = InputDevice.getDevice(deviceId);
        int controllerNumber = inputDevice.getControllerNumber();
        String deviceDescriptor = inputDevice.getDescriptor();
        String deviceName = inputDevice.getName();
        int deviceProductId = inputDevice.getProductId();
        int deviceSources = inputDevice.getSources();
        int deviceVendorId = inputDevice.getVendorId();
        boolean hasVibrator = inputDevice.getVibrator().hasVibrator();
        boolean isVirtual = inputDevice.isVirtual();

        Log.d(TAG, "logControllerInfo" +
                "\nfor deviceId: " + deviceId +
                "\nname: " + deviceName +
                "\ndescriptor: " + deviceDescriptor +
                "\nvendorId: " + deviceVendorId +
                "\nproductId " + deviceProductId +
                "\nhasVibrator: " + hasVibrator +
                "\nisVirtual: " + isVirtual +
                "\n" + generateSourceString(deviceSources));

        List<InputDevice.MotionRange> motionRanges = inputDevice.getMotionRanges();
        Log.d(TAG, "Motion Range count: " + motionRanges.size());

        for (InputDevice.MotionRange motionRange : motionRanges) {
            logMotionRange(motionRange);
        }
    }

    // JNI interface functions for native GameControllerManager
    public native void onControllerConnected(int[] deviceInfoArray,
                                             float[] axisMinArray, float[] axisMaxArray,
                                             float[] axisFlatArray, float[] axisFloorArray);

    public native void onControllerDisconnected(int deviceId);

    public native void onMouseConnected(int deviceId);

    public native void onMouseDisconnected(int deviceId);
}