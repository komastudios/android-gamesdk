/*
 * Copyright (C) 2024 The Android Open Source Project
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
package com.google.androidgamesdk.gametextinput.test;

import static org.junit.Assert.*;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.concurrent.TimeUnit;
import android.view.KeyEvent;

import androidx.test.core.app.ActivityScenario;
import androidx.test.espresso.action.ViewActions;
import androidx.test.espresso.matcher.ViewMatchers;
import androidx.test.ext.junit.rules.ActivityScenarioRule;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.*;
// import static androidx.test.espresso.action.ViewActions.click;
// import static androidx.test.espresso.action.ViewActions.closeSoftKeyboard;
// import static androidx.test.espresso.action.ViewActions.typeText;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import static androidx.test.espresso.matcher.ViewMatchers.withText;

import com.google.androidgamesdk.gametextinput.test.R;
import com.google.androidgamesdk.gametextinput.test.MainActivity;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class InputTest {

    static private final String STRING_TO_BE_TYPED = "this is test text";
    static private final String RESULTING_STRING = "1";

    @Rule public ActivityScenarioRule<MainActivity> activityScenarioRule
            = new ActivityScenarioRule<>(MainActivity.class);

    @Test
    public void canTypeTextOnSoftwareKeyboard() {
      try (ActivityScenario<MainActivity> scenario = ActivityScenario.launch(MainActivity.class)) {
        onView(withId(R.id.displayed_text)).check(matches(withText(STRING_TO_BE_TYPED)));

        activityScenarioRule.getScenario().onActivity(activity -> activity.enableSoftKeyboard());

        // Type some text
        onView(withId(R.id.input_enabled_text_view)).perform(typeText("1"), closeSoftKeyboard());

        // Check that the text was changed.
        onView(withId(R.id.displayed_text)).check(matches(withText(RESULTING_STRING)));
      }
    }

    @Test
    public void canTypeTextOnHardwareKeyboard() {
      try (ActivityScenario<MainActivity> scenario = ActivityScenario.launch(MainActivity.class)) {
        onView(withId(R.id.displayed_text)).check(matches(withText(STRING_TO_BE_TYPED)));

        activityScenarioRule.getScenario().onActivity(activity -> activity.enableSoftKeyboard());

        // Type some text
        onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_1), closeSoftKeyboard());

        // Check that the text was changed.
        onView(withId(R.id.displayed_text)).check(matches(withText(RESULTING_STRING)));
      }
    }
}
