/*
 * Copyright (C) Google Inc.
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
#include "anim.hpp"
#include "dialog_scene.hpp"
#include "play_scene.hpp"
#include "welcome_scene.hpp"

#include "blurb.inl"
#include "strings.inl"

#include "game-input/gameinput.h"

#include <string>
#include <android/window.h>

#define TITLE_POS center, 0.15f
#define TITLE_FONT_SCALE 1.0f
#define TITLE_COLOR 0.0f, 1.0f, 0.0f

#define NAME_EDIT_POS center, 0.85f
#define NAME_EDIT_FONT_SCALE .7f
#define NAME_EDIT_COLOR 1.0f, 1.0f, 0.0f
#define NAME_EDIT_SIZE 1.0f, 0.2f

// button defaults:
#define BUTTON_COLOR 0.0f, 1.0f, 0.0f
#define BUTTON_SIZE 0.2f, 0.2f
#define BUTTON_FONT_SCALE 0.5f

// button geometry
#define BUTTON_PLAY_POS center, 0.5f
#define BUTTON_PLAY_SIZE 0.4f, 0.4f
#define BUTTON_PLAY_FONT_SCALE 1.0f

// size of all side buttons (story, about)
#define BUTTON_SIDEBUTTON_WIDTH (center - 0.4f)
#define BUTTON_SIDEBUTTON_HEIGHT 0.2f
#define BUTTON_SIDEBUTTON_SIZE BUTTON_SIDEBUTTON_WIDTH, BUTTON_SIDEBUTTON_HEIGHT

// position of each side button (the buttons on the sides of the PLAY button)
#define BUTTON_STORY_POS 0.1f + 0.5f * BUTTON_SIDEBUTTON_WIDTH, 0.5f
#define BUTTON_ABOUT_POS center + 0.3f + 0.5f * BUTTON_SIDEBUTTON_WIDTH, 0.5f
#define BUTTON_TEST_POS center + 0.3f + 0.5f * BUTTON_SIDEBUTTON_WIDTH, 0.75f
#define BUTTON_QUIT_POS center + 0.3f + 0.5f * BUTTON_SIDEBUTTON_WIDTH, 0.25f
#define INITIAL_NAME "noname"

static std::string sNameEdit(S_NAME_EDIT);

WelcomeScene::WelcomeScene() : mTextInputState{} {
    mTextInputState.text_UTF8 = INITIAL_NAME;
    mTextInputState.text_length = strlen(INITIAL_NAME);
    mTextInputState.selection.start = 0;
    mTextInputState.selection.end = mTextInputState.text_length;
    mTextInputState.composingRegion.start = -1;
    mTextInputState.composingRegion.end = -1;
}

WelcomeScene::~WelcomeScene() {
}

void WelcomeScene::RenderBackground() {
    RenderBackgroundAnimation(mShapeRenderer);
}

void WelcomeScene::OnButtonClicked(int id) {
    SceneManager *mgr = SceneManager::GetInstance();

    if (id == mPlayButtonId) {
        mgr->RequestNewScene(new PlayScene());
    } else if (id == mStoryButtonId) {
        mgr->RequestNewScene((new DialogScene())->SetText(BLURB_STORY)->SetSingleButton(S_OK,
                                                                                        DialogScene::ACTION_RETURN));
    } else if (id == mAboutButtonId) {
        mgr->RequestNewScene((new DialogScene())->SetText(BLURB_ABOUT)->SetSingleButton(S_OK,
                                                                                        DialogScene::ACTION_RETURN));
    } else if (id == mNameEdit->GetId()) {
        auto activity = NativeEngine::GetInstance()->GetAndroidApp()->activity;
        // NB: the UI is resized when the IME is shown and OnCreateWidgets is called again.
        sNameEdit = mTextInputState.text_UTF8;
        mNameEdit->SetText(sNameEdit.c_str());
        GameActivity_setTextInputState(activity, &mTextInputState);
        GameActivity_showSoftInput(NativeEngine::GetInstance()->GetAndroidApp()->activity, 0);
    } else if (id == mTestButtonId) {
        auto activity = NativeEngine::GetInstance()->GetAndroidApp()->activity;
        GameActivity_setWindowFlags(activity,
                                    AWINDOW_FLAG_KEEP_SCREEN_ON | AWINDOW_FLAG_TURN_SCREEN_ON |
                                    AWINDOW_FLAG_FULLSCREEN |
                                    AWINDOW_FLAG_SHOW_WHEN_LOCKED,
                                    0);
        GameActivity_setWindowFormat(activity, WINDOW_FORMAT_RGBA_8888);

        // Check disabling/enabling axis, including out of range values:
        GameActivityPointerInfo_disableAxis(-1);
        GameActivityPointerInfo_disableAxis(100);
        GameActivityPointerInfo_disableAxis(AMOTION_EVENT_AXIS_X);

        GameActivityPointerInfo_enableAxis(-1);
        GameActivityPointerInfo_enableAxis(100);
        GameActivityPointerInfo_enableAxis(AMOTION_EVENT_AXIS_X);
        GameActivityPointerInfo_enableAxis(AMOTION_EVENT_AXIS_Y);

    } else if (id == mQuitButtonId) {
        auto activity = NativeEngine::GetInstance()->GetAndroidApp()->activity;
        GameActivity_finish(activity);
    }
}

void WelcomeScene::OnTextInput() {
    auto activity = NativeEngine::GetInstance()->GetAndroidApp()->activity;
    GameInputState_set(&mTextInputState, GameActivity_getTextInputState(activity));
    sNameEdit = std::string(mTextInputState.text_UTF8);
    __android_log_print(ANDROID_LOG_DEBUG, "WelcomeScene", "Got game text %s", sNameEdit.c_str());
    mNameEdit->SetText(sNameEdit.c_str());
}

void WelcomeScene::DoFrame() {
    // update widget states based on signed-in status
    UpdateWidgetStates();

    // if the sign in or cloud save process is in progress, show a wait screen. Otherwise, not:
    SetWaitScreen(false);

    // draw the UI
    UiScene::DoFrame();
}

void WelcomeScene::UpdateWidgetStates() {
    // Build navigation
    AddNav(mPlayButtonId, UI_DIR_LEFT, mStoryButtonId);
    AddNav(mPlayButtonId, UI_DIR_RIGHT, mAboutButtonId);

    AddNav(mStoryButtonId, UI_DIR_RIGHT, mPlayButtonId);

    AddNav(mAboutButtonId, UI_DIR_LEFT, mPlayButtonId);

}

void WelcomeScene::OnStartGraphics() {
    UiScene::OnStartGraphics();
}

void WelcomeScene::OnCreateWidgets() {

    // create widgets
    float maxX = SceneManager::GetInstance()->GetScreenAspect();
    float center = 0.5f * maxX;

    // create the static title
    NewWidget()->SetText(S_TITLE)->SetCenter(TITLE_POS)->SetTextColor(TITLE_COLOR)
            ->SetFontScale(TITLE_FONT_SCALE)->SetTransition(UiWidget::TRANS_FROM_BOTTOM);

    // create the "play" button
    mPlayButtonId = NewWidget()->SetText(S_PLAY)->SetTextColor(BUTTON_COLOR)
            ->SetCenter(BUTTON_PLAY_POS)->SetSize(BUTTON_PLAY_SIZE)
            ->SetFontScale(BUTTON_PLAY_FONT_SCALE)->SetIsButton(true)
            ->SetTransition(UiWidget::TRANS_SCALE)->GetId();

    // story button
    mStoryButtonId = NewWidget()->SetTextColor(BUTTON_COLOR)->SetText(S_STORY)
            ->SetCenter(BUTTON_STORY_POS)->SetSize(BUTTON_SIDEBUTTON_SIZE)
            ->SetFontScale(BUTTON_FONT_SCALE)->SetIsButton(true)
            ->SetTransition(UiWidget::TRANS_FROM_RIGHT)->GetId();

    // about button
    mAboutButtonId = NewWidget()->SetTextColor(BUTTON_COLOR)->SetText(S_ABOUT)
            ->SetCenter(BUTTON_ABOUT_POS)->SetSize(BUTTON_SIDEBUTTON_SIZE)
            ->SetFontScale(BUTTON_FONT_SCALE)->SetIsButton(true)
            ->SetTransition(UiWidget::TRANS_FROM_RIGHT)->GetId();

    // create editable name field
    mNameEdit = NewWidget()->SetText(sNameEdit.c_str())->SetCenter(NAME_EDIT_POS)->SetTextColor(
                    NAME_EDIT_COLOR)
            ->SetFontScale(NAME_EDIT_FONT_SCALE)->SetTransition(UiWidget::TRANS_FROM_TOP)
            ->SetSize(NAME_EDIT_SIZE)->SetIsButton(true);

    // test button
    mTestButtonId = NewWidget()->SetTextColor(BUTTON_COLOR)->SetText(S_TEST)
            ->SetCenter(BUTTON_TEST_POS)->SetSize(BUTTON_SIDEBUTTON_SIZE)
            ->SetFontScale(BUTTON_FONT_SCALE)->SetIsButton(true)
            ->SetTransition(UiWidget::TRANS_FROM_TOP)->GetId();

    // quit button
    mQuitButtonId = NewWidget()->SetTextColor(BUTTON_COLOR)->SetText(S_QUIT)
            ->SetCenter(BUTTON_QUIT_POS)->SetSize(BUTTON_SIDEBUTTON_SIZE)
            ->SetFontScale(BUTTON_FONT_SCALE)->SetIsButton(true)
            ->SetTransition(UiWidget::TRANS_FROM_BOTTOM)->GetId();

    // "Play" button is the default button
    SetDefaultButton(mPlayButtonId);

    // enable/disable widgets as appropriate to signed in state
    UpdateWidgetStates();
}

void WelcomeScene::OnKillGraphics() {
    UiScene::OnKillGraphics();
}

