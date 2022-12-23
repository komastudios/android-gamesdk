package com.google.sample.agdktunnel;

import android.util.Log;

import com.google.android.libraries.play.games.inputmapping.datamodel.InputGroup;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputAction;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputControls;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputMap;
import com.google.android.libraries.play.games.inputmapping.datamodel.InputEnums;
import com.google.android.libraries.play.games.inputmapping.InputRemappingListener;
import java.util.List;

public class InputSDKRemappingListener implements InputRemappingListener {

    private static final String TAG = "InputSDKRemappingListener";

    @Override
    public void onInputMapChanged(InputMap inputMap) {
        Log.i(TAG, "Received update on input map remapped.");
        if (inputMap.inputRemappingOption() == InputEnums.REMAP_OPTION_DISABLED) {
            return;
        }
        for (InputGroup inputGroup : inputMap.inputGroups()) {
            if (inputGroup.inputRemappingOption() == InputEnums.REMAP_OPTION_DISABLED) {
                continue;
            }
            for (InputAction inputAction : inputGroup.inputActions()) {
                if (inputAction.inputRemappingOption() != InputEnums.REMAP_OPTION_DISABLED &&
                        !inputAction.inputControls().equals(inputAction.remappedInputControls())) {
                    // Found InputAction remapped by user
                    processRemappedAction(inputAction);
                }
            }
        }
    }

    private void processRemappedAction(InputAction remappedInputAction) {
        // Get remapped action info
        InputControls remappedControls = remappedInputAction.remappedInputControls();
        List<Integer> remappedKeyCodes = remappedControls.keycodes();
        List<Integer> mouseActions = remappedControls.mouseActions();
        String version = remappedInputAction.inputActionId().versionString();
        Long remappedActionId = remappedInputAction.inputActionId().uniqueId();
        InputAction currentInputAction = getCurrentVersionInputActionWithId(
                remappedActionId, remappedInputAction);
        InputControls originalControls = currentInputAction.inputControls();
        List<Integer> originalKeyCodes = originalControls.keycodes();

        Log.i(TAG, String.format(
                "Found input action with id %d remapped from key %s to key %s",
                remappedActionId,
                keyCodesToString(originalKeyCodes),
                keyCodesToString(remappedKeyCodes)));

        if (currentInputAction.inputActionId().versionString().equals(version)) {
            // TODO: make display changes to match controls used by the user
        } else {
            // Detected version of user-saved input action defers from current version
            // TODO: make display changes considering user-saved controls belong to an older version
        }
    }

    private InputAction getCurrentVersionInputActionWithId(
            Long inputActionId, InputAction defaultInputAction) {
        for (InputGroup inputGroup : InputSDKProvider.sGameInputMap.inputGroups()) {
            for (InputAction inputAction : inputGroup.inputActions()) {
                if (inputAction.inputActionId().equals(inputActionId)) {
                    return inputAction;
                }
            }
        }
        return defaultInputAction;
    }

    private String keyCodesToString(List<Integer> keyCodes) {
        StringBuilder builder = new StringBuilder();
        for (Integer keyCode : keyCodes) {
            if (!builder.toString().isEmpty()) {
                builder.append(" + ");
            }
            builder.append(keyCode);
        }
        return String.format("(%s)", builder);
    }
}
