package com.google.sample.agdktunnel;

import com.google.android.libraries.play.games.inputmapping.datamodel.InputGroup;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputAction;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputControls;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputMap;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputEnums;
import com.google.android.libraries.play.games.inputmapping.InputRemappingListener;
import java.util.List;

public class MyInputRemappingListener implements InputRemappingListener {

    @Override
    public void onInputMapChanged(InputMap inputMap) {
        if (inputMap.inputRemappingOption() == InputEnums.REMAP_OPTION_DISABLED) {
            return;
        }
        for (InputGroup inputGroup : inputMap.inputGroups()) {
            if (inputGroup.inputRemappingOption() == InputEnums.REMAP_OPTION_DISABLED) {
                continue;
            }
            for (InputAction inputAction : inputGroup.inputActions()) {
                if (inputAction.inputRemappingOption() != InputEnums.REMAP_OPTION_DISABLED &&
                        inputAction.remappedInputControls().isPresent()) {
                    // Found InputAction remapped by user
                    processRemappedAction(inputAction);
                }
            }
        }
    }

    private void processRemappedAction(InputAction remappedInputAction) {
        // Get remapped action info
        InputControls remappedControls = remappedInputAction.remappedInputControls().get();
        List<Integer> remappedKeyCodes = remappedControls.keycodes();
        List<Integer> mouseActions = remappedControls.mouseActions();
        String version = remappedInputAction.inputActionId().versionString().or("");
        Long remappedActionId = remappedInputAction.inputActionId().uniqueId();
        InputAction currentInputAction = getCurrentVersionInputActionWithId(
                remappedActionId, remappedInputAction);

        if (currentInputAction.inputActionId().versionString().equals(version)) {
            // TODO: make display changes to match controls used by the user
        } else {
            // Detected version of user-saved input action defers from current version
            // TODO: make display changes considering user-saved controls belong to an older version
        }
    }

    private InputAction getCurrentVersionInputActionWithId(
            Long inputActionId, InputAction defaultInputAction) {
        for (InputGroup inputGroup : InputSDKProvider.gameInputMap.inputGroups()) {
            for (InputAction inputAction : inputGroup.inputActions()) {
                if (inputAction.inputActionId().equals(inputActionId)) {
                    return inputAction;
                }
            }
        }
        return defaultInputAction;
    }
}
