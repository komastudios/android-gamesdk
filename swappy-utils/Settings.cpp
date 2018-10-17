/*
 * Copyright 2018 The Android Open Source Project
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

#include "Settings.h"

#define LOG_TAG "Settings"

#include <memory>

#include "Log.h"

Settings *Settings::getInstance() {
    static auto settings = std::make_unique<Settings>(ConstructorTag{});
    return settings.get();
}

void Settings::addListener(Listener listener) {
    std::lock_guard lock(mMutex);
    mListeners.emplace_back(std::move(listener));
}

void Settings::setRefreshPeriod(std::chrono::nanoseconds value) {
    {
        std::lock_guard lock(mMutex);
        mRefreshPeriod = value;
    }
    notifyListeners();
}

void Settings::setSwapInterval(int32_t value) {
    {
        std::lock_guard lock(mMutex);
        mSwapInterval = value;
    }
    notifyListeners();
}

void Settings::setUseAffinity(bool value) {
    {
        std::lock_guard lock(mMutex);
        mUseAffinity = value;
    }
    notifyListeners();
}

std::chrono::nanoseconds Settings::getRefreshPeriod() const {
    std::lock_guard lock(mMutex);
    return mRefreshPeriod;
}

int32_t Settings::getSwapInterval() const {
    std::lock_guard lock(mMutex);
    return mSwapInterval;
}

bool Settings::getUseAffinity() const {
    std::lock_guard lock(mMutex);
    return mUseAffinity;
}

void Settings::notifyListeners() {
    // Grab a local copy of the listeners
    std::vector<Listener> listeners;
    {
        std::lock_guard lock(mMutex);
        listeners = mListeners;
    }

    // Call the listeners without the lock held
    for (const auto &listener : listeners) {
        listener();
    }
}