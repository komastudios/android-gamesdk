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

package com.prefabulated.bouncyball;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import androidx.preference.Preference;
import androidx.preference.ListPreference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceManager;
import androidx.preference.PreferenceScreen;

public class SettingsFragment
        extends PreferenceFragmentCompat
        implements SharedPreferences.OnSharedPreferenceChangeListener {
    private ListPreference mSwapIntervalPreference;
    private String mSwapIntervalKey;

    @Override
    public void onCreatePreferences(Bundle bundle, String s) {
        mSwapIntervalKey = getResources().getString(R.string.swap_interval_key);
        addPreferencesFromResource(R.xml.preferences);

        PreferenceScreen preferenceScreen = getPreferenceScreen();
        for (int i = 0; i < preferenceScreen.getPreferenceCount(); ++i) {
            Preference preference = preferenceScreen.getPreference(i);
            final String key = preference.getKey();
            if (key != null && key.equals(mSwapIntervalKey)) {
                mSwapIntervalPreference = (ListPreference)preference;
            }
        }

        // fill the swap interval list based on the screen refresh rate
        List<String> entries = new ArrayList<>();
        List<String> entryValues = new ArrayList<>();

        float refreshRate = getActivity().getWindowManager().getDefaultDisplay().getRefreshRate();
        for(int dev = 1; refreshRate / dev >= 20; dev++) {
            int fps = (int)(refreshRate / dev);
            float ms =  1000 / (refreshRate / dev);

            entries.add(String.format(Locale.US, "%.2fms (%dfps)", ms, fps));
            entryValues.add(Float.toString(ms));
        }

        entries.add(String.format(Locale.US, "No pacing"));
        entryValues.add(Float.toString(100));

        mSwapIntervalPreference.setEntries(entries.toArray(new String[0]));
        mSwapIntervalPreference.setEntryValues(entryValues.toArray(new String[0]));
        mSwapIntervalPreference.setDefaultValue(entries.get(0));

        Context context = getContext();
        if (context != null) {
            SharedPreferences sharedPreferences =
                    PreferenceManager.getDefaultSharedPreferences(context);
            sharedPreferences.registerOnSharedPreferenceChangeListener(this);
            // set default if it doesn't exist
            if (sharedPreferences.getString(mSwapIntervalKey, null) == null)
                sharedPreferences.edit()
                        .putString(mSwapIntervalKey, entryValues.get(0))
                        .apply();
            updateSwapIntervalSummary(sharedPreferences.getString(mSwapIntervalKey, null));
        }
    }

    private void updateSwapIntervalSummary(String swapIntervalString) {
        Resources resources;
        try {
            resources = getResources();
        } catch (IllegalStateException e) {
            // Swallow this and return early if we're not currently associated with an Activity
            return;
        }

        float swapInterval = Float.parseFloat(swapIntervalString);
        mSwapIntervalPreference.setSummary(String.format(Locale.US,
                "%s %.2f %s",
                resources.getString(R.string.swap_interval_summary_prologue),
                swapInterval,
                resources.getString(R.string.swap_interval_summary_epilogue)));
    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        if (key.equals(mSwapIntervalKey)) {
            updateSwapIntervalSummary(sharedPreferences.getString(mSwapIntervalKey, null));
        }
    }
}
