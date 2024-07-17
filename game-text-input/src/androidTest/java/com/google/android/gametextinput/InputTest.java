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

import androidx.test.ext.junit.rules.ActivityScenarioRule;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.*;
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

    static private final String INITIAL_VALUE = "this is test text";

    @Rule public ActivityScenarioRule<MainActivity> activityScenarioRule
            = new ActivityScenarioRule<>(MainActivity.class);

    @Test
    public void canTypeCharOnSoftwareKeyboard() {
      onView(withId(R.id.displayed_text)).check(matches(withText(INITIAL_VALUE)));

      activityScenarioRule.getScenario().onActivity(activity -> activity.enableSoftKeyboard());

      // Type some text
      onView(withId(R.id.input_enabled_text_view)).perform(typeText("s"), closeSoftKeyboard());

      // Check that the text was changed.
      onView(withId(R.id.displayed_text)).check(matches(withText("s")));
    }

    @Test
    public void canTypeTextOnSoftwareKeyboard() {
      onView(withId(R.id.displayed_text)).check(matches(withText(INITIAL_VALUE)));

      activityScenarioRule.getScenario().onActivity(activity -> activity.enableSoftKeyboard());

      // Type some text
      onView(withId(R.id.input_enabled_text_view)).perform(typeText("abcdef"), closeSoftKeyboard());

      // Check that the text was changed.
      onView(withId(R.id.displayed_text)).check(matches(withText("abcdef")));
    }

    @Test
    public void canTypeCharOnHardwareKeyboard() {
      onView(withId(R.id.displayed_text)).check(matches(withText(INITIAL_VALUE)));

      activityScenarioRule.getScenario().onActivity(activity -> activity.enableSoftKeyboard());

      // Type some text
      onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_1), closeSoftKeyboard());

      // Check that the text was changed.
      onView(withId(R.id.displayed_text)).check(matches(withText("1")));
    }

    @Test
    public void canTypeTextOnHardwareKeyboard() {
      onView(withId(R.id.displayed_text)).check(matches(withText(INITIAL_VALUE)));

      activityScenarioRule.getScenario().onActivity(activity -> activity.enableSoftKeyboard());

      // Type some text
      onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_1));
      onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_A));
      onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_2));
      onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_Z));
      onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_9));
      onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_R));
      onView(withId(R.id.input_enabled_text_view)).perform(closeSoftKeyboard());

      // Check that the text was changed.
      onView(withId(R.id.displayed_text)).check(matches(withText("1a2z9r")));
    }

    @Test
    public void backspaceWorks() {
      onView(withId(R.id.displayed_text)).check(matches(withText(INITIAL_VALUE)));

      activityScenarioRule.getScenario().onActivity(activity -> activity.enableSoftKeyboard());

      // Type some text
      onView(withId(R.id.input_enabled_text_view)).perform(typeText("abcdef"));
      onView(withId(R.id.input_enabled_text_view)).perform(pressKey(KeyEvent.KEYCODE_DEL));
      onView(withId(R.id.input_enabled_text_view)).perform(closeSoftKeyboard());

      // Check that the text was changed.
      onView(withId(R.id.displayed_text)).check(matches(withText("abcde")));
    }

    @Test
    public void multipleBackspaceWork() {
      onView(withId(R.id.displayed_text)).check(matches(withText(INITIAL_VALUE)));

      activityScenarioRule.getScenario().onActivity(activity -> activity.enableSoftKeyboard());

      // Type some text
      onView(withId(R.id.input_enabled_text_view)).perform(typeText("abcdef"));
      onView(withId(R.id.input_enabled_text_view)).perform(
          pressKey(KeyEvent.KEYCODE_DEL),
          pressKey(KeyEvent.KEYCODE_DEL),
          pressKey(KeyEvent.KEYCODE_DEL));
      onView(withId(R.id.input_enabled_text_view)).perform(typeText("xyz"));
      onView(withId(R.id.input_enabled_text_view)).perform(closeSoftKeyboard());

      // Check that the text was changed.
      onView(withId(R.id.displayed_text)).check(matches(withText("abcxyz")));
    }
}
