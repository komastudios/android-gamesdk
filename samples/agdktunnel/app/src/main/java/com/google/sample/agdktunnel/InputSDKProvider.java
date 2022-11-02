/*
 * Copyright 2021 The Android Open Source Project
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

package com.google.sample.agdktunnel;

import static com.google.android.libraries.play.games.inputmapping.datamodel.InputEnums.REMAP_OPTION_ENABLED;
import static com.google.android.libraries.play.games.inputmapping.datamodel.InputEnums.REMAP_OPTION_DISABLED;

import android.view.KeyEvent;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputGroup;
import com.google.android.libraries.play.games.inputmapping.InputMappingProvider;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputAction;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputControls;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputContext;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputMap;
import com.google.android.libraries.play.games.inputmapping.datamodel.MouseSettings;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputIdentifier;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputEnums;

import java.util.Arrays;
import java.util.Collections;

public class InputSDKProvider implements InputMappingProvider {
    private static final String INPUTMAP_VERSION = "1.0.0";

    public enum InputActionsIds {
        NAVIGATE_UP,
        NAVIGATE_LEFT,
        NAVIGATE_DOWN,
        NAVIGATE_RIGHT,
        ENTER_MENU,
        EXIT_MENU,
        MOVE_UP,
        MOVE_LEFT,
        MOVE_DOWN,
        MOVE_RIGHT,
        PAUSE_GAME,
        MOUSE_MOVEMENT,
        ON_PAUSE_SELECT_OPTION,
        ON_PAUSE_NAVIGATE_UP,
        ON_PAUSE_NAVIGATE_DOWN,
        EXIT_PAUSE,
    }

    public enum InputGroupsIds {
        MENU_DIRECTIONAL_NAVIGATION,
        MENU_ACTION_KEYS,
        BASIC_MOVEMENT,
        MOUSE_MOVEMENT,
        PAUSE_MENU,
    }

    public enum InputContextIds {
        MENU_SCENE_CONTROLS(1),
        GAME_SCENE_CONTROLS(2),
        PAUSE_SCENE_CONTROLS(3);

        InputContextIds(int value) { this.value = value; }

        private final int value;
        public int value() { return value; }
    }

    public enum InputMapIds {
        GAME_INPUT_MAP,
    }

    private static final InputAction navigateMenuUpAction = InputAction.create(
            "Navigate up",
            InputActionsIds.NAVIGATE_UP.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_W),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction navigateMenuLeftAction = InputAction.create(
            "Navigate left",
            InputActionsIds.NAVIGATE_LEFT.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_A),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction navigateMenuDownAction = InputAction.create(
            "Navigate down",
            InputActionsIds.NAVIGATE_DOWN.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_S),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction navigateMenuRightAction = InputAction.create(
            "Navigate right",
            InputActionsIds.NAVIGATE_RIGHT.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_D),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputGroup basicMenuNavigationGroup = InputGroup.create(
            "Menu navigation keys",
            Arrays.asList(
                    navigateMenuUpAction,
                    navigateMenuLeftAction,
                    navigateMenuDownAction,
                    navigateMenuRightAction),
            InputGroupsIds.MENU_DIRECTIONAL_NAVIGATION.ordinal(),
            REMAP_OPTION_ENABLED);

    private static final InputAction enterMenuAction = InputAction.create(
            "Enter menu",
            InputActionsIds.ENTER_MENU.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_ENTER),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction exitMenuAction = InputAction.create(
            "Exit menu",
            InputActionsIds.EXIT_MENU.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_ESCAPE),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputGroup menuActionKeysGroup = InputGroup.create(
            "Menu keys",
            Arrays.asList(
                    enterMenuAction,
                    exitMenuAction),
            InputGroupsIds.MENU_ACTION_KEYS.ordinal(),
            REMAP_OPTION_ENABLED);

    private static final InputAction moveUpInputAction = InputAction.create(
            "Move Up",
            InputActionsIds.MOVE_UP.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_W),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction moveLeftInputAction = InputAction.create(
            "Move Left",
            InputActionsIds.MOVE_LEFT.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_A),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction moveDownInputAction = InputAction.create(
            "Move Down",
            InputActionsIds.MOVE_DOWN.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_S),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction moveRightInputAction = InputAction.create(
            "Move Right",
            InputActionsIds.MOVE_RIGHT.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_D),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction pauseGameAction = InputAction.create(
            "Pause game",
            InputActionsIds.PAUSE_GAME.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_ESCAPE),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputGroup movementInputGroup = InputGroup.create(
            "Basic movement",
            Arrays.asList(
                    moveUpInputAction,
                    moveLeftInputAction,
                    moveDownInputAction,
                    moveRightInputAction,
                    pauseGameAction
            ),
            InputGroupsIds.BASIC_MOVEMENT.ordinal(),
            REMAP_OPTION_ENABLED
    );

    private static final InputAction mouseInputAction = InputAction.create(
            "Move",
            InputGroupsIds.BASIC_MOVEMENT.ordinal(),
            InputControls.create(
                    Collections.emptyList(),
                    Collections.singletonList(InputControls.MOUSE_LEFT_CLICK)
            ),
            REMAP_OPTION_DISABLED);

    private static final InputGroup mouseMovementInputGroup = InputGroup.create(
            "Mouse movement",
            Collections.singletonList(mouseInputAction),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputGroupsIds.MOUSE_MOVEMENT.ordinal()),
            InputEnums.REMAP_OPTION_DISABLED
    );

    private static final InputAction navigateUpOnPauseAction = InputAction.create(
            "Up",
            InputActionsIds.ON_PAUSE_NAVIGATE_UP.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_W),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction navigateDownOnPauseAction = InputAction.create(
            "Down",
            InputActionsIds.MOVE_DOWN.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_S),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction selectOptionOnPauseInputAction = InputAction.create(
            "Select option",
            InputActionsIds.ON_PAUSE_SELECT_OPTION.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_ENTER),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputAction resumeGameInputAction = InputAction.create(
            "Resume game",
            InputActionsIds.MOVE_DOWN.ordinal(),
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_ESCAPE),
                    Collections.emptyList()),
            REMAP_OPTION_ENABLED);

    private static final InputGroup pauseMenuInputGroup = InputGroup.create(
            "Pause menu",
            Arrays.asList(
                    navigateUpOnPauseAction,
                    navigateDownOnPauseAction,
                    selectOptionOnPauseInputAction,
                    resumeGameInputAction),
            InputGroupsIds.PAUSE_MENU.ordinal(),
            InputEnums.REMAP_OPTION_DISABLED
    );

    public static final InputContext mainMenuContext = InputContext.create(
            "Main menu",
            InputContextIds.MENU_SCENE_CONTROLS.ordinal(),
            Arrays.asList(menuActionKeysGroup, basicMenuNavigationGroup)
    );

    public static final InputContext gameSceneContext = InputContext.create(
            "In game controls",
            InputContextIds.GAME_SCENE_CONTROLS.ordinal(),
            Arrays.asList(movementInputGroup, mouseMovementInputGroup)
    );

    public static final InputContext pauseGameInputContext = InputContext.create(
            "Pause menu",
            InputContextIds.PAUSE_SCENE_CONTROLS.ordinal(),
            Collections.singletonList(pauseMenuInputGroup)
    );

    public static final InputMap gameInputMap = InputMap.create(
            Arrays.asList(
                    basicMenuNavigationGroup,
                    menuActionKeysGroup,
                    movementInputGroup,
                    mouseMovementInputGroup,
                    pauseMenuInputGroup),
            MouseSettings.create(true, false),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputMapIds.GAME_INPUT_MAP.ordinal()),
            InputEnums.REMAP_OPTION_ENABLED,
            Arrays.asList(
                    InputControls.create(
                            Collections.singletonList(KeyEvent.KEYCODE_ESCAPE),
                            Collections.emptyList()
                    )
            )
    );

    @Override
    public InputMap onProvideInputMap() {
        return gameInputMap;
    }
}