package com.prefabulated.bouncyball;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.Bundle;

import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceManager;
import androidx.preference.PreferenceScreen;

public class SettingsFragment extends PreferenceFragmentCompat implements SharedPreferences.OnSharedPreferenceChangeListener {
    private Preference mSwapIntervalPreference;
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
                mSwapIntervalPreference = preference;
            }
        }

        Context context = getContext();
        if (context != null) {
            SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context);
            sharedPreferences.registerOnSharedPreferenceChangeListener(this);
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

        int swapInterval = Integer.parseInt(swapIntervalString);
        String summary = resources.getString(R.string.swap_interval_summary_prologue);
        summary += resources.getQuantityString(R.plurals.swap_interval_summary_epilogue, swapInterval, swapInterval);
        mSwapIntervalPreference.setSummary(summary);
    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        if (key.equals(mSwapIntervalKey)) {
            updateSwapIntervalSummary(sharedPreferences.getString(mSwapIntervalKey, null));
        }
    }
}
