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
    private static final String INPUTMAP_VERSION = "my.input.map.version";

    public enum InputActionsIds {
        MOVE_UP,
        MOVE_LEFT,
        MOVE_DOWN,
        MOVE_RIGHT,
        MOUSE_MOVEMENT,
    }

    public enum InputGroupsIds {
        BASIC_MOVEMENT,
        MOUSE_MOVEMENT,
    }

    public enum InputMapIds {
        GAME_INPUT_MAP,
    }

    public enum InputContextIds {
        KEYBOARD_MOVEMENT,
        MOUSE_MOVEMENT,
    }

    private static final InputAction moveUpInputAction = InputAction.create(
            "Move Up",
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_W),
                    Collections.emptyList()),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputActionsIds.MOVE_UP.ordinal()),
            InputEnums.REMAP_OPTION_ENABLED);

    private static final InputAction moveLeftInputAction = InputAction.create(
            "Move Left",
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_A),
                    Collections.emptyList()),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputActionsIds.MOVE_LEFT.ordinal()),
            InputEnums.REMAP_OPTION_ENABLED);

    private static final InputAction moveDownInputAction = InputAction.create(
            "Move Down",
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_S),
                    Collections.emptyList()),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputActionsIds.MOVE_DOWN.ordinal()),
            InputEnums.REMAP_OPTION_ENABLED);

    private static final InputAction moveRightInputAction = InputAction.create(
            "Move Right",
            InputControls.create(
                    Collections.singletonList(KeyEvent.KEYCODE_D),
                    Collections.emptyList()),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputActionsIds.MOVE_RIGHT.ordinal()),
            InputEnums.REMAP_OPTION_ENABLED);

    private static final InputAction mouseInputAction = InputAction.create(
            "Move",
            InputControls.create(
                    Collections.emptyList(),
                    Collections.singletonList(InputControls.MOUSE_LEFT_CLICK)
            ),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputActionsIds.MOUSE_MOVEMENT.ordinal()),
            InputEnums.REMAP_OPTION_DISABLED);

    private static final InputGroup movementInputGroup = InputGroup.create(
            "Basic movement",
            Arrays.asList(
                    moveUpInputAction,
                    moveLeftInputAction,
                    moveDownInputAction,
                    moveRightInputAction
            ),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputGroupsIds.BASIC_MOVEMENT.ordinal()),
            InputEnums.REMAP_OPTION_DISABLED
    );

    private static final InputGroup mouseMovementInputGroup = InputGroup.create(
            "Mouse movement",
            Collections.singletonList(mouseInputAction),
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputGroupsIds.MOUSE_MOVEMENT.ordinal()),
            InputEnums.REMAP_OPTION_DISABLED
    );

    public static final InputContext mouseInputContext = InputContext.create(
            "Mouse Input Context",
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputContextIds.MOUSE_MOVEMENT.ordinal()),
            Collections.singletonList(mouseMovementInputGroup)
    );

    public static final InputContext keyboardInputContext = InputContext.create(
            "Keyboard Input Context",
            InputIdentifier.create(
                    INPUTMAP_VERSION,
                    InputContextIds.KEYBOARD_MOVEMENT.ordinal()),
            Collections.singletonList(movementInputGroup)
    );

    public static final InputMap gameInputMap = InputMap.create(
            Arrays.asList(movementInputGroup, mouseMovementInputGroup),
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