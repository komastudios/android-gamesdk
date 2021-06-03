/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This is the main interface to the Android Game Controller library, also known as Paddleboat.

   See the documentation at
   https://developer.android.com/games/sdk/game-controller for more information on using this
   library in a native Android game.
 */

/**
 * @defgroup paddleboat Game Controller main interface
 * The main interface to use the Game Controller library.
 * @{
 */

#ifndef PADDLEBOAT_H
#define PADDLEBOAT_H

#include <stdint.h>
#include <jni.h>
#include <android/input.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @cond INTERNAL */

// Internal macros to track Paddleboat version, do not use directly.
#define PADDLEBOAT_MAJOR_VERSION 1
#define PADDLEBOAT_MINOR_VERSION 0
#define PADDLEBOAT_PACKED_VERSION ((PADDLEBOAT_MAJOR_VERSION<<16)|(PADDLEBOAT_MINOR_VERSION))

// Internal macros to generate a symbol to track PADDLEBOAT version, do not use directly.
#define PADDLEBOAT_VERSION_CONCAT_NX(PREFIX, MAJOR, MINOR) PREFIX ## _ ## MAJOR ## _ ## MINOR
#define PADDLEBOAT_VERSION_CONCAT(PREFIX, MAJOR, MINOR) PADDLEBOAT_VERSION_CONCAT_NX(PREFIX, MAJOR, MINOR)
#define PADDLEBOAT_VERSION_SYMBOL PADDLEBOAT_VERSION_CONCAT(PADDLEBOAT_version, PADDLEBOAT_MAJOR_VERSION, PADDLEBOAT_MINOR_VERSION)

/** @endcond */

//* @brief Maximum number of simultaneously connected controllers. */
#define PADDLEBOAT_MAX_CONTROLLERS 8
//* @brief Maximum number of mouse devices reported by the library. */
#define PADDLEBOAT_MAX_MOUSE 1

//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` direction pad up button */
#define PADDLEBOAT_BUTTON_DPAD_UP (1U << 0)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` direction pad left button */
#define PADDLEBOAT_BUTTON_DPAD_LEFT (1U << 1)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` direction pad down button */
#define PADDLEBOAT_BUTTON_DPAD_DOWN (1U << 2)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` direction pad right button */
#define PADDLEBOAT_BUTTON_DPAD_RIGHT (1U << 3)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` A button */
#define PADDLEBOAT_BUTTON_A (1U << 4)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` B button */
#define PADDLEBOAT_BUTTON_B (1U << 5)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` X button */
#define PADDLEBOAT_BUTTON_X (1U << 6)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` Y button */
#define PADDLEBOAT_BUTTON_Y (1U << 7)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` L1 trigger button */
#define PADDLEBOAT_BUTTON_L1 (1U << 8)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` L2 trigger button */
#define PADDLEBOAT_BUTTON_L2 (1U << 9)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` L3 thumbstick button */
#define PADDLEBOAT_BUTTON_L3 (1U << 10)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` R1 trigger button */
#define PADDLEBOAT_BUTTON_R1 (1U << 11)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` R2 trigger button */
#define PADDLEBOAT_BUTTON_R2 (1U << 12)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` R3 thumbstick button */
#define PADDLEBOAT_BUTTON_R3 (1U << 13)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` Select button */
#define PADDLEBOAT_BUTTON_SELECT (1U << 14)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` Start button */
#define PADDLEBOAT_BUTTON_START (1U << 15)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` System button */
#define PADDLEBOAT_BUTTON_SYSTEM (1U << 16)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` Touchpad pressed */
#define PADDLEBOAT_BUTTON_TOUCHPAD (1U << 17)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` AUX1 button */
#define PADDLEBOAT_BUTTON_AUX1 (1U << 18)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` AUX2 button */
#define PADDLEBOAT_BUTTON_AUX2 (1U << 19)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` AUX3 button */
#define PADDLEBOAT_BUTTON_AUX3 (1U << 20)
//* @brief Bitmask for `Paddleboat_Controller_Data.buttonsDown` AUX4 button */
#define PADDLEBOAT_BUTTON_AUX4 (1U << 21)
//* @brief Count of defined controller buttons. */
#define PADDLEBOAT_BUTTON_COUNT 22

/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device wasn't found in the internal
 * controller database and a generic button and axis mapping
 * profile is being used
*/
#define PADDLEBOAT_CONTROLLER_FLAG_GENERIC_PROFILE      (0000000010)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device reports battery status information
 * in `Paddleboat_Controller_Data.battery`
*/
#define PADDLEBOAT_CONTROLLER_FLAG_BATTERY              (0x04000000)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device reports accelerometer data
 * in `Paddleboat_Controller_Data.accelerometer`
*/
#define PADDLEBOAT_CONTROLLER_FLAG_ACCELEROMETER        (0x08000000)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device reports gyroscope data
 * in `Paddleboat_Controller_Data.gyroscope`
*/
#define PADDLEBOAT_CONTROLLER_FLAG_GYROSCOPE            (0x01000000)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device supports calling
 * ::Paddleboat_setControllerLight with a `PADDLEBOAT_LIGHT_PLAYER_INDEX`
 * light type
*/
#define PADDLEBOAT_CONTROLLER_FLAG_LIGHT_PLAYER         (0x02000000)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device supports calling
 * ::Paddleboat_setControllerLight with a `PADDLEBOAT_LIGHT_RGB`
 * light type
*/
#define PADDLEBOAT_CONTROLLER_FLAG_LIGHT_RGB            (0x04000000)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device supports vibration effects
 * using the ::Paddleboat_setControllerVibrationData function
*/
#define PADDLEBOAT_CONTROLLER_FLAG_VIBRATION            (0x08000000)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device supports both left and right
 * vibration motor data for vibration effects
*/
#define PADDLEBOAT_CONTROLLER_FLAG_VIBRATION_DUAL_MOTOR (0x10000000)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device has a touchpad that will register
 * the `PADDLEBOAT_BUTTON_TOUCHPAD` button when pressed
*/
#define PADDLEBOAT_CONTROLLER_FLAG_TOUCHPAD             (0x20000000)
/**
 * @brief Bitmask for `Paddleboat_Controller_Info.controllerFlags
 * If set, this controller device can simulate a virtual mouse and
 * will report coordinates in the `Paddleboat_Controller_Data.virtualX`
 * and `Paddleboat_Controller_Data.virtualY` fields
*/
#define PADDLEBOAT_CONTROLLER_FLAG_VIRTUAL_MOUSE        (0x40000000)

//* @brief Bitmask for `Paddleboat_Mouse_Data.buttonsDown` left mouse button */
#define PADDLEBOAT_MOUSE_BUTTON_LEFT (1U << 0)
//* @brief Bitmask for `Paddleboat_Mouse_Data.buttonsDown` right mouse button */
#define PADDLEBOAT_MOUSE_BUTTON_RIGHT (1U << 1)
//* @brief Bitmask for `Paddleboat_Mouse_Data.buttonsDown` middle mouse button */
#define PADDLEBOAT_MOUSE_BUTTON_MIDDLE (1U << 2)
//* @brief Bitmask for `Paddleboat_Mouse_Data.buttonsDown` mouse button 4 */
#define PADDLEBOAT_MOUSE_BUTTON_4 (1U << 3)
//* @brief Bitmask for `Paddleboat_Mouse_Data.buttonsDown` mouse button 5 */
#define PADDLEBOAT_MOUSE_BUTTON_5 (1U << 4)
//* @brief Bitmask for `Paddleboat_Mouse_Data.buttonsDown` mouse button 6 */
#define PADDLEBOAT_MOUSE_BUTTON_6 (1U << 5)
//* @brief Bitmask for `Paddleboat_Mouse_Data.buttonsDown` mouse button 7 */
#define PADDLEBOAT_MOUSE_BUTTON_7 (1U << 6)
//* @brief Bitmask for `Paddleboat_Mouse_Data.buttonsDown` mouse button 8 */
#define PADDLEBOAT_MOUSE_BUTTON_8 (1U << 7)

//* @brief Number of axis used for controller mapping configuration. */
#define PADDLEBOAT_MAPPING_AXIS_COUNT 10
/**
 * @brief Constant that signifies an axis in the
 * `Paddleboat_Controller_Mapping_Data.axisMapping` array is unused by
 * the controller
*/
#define PADDLEBOAT_AXIS_IGNORED 0xFFFE
/**
 * @brief Constant that signifies an axis in the
 * `Paddleboat_Controller_Mapping_Data.axisPositiveButtonMapping` array
 * and/or `Paddleboat_Controller_Mapping_Data.axisNegativeButtonMapping` array
 * does not have a mapping to a button
*/
#define PADDLEBOAT_AXIS_BUTTON_IGNORED 0xFE
/**
 * @brief Constant that signifies a button in the
 * `Paddleboat_Controller_Mapping_Data.buttonMapping` array
 * does not map to a button on the controller
*/
#define PADDLEBOAT_BUTTON_IGNORED 0xFFFE

/**
 * @brief Battery status of a controller's internal battery
*/
enum Paddleboat_BatteryStatus {
    PADDLEBOAT_CONTROLLER_BATTERY_UNKNOWN = 0, ///< Battery status is unknown
    PADDLEBOAT_CONTROLLER_BATTERY_CHARGING, ///< Controller battery is charging
    PADDLEBOAT_CONTROLLER_BATTERY_DISCHARGING, ///< Controller battery is discharging
    PADDLEBOAT_CONTROLLER_BATTERY_NOT_CHARGING, ///< Controller battery is not charging
    PADDLEBOAT_CONTROLLER_BATTERY_FULL ///< Controller battery is completely charged
};

/**
 * @brief Current status of a controller (at a specified controller index)
*/
enum Paddleboat_ControllerStatus {
    PADDLEBOAT_CONTROLLER_INACTIVE = 0, ///< No controller is connected
    PADDLEBOAT_CONTROLLER_ACTIVE = 1, ///< Controller is connected and active
    PADDLEBOAT_CONTROLLER_JUST_CONNECTED = 2, ///< Controller has just connected,
                                              ///< only seen in a controller status callback
    PADDLEBOAT_CONTROLLER_JUST_DISCONNECTED = 3 ///< Controller has just disconnected,
                                                ///< only seen in a controller status callback
};

/**
 * @brief The button layout and iconography of the controller buttons
*/
enum Paddleboat_ControllerButtonLayout {
    PADDLEBOAT_CONTROLLER_LAYOUT_STANDARD = 0, ///<  Y
                                               ///< X B
                                               ///<  A
    PADDLEBOAT_CONTROLLER_LAYOUT_SHAPES = 1, ///<  △
                                             ///< □ ○
                                             ///<  x
                                             ///< x = A, ○ = B, □ = X, △ = Y
    PADDLEBOAT_CONTROLLER_LAYOUT_REVERSE = 2, ///<  X
                                              ///< Y A
                                              ///<  B
    PADDLEBOAT_CONTROLLER_LAYOUT_ARCADE_STICK = 3, ///< X Y R1 L1
                                                   ///< A B R2 L2
    PADDLEBOAT_CONTROLLER_LAYOUT_MASK = 3 ///< Mask value, AND with
                                          ///< `Paddleboat_Controller_Info.controllerFlags`
                                          ///< to get the `Paddleboat_ControllerButtonLayout` value
};

/**
 * @brief The type of GameActivity event structure being
 * passed to ::Paddleboat_processGameActivityInputEvent
*/
enum Paddleboat_GameActivityEvent {
    PADDLEBOAT_GAMEACTIVITY_EVENT_KEY = 0, ///< Event is a `GameActivityKeyEvent`
    PADDLEBOAT_GAMEACTIVITY_EVENT_MOTION ///< Event is a `GameActivityMotionEvent`
};

/**
 * @brief The type of light being specified by a call to
 * ::Paddleboat_setControllerLight
*/
enum Paddleboat_LightType {
    PADDLEBOAT_LIGHT_PLAYER_INDEX = 0, ///< Light is a player index,
                                       ///< `lightData` is the player number
    PADDLEBOAT_LIGHT_RGB ///< Light is a color light, `lightData` is a ARGB (8888) light value
};

/**
 * @brief The status of the mouse device
*/
enum Paddleboat_MouseStatus {
    PADDLEBOAT_MOUSE_NONE = 0, ///< No mouse device is connected
    PADDLEBOAT_MOUSE_CONTROLLER_EMULATED = 1, ///< A virtual mouse is connected
                                              ///< The virtual mouse is being simulated
                                              ///< by a game controller
    PADDLEBOAT_MOUSE_PHYSICAL ///< A physical mouse or trackpad device is connected
};

/**
 * @brief The addition mode to use when passing new controller mapping data
 * to ::Paddleboat_addControllerRemapData
*/
enum Paddleboat_Remap_Addition_Mode {
    PADDLEBOAT_REMAP_ADD_MODE_DEFAULT = 0, ///< If a vendorId/productId controller entry in the
                                           ///< new remap table exists in the current database,
                                           ///< and the min/max API ranges overlap, replace the
                                           ///< existing entry, otherwise add a new entry.
                                           ///< Always adds a new entry if the vendorId/productId
                                           ///< does not exist in the current database. These
                                           ///< changes only persist for the current session.
    PADDLEBOAT_REMAP_ADD_MODE_REPLACE_ALL  ///< The current controller database will be erased and
                                           ///< entirely replaced by the new remap table.
                                           ///< This change only persists for the current session.
};

/**
 * @brief A structure that describes the current battery state of a controller. This structure
 * will only be populated if a controller has `PADDLEBOAT_CONTROLLER_FLAG_BATTERY` set in
 * `Paddleboat_Controller_Info.controllerFlags`
 */
typedef struct Paddleboat_Controller_Battery {
    Paddleboat_BatteryStatus batteryStatus; /** @brief The current status of the battery */
    float batteryLevel; /** @brief The current charge level of the battery, from 0.0 to 1.0 */
} Paddleboat_Controller_Battery;

/**
 * @brief A structure that contains X/Y/Z axis data for a motion sensor.
 * For an accelerometer sensor, units are radians/second.
 * For a gyroscope sensor, SI units (m/s^2).
 */
typedef struct Paddleboat_Controller_Motion {
    float motionX; /** @brief X axis data for the motion sensor */
    float motionY; /** @brief Y axis data for the motion sensor */
    float motionZ; /** @brief Z axis data for the motion sensor */
} Paddleboat_Controller_Motion;

/**
 * @brief A structure that contains X and Y axis data for an analog thumbstick.
 * Axis ranges from -1.0 to 1.0.
 */
typedef struct Paddleboat_Controller_Thumbstick {
    float stickX; /** @brief X axis data for the thumbstick */
    float stickY; /** @brief X axis data for the thumbstick */
} Paddleboat_Controller_Thumbstick;

/**
 * @brief A structure that contains the current data
 * for a controller's inputs and sensors.
 */
typedef struct Paddleboat_Controller_Data {
    uint32_t updateSerial; /** @brief Serial value that increments when new data is read */
    uint32_t buttonsDown; /** @brief Bit-per-button bitfield array */
    Paddleboat_Controller_Thumbstick leftStick; /** @brief Left analog thumbstick axis data */
    Paddleboat_Controller_Thumbstick rightStick; /** @brief Right analog thumbstick axis data */
    float triggerL1; /** @brief L1 trigger axis data */
    float triggerL2; /** @brief L2 trigger axis data */
    float triggerR1; /** @brief R1 trigger axis data */
    float triggerR2; /** @brief R2 trigger axis data */
    float virtualX; /** @brief Virtual pointer X coordinates in screen pixels */
    float virtualY; /** @brief Virtual pointer Y coordinates in screen pixels */
    Paddleboat_Controller_Motion accelerometer; /** @brief Accelerometer motion sensor data */
    Paddleboat_Controller_Motion gyroscope; /** @brief Gyroscope motion sensor data */
    Paddleboat_Controller_Battery battery; /** @brief Battery status */
} Paddleboat_Controller_Data;

/**
 * @brief A structure that contains information
 * about a particular controller device. Several fields
 * are populated by the value of the corresponding fields from InputDevice.
 */
typedef struct Paddleboat_Controller_Info {
    const char *controllerName; /** @brief Name string, maps to InputDevice.getName() */
    uint32_t controllerFlags; /** @brief Controller feature flag bits */
    int32_t controllerNumber; /** @brief Controller number, maps
                                * to InputDevice.getControllerNumber()
                                */
    int32_t vendorId; /** @brief Vendor ID, maps to InputDevice.getVendorId() */
    int32_t productId; /** @brief Product ID, maps to InputDevice.getProductId() */
    int32_t deviceId; /** @brief , Device ID, maps to InputDevice.getId() */
    float stickFlat; /** @brief, Extent of a center flat (deadzone) area of the thumbsticks */
    float stickFuzz; /** @brief, Error tolerance (deviation) values of the thumbsticks */
} Paddleboat_Controller_Info;

/**
 * @brief A structure that contains input data for the mouse device.
 */
typedef struct Paddleboat_Mouse_Data {
    uint32_t buttonsDown; /** @brief Bit-per-button bitfield array of mouse button status. */
    int32_t mouseScrollDeltaH; /** @brief Number of horizontal mouse wheel movements since previous
                                 * read of mouse data. Can be positive or negative depending
                                 * on direction of scrolling.
                                 */
    int32_t mouseScrollDeltaV; /** @brief Number of vertical mouse wheel movements since previous
                                 * read of mouse data. Can be positive or negative depending
                                 * on direction of scrolling.
                                 */
    float mouseX; /** @brief Current mouse X coordinates in screen pixels. */
    float mouseY; /** @brief Current mouse Y coordinates in screen pixels. */
} Paddleboat_Mouse_Data;

/**
 * @brief A structure that describes the parameters of a vibration effect.
 */
typedef struct Paddleboat_Vibration_Data {
    int32_t durationLeft; /** @brief Duration to vibrate the left motor in milliseconds. */
    int32_t durationRight; /** @brief Duration to vibrate the right motor in milliseconds. */
    float intensityLeft; /** @brief Intensity of vibration of left motor,
                           * valid range is 0.0 to 1.0.
                           */
    float intensityRight; /** @brief Intensity of vibration of right motor,
                            * valid range is 0.0 to 1.0.
                            */
} Paddleboat_Vibration_Data;

/**
 * @brief A structure that describes the button and axis mappings
 * for a specified controller device running on a specified range of Android API levels
 * Axis order: LStickX, LStickY, RStickX, RStickY, L1, L2, R1, R2, HatX, HatY
 */
typedef struct Paddleboat_Controller_Mapping_Data {
    int16_t minimumEffectiveApiLevel; /** @brief Minimum API level required for this entry */
    int16_t maximumEffectiveApiLevel; /** @brief Maximum API level required for this entry,
                                        * 0 = no max
                                        */
    int32_t vendorId; /** @brief VendorID of the controller device for this entry */
    int32_t productId; /** @brief ProductID of the controller device for this entry */
    int32_t flags; /** @brief Flag bits, will be ORed with
                     * `Paddleboat_Controller_Info.controllerFlags`
                     */
    //
    // Hat axis will map to dpad buttons
    // Set PADDLEBOAT_AXIS_IGNORED for unmapped/unused axis, otherwise should set
    // to the AMOTION_EVENT_AXIS corresponding with the desired paddleboat control axis.
    uint16_t axisMapping[PADDLEBOAT_MAPPING_AXIS_COUNT]; /** @brief AMOTION_EVENT_AXIS value for
            * the corresponding Paddleboat control axis, or PADDLEBOAT_AXIS_IGNORED if unsupported.
            */
    uint8_t axisPositiveButtonMapping[PADDLEBOAT_MAPPING_AXIS_COUNT]; /** @brief Button to set on
            * positive axis value, PADDLEBOAT_AXIS_BUTTON_IGNORED if none.
            */
    uint8_t axisNegativeButtonMapping[PADDLEBOAT_MAPPING_AXIS_COUNT]; /** @brief Button to set on
            * negative axis value, PADDLEBOAT_AXIS_BUTTON_IGNORED if none.
            */
    uint16_t buttonMapping[PADDLEBOAT_BUTTON_COUNT]; /** @brief AKEYCODE_ value corresponding
                                                       * with the corresponding Paddleboat button.
                                                       * PADDLEBOAT_BUTTON_IGNORED if unsupported.
                                                       */
} Paddleboat_Controller_Mapping_Data;

/**
 * @brief Signature of a function that can be passed to
 * ::Paddleboat_setControllerStatusCallback to receive information about controller
 * connections and disconnections.

 * @param controllerIndex Index of the controller that has registered a status change,
 * will range from 0 to PADDLEBOAT_MAX_CONTROLLERS - 1.
 * @param controllerStatus New status of the controller.
 *
 * Function will be called on the same thread that calls ::Paddleboat_update.
 */
typedef void (*Paddleboat_ControllerStatusCallback)(const int32_t controllerIndex,
    const Paddleboat_ControllerStatus controllerStatus);

/**
 * @brief Signature of a function that can be passed to
 * ::Paddleboat_setMouseStatusCallback to receive information about mouse
 * device status changes.
 * @param mouseStatus Current status of the mouse.
 *
 * Function will be called on the same thread that calls ::Paddleboat_update.
 */
typedef void (*Paddleboat_MouseStatusCallback)(const Paddleboat_MouseStatus mouseStatus);

/** @cond INTERNAL */

// Internal init function. Do not call directly.
bool Paddleboat_init_internal(JNIEnv *env, jobject jactivity, const char *libraryName);

// Internal function to track Paddleboat version bundled in a binary. Do not call directly.
// If you are getting linker errors related to Paddleboat_version_x_y, you probably have a
// mismatch between the header used at compilation and the actually library used by the linker.
void PADDLEBOAT_VERSION_SYMBOL();

/** @endcond */

/**
 * @brief Initialize Paddleboat, constructing internal resources via JNI.
 * @param env The JNI environment where Paddleboat is to be used.
 * @param jactivity The activity used by the game.
 * @param libraryName If Paddleboat is linked as a static library, this must be the name
 * of the shared library which includes the Paddleboat shared library. Omit the 'lib'
 * prefix and '.so' suffix. i.e. for libgame.so, pass "game".
 * If including the libpaddleboat.so shared library, libraryName should be NULL or nullptr.
 * @return false if Paddleboat failed to initialize.
 * @see Paddleboat_destroy
 */
static inline bool Paddleboat_init(JNIEnv *env, jobject jactivity, const char *libraryName) {
    // This call ensures that the header and the linked library are from the same version
    // (if not, a linker error will be triggered because of an undefined symbol).
    PADDLEBOAT_VERSION_SYMBOL();
    return Paddleboat_init_internal(env, jactivity, libraryName);
}

/**
 * @brief Check if Paddleboat was successfully initialized.
 * @return false if the initialization failed or was not called.
 */
bool Paddleboat_isEnabled();

/**
 * @brief Destroy resources that Paddleboat has created.
 * @see Paddleboat_init
 */
void Paddleboat_destroy();

/**
 * @brief Inform Paddleboat that a pause event was sent to the application.
 */
void Paddleboat_onPause();

/**
 * @brief Inform Paddleboat that a resume event was sent to the application.
 */
void Paddleboat_onResume();

/**
 * @brief Process an input event to see if it is from a device being
 * managed by Paddleboat.
 * @param event the input event received by the application.
 * @return 0 if the event was ignored, 1 if the event was processed/consumed by Paddleboat.
 */
int32_t Paddleboat_processInputEvent(const AInputEvent *event);

/**
 * @brief Process a Game Activity input event to see if it is from a
 * device being managed by Paddleboat.
 * @param eventType the type of Game Activity event being passed in the
 * event param. PADDLEBOAT_GAMEACTIVITY_EVENT_KEY means a
 * GameActivityKeyEvent is being passed.
 * PADDLEBOAT_GAMEACTIVITY_EVENT_MOTION means a GameActivityMotionEvent
 * is being passed.
 * @param event the Game Activity input event received by the application.
 * @param eventSize the size of the Game Activity event structing being
 * passed in bytes.
 * @return 0 if the event was ignored, 1 if the event was processed/consumed by Paddleboat.
 */
int32_t Paddleboat_processGameActivityInputEvent(const Paddleboat_GameActivityEvent eventType,
                                                 const void *event, const size_t eventSize);

/**
 * @brief Retrieve the active axis ids being used by connected devices. This can be used
 * to determine what axis values to provide to GameActivityPointerInfo_enableAxis
 * when GameActivity is being used.
 * @return A bitmask of the active axis ids that have been used by connected devices
 * during the current application session.
 */
uint64_t Paddleboat_getActiveAxisMask();

/**
 * @brief Set whether Paddleboat consumes AKEYCODE_BACK key events from devices being
 * managed by Paddleboat. The default at initialization is true. This can be set to false
 * to allow exiting the application from a back button press when the application is in
 * an appropriate state (i.e. the title screen).
 * @param consumeBackButton If true, Paddleboat will consume AKEYCODE_BACK key events, if false
 * it will pass them through.
 */
void Paddleboat_setBackButtonConsumed(bool consumeBackButton);

/**
 * @brief Set a callback to be called whenever a controller managed by Paddleboat changes
 * status. This is used to inform of controller connections and disconnections.
 * @param statusCallback function pointer to the controllers status change callback,
 * passing NULL or nullptr will remove any currently registered callback.
 */
void Paddleboat_setControllerStatusCallback(Paddleboat_ControllerStatusCallback statusCallback);

/**
 * @brief Configure a light on the controller with the specified index.
 * @param controllerIndex The index of the controller to read from, must be between
 * 0 and PADDLEBOAT_MAX_CONTROLLERS - 1.
 * @param lightType Specifies the type of light on the controller to configure.
 * @param lightData Light configuration data. For player index lights, this is a number
 * indicating the player index (usually between 1 and 4). For RGB lights, this is a
 * 8888 ARGB value.
 * @return true if the light was set, false if there was no connected controller or the
 * connected controller does not support the specified light type.
 */
 bool Paddleboat_setControllerLight(const int32_t controllerIndex,
                                    const Paddleboat_LightType lightType,
                                    const uint32_t lightData);

/**
 * @brief Set a callback to be called when the mouse status changes. This is used
 * to inform of physical or virual mouse device connections and disconnections.
 * @param statusCallback function pointer to the controllers status change callback,
 * passing NULL or nullptr will remove any currently registered callback.
 */
void Paddleboat_setMouseStatusCallback(Paddleboat_MouseStatusCallback statusCallback);

/**
 * @brief Retrieve the current controller data from the controller with the specified index.
 * @param controllerIndex The index of the controller to read from, must be between
 * 0 and PADDLEBOAT_MAX_CONTROLLERS - 1
 * @param[out] controllerData a pointer to the controller data struct to populate.
 * @return true if the data was read, false if there was no connected controller
 * at the specified index.
 */
bool Paddleboat_getControllerData(const int32_t controllerIndex,
                                  Paddleboat_Controller_Data *controllerData);

/**
 * @brief Retrieve the current controller device info from the controller with the specified index.
 * @param controllerIndex The index of the controller to read from, must be between
 * 0 and PADDLEBOAT_MAX_CONTROLLERS - 1
 * @param[out] controllerInfo a pointer to the controller device info struct to populate.
 * @return true if the data was read, false if there was no connected controller
 * at the specified index.
 */
bool Paddleboat_getControllerInfo(const int32_t controllerIndex,
                                  Paddleboat_Controller_Info *controllerInfo);

/**
 * @brief Retrieve the current controller device info from the controller with the specified index.
 * @param controllerIndex The index of the controller to read from, must be between
 * 0 and PADDLEBOAT_MAX_CONTROLLERS - 1.
 * @return Paddleboat_ControllerStatus enum value of the current controller status of
 * the specified controller index.
 */
Paddleboat_ControllerStatus Paddleboat_getControllerStatus(const int32_t controllerIndex);

/**
 * @brief Set vibration data for the controller with the specified index.
 * @param controllerIndex The index of the controller to read from, must be between
 * 0 and PADDLEBOAT_MAX_CONTROLLERS - 1.
 * @param vibrationData The intensity and duration data for the vibration effect. Valid intensity
 * range is from 0.0 to 1.0. Intensity of 0.0 will turn off vibration if it is active. Duration
 * is specified in milliseconds.
 * @return true if the vibration data was set, false if there was no connected controller or the
 * connected controller does not support vibration.
 */
bool Paddleboat_setControllerVibrationData(const int32_t controllerIndex,
                                           const Paddleboat_Vibration_Data *vibrationData);
/**
 * @brief Retrieve the current mouse data.
 * @param[out] mouseData pointer to the mouse data struct to populate.
 * @return true if the data was read, false if there was no connected mouse device.
 */
bool Paddleboat_getMouseData(Paddleboat_Mouse_Data *mouseData);

/**
 * @brief Retrieve the current controller device info from the controller with the specified index.
 * @return Paddleboat_MouseStatus enum value of the current mouse status.
 */
Paddleboat_MouseStatus Paddleboat_getMouseStatus();

/**
 * @brief Add new controller remap information to the internal remapping table.
 * @param addMode The addition mode for the new data. See the `Paddleboat_Remap_Addition_Mode` enum
 * for details on each mode.
 * @param remapTableEntryCount the number of remap elements in the mappingData array.
 * @param mappingData An array of controller mapping structs to be added to the internal
 * remapping table.
 * @param statusCallback function pointer to the controller status change callback.
 */
void Paddleboat_addControllerRemapData(const Paddleboat_Remap_Addition_Mode addMode,
                                       const int32_t remapTableEntryCount,
                                       const Paddleboat_Controller_Mapping_Data* mappingData);

/**
 * @brief Retrieve the current table of controller remap entries.
 * @param destRemapTableEntryCount the number of `Paddleboat_Controller_Mapping_Data`
 * entries in the array passed in the mappingData parameter. Paddleboat will not copy
 * more than this number of entries out of the internal array to avoid overflowing the buffer.
 * @param[out] mappingData pointer to an array of `Paddleboat_Controller_Mapping_Data` structures.
 * this should contain at least destRemapTableEntryCount elements. Passing nullptr is valid,
 * and can be used to get the number of elements in the internal remap table.
 * @return The number of elements in the internal remap table.
 */
int32_t Paddleboat_getControllerRemapTableData(const int32_t destRemapTableEntryCount,
                                               Paddleboat_Controller_Mapping_Data* mappingData);

/**
 * @brief Updates internal Paddleboat status and processes pending connection/disconnections.
 * This function should be called once per game frame from the thread that called
 * ::Paddleboat_init.
 */
void Paddleboat_update();

/** @cond INTERNAL */
/**
 * @brief An internal debugging function that returns the last keycode seen in a key event
 * coming from a controller owned by Paddleboat. Used for visual troubleshooting of unknown
 * buttons in new devices in the sample app
 * @return keycode from last controller key event.
 */
int32_t Paddleboat_getLastKeycode();
/** @endcond */

#ifdef __cplusplus
}
#endif

#endif
/** @} */
