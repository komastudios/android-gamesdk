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

#include "paddleboat.h"
#include "GameControllerManager.h"

using namespace paddleboat;

extern "C" {

// Internal init function. Do not call directly.
bool Paddleboat_init_internal(JNIEnv *env, jobject jactivity, const char *libraryName) {
    bool success = GameControllerManager::init(env, jactivity, libraryName);
    if (success) {
        GameControllerManager::update();
    }
    return success;
}

bool Paddleboat_isEnabled() {
    return GameControllerManager::isEnabled();
}

void Paddleboat_destroy() {
    GameControllerManager::destroyInstance();
}

void Paddleboat_onPause() {
    GameControllerManager::onPause();
}

void Paddleboat_onResume() {
    GameControllerManager::onResume();
}

int32_t Paddleboat_processInputEvent(const AInputEvent *event) {
    return GameControllerManager::processInputEvent(event);
}

int32_t Paddleboat_processGameActivityInputEvent(const Paddleboat_GameActivityEvent eventType,
                                                 const void *event,
                                                 const size_t eventSize) {
    return GameControllerManager::processGameActivityInputEvent(eventType, event, eventSize);
}

uint64_t Paddleboat_getActiveAxisMask() {
    return GameControllerManager::getActiveAxisMask();
}

void Paddleboat_setBackButtonConsumed(bool consumeBackButton) {
    GameControllerManager::setBackButtonConsumed(consumeBackButton);
}

void Paddleboat_setControllerStatusCallback(Paddleboat_ControllerStatusCallback statusCallback) {
    GameControllerManager::setControllerStatusCallback(statusCallback);
}

void Paddleboat_setMouseStatusCallback(Paddleboat_MouseStatusCallback statusCallback) {
    GameControllerManager::setMouseStatusCallback(statusCallback);
}


bool Paddleboat_getControllerData(const int32_t controllerIndex,
                                  Paddleboat_Controller_Data *controllerData) {
    return GameControllerManager::getControllerData(controllerIndex, controllerData);
}

bool Paddleboat_getControllerInfo(const int32_t controllerIndex,
                                  Paddleboat_Controller_Info *controllerInfo) {
    return GameControllerManager::getControllerInfo(controllerIndex, controllerInfo);
}

Paddleboat_ControllerStatus Paddleboat_getControllerStatus(const int32_t controllerIndex) {
    return GameControllerManager::getControllerStatus(controllerIndex);
}

bool Paddleboat_setControllerVibrationData(const int32_t controllerIndex,
                                           const Paddleboat_Vibration_Data *vibrationData) {
    return GameControllerManager::setControllerVibrationData(controllerIndex, vibrationData);
}

bool
Paddleboat_setControllerLight(const int32_t controllerIndex, const Paddleboat_LightType lightType,
                              const uint32_t lightData) {
    return GameControllerManager::setControllerLight(controllerIndex, lightType, lightData);
}

bool Paddleboat_getMouseData(Paddleboat_Mouse_Data *mouseData) {
    return GameControllerManager::getMouseData(mouseData);
}

Paddleboat_MouseStatus Paddleboat_getMouseStatus() {
    return GameControllerManager::getMouseStatus();
}

void Paddleboat_addControllerRemapData(const Paddleboat_Remap_Addition_Mode addMode,
                                       const int32_t remapTableEntryCount,
                                       const Paddleboat_Controller_Mapping_Data *mappingData) {
    GameControllerManager::addControllerRemapData(addMode, remapTableEntryCount, mappingData);
}

int32_t Paddleboat_getControllerRemapTableData(const int32_t destRemapTableEntryCount,
                                               Paddleboat_Controller_Mapping_Data *mappingData) {
    return GameControllerManager::getControllerRemapTableData(destRemapTableEntryCount,
                                                              mappingData);
}

void Paddleboat_update() {
    GameControllerManager::update();
}

int32_t Paddleboat_getLastKeycode() {
    return GameControllerManager::getLastKeycode();
}

void PADDLEBOAT_VERSION_SYMBOL() {
    // Intentionally empty: this function is used to ensure that the proper
    // version of the library is linked against the proper headers.
    // In case of mismatch, a linker error will be triggered because of an
    // undefined symbol, as the name of the function depends on the version.
}
}