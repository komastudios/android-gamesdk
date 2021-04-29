package com.google.androidgamesdk.gameinput;

import android.content.Context;
import android.os.Build.VERSION;
import android.text.Editable;
import android.text.SpannableStringBuilder;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;

import com.google.androidgamesdk.gameinput.GameInput.Pair;

public class InputConnection extends BaseInputConnection {
    private EditorInfo editorInfo;
    private Listener listener;
    private boolean mSoftKeyboardActive;
    private final String TAG;
    private final InputMethodManager imm;
    private Editable mEditable;
    private final View targetView;
    private final Settings settings;
    public static final int SPAN_EMPTY = -1;

    public InputConnection(Context ctx, View targetView, Settings settings) {
        super(targetView, settings.getEditorInfo().inputType != 0);
        this.targetView = targetView;
        this.settings = settings;
        this.editorInfo = this.settings.getEditorInfo();
        this.TAG = "gameinput.InputConnection";
        Object imm = ctx.getSystemService("input_method");
        if (imm == null) {
            throw new java.lang.RuntimeException("Can't get IMM");
        } else {
            this.imm = (InputMethodManager) imm;
            this.mEditable = (Editable) (new SpannableStringBuilder());
        }
    }

    public final boolean getSoftKeyboardActive() {
        return this.mSoftKeyboardActive;
    }

    public final void setSoftKeyboardActive(boolean active, int flags) {
        if (active) {
            this.targetView.setFocusableInTouchMode(true);
            this.targetView.requestFocus();
            this.imm.showSoftInput(this.targetView, flags);
        } else {
            this.imm.hideSoftInputFromWindow(this.targetView.getWindowToken(), flags);
            this.listener = (Listener) null;
        }

        this.mSoftKeyboardActive = active;
    }

    public final EditorInfo getEditorInfo() {
        return this.editorInfo;
    }

    public final void setEditorInfo(EditorInfo var1) {
        this.editorInfo = var1;
    }

    public final void setState(State state) {
        if (state == null) return;
        Log.d(this.TAG, "setState: '" + state.text + "', selection=(" + state.selectionStart + "," + state.selectionEnd + "), composing region=(" + state.composingRegionStart + "," + state.composingRegionEnd + ")");
        this.mEditable.clear();
        this.mEditable.insert(0, (CharSequence) state.text);
        this.setSelectionInternal(state.selectionStart, state.selectionEnd);
        this.setComposingRegionInternal(state.composingRegionStart, state.composingRegionEnd);
        this.informIMM();
    }

    private final void informIMM() {
        Pair selection = this.getSelection();
        Pair cr = this.getComposingRegion();
        this.imm.updateSelection(this.targetView, selection.first, selection.second, cr.first, cr.second);
    }

    public final Listener getListener() {
        return this.listener;
    }

    public final InputConnection setListener(Listener l) {
        this.listener = l;
        return this;
    }

    private final Pair getSelection() {
        return GameInput.getSelection(this.mEditable);
    }

    private final Pair getComposingRegion() {
        return GameInput.getComposingRegion(this.mEditable);
    }

    public Editable getEditable() {
        Log.d(this.TAG, "getEditable ");
        return this.mEditable;
    }

    private final void setSelectionInternal(int start, int end) {
        GameInput.setSelection(this.mEditable, start, end);
    }

    public boolean setSelection(int start, int end) {
        Log.d(this.TAG, "setSelection: " + start + ":" + end);
        this.setSelectionInternal(start, end);
        return true;
    }

    public boolean setComposingText(CharSequence text, int newCursorPosition) {
        Log.d(this.TAG, (new StringBuilder()).append("setComposingText: '").append(text).append("', newCursorPosition=").append(newCursorPosition).toString());
        if (text == null) {
            return false;
        } else {
            Pair composingRegion = this.getComposingRegion();
            if (composingRegion.first == -1) {
                composingRegion = this.getSelection();
                if (composingRegion.first == -1) {
                    composingRegion = new Pair(0, 0);
                }
            }

            this.mEditable.delete(composingRegion.first, composingRegion.second);
            this.mEditable.insert(composingRegion.first, text);
            this.setComposingRegion(composingRegion.first, composingRegion.first + text.length());
            composingRegion = this.getComposingRegion();
            int actualNewCursorPosition = newCursorPosition > 0 ? Math.min(composingRegion.second + newCursorPosition - 1, this.mEditable.length()) : Math.max(0, composingRegion.first + newCursorPosition);
            this.setSelection(actualNewCursorPosition, actualNewCursorPosition);
            this.stateUpdated(false);
            return true;
        }
    }

    private final void setComposingRegionInternal(int start_in, int end_in) {
        if (start_in == -1) {
            GameInput.removeComposingRegion(this.mEditable);
        } else {
            int start = Math.min(this.mEditable.length(), Math.max(0, start_in));
            int end = Math.min(this.mEditable.length(), Math.max(0, end_in));
            GameInput.setComposingRegion(this.mEditable, start, end);
        }

    }

    public boolean setComposingRegion(int start, int end) {
        Log.d(this.TAG, "setComposingRegion: " + start + ":" + end);
        this.setComposingRegionInternal(start, end);
        this.stateUpdated(false);
        return true;
    }

    public boolean finishComposingText() {
        Log.d(this.TAG, "finishComposingText");
        this.setComposingRegion(-1, -1);
        return true;
    }

    public boolean commitText(CharSequence text, int newCursorPosition) {
        Log.d(this.TAG, (new StringBuilder()).append("Commit: ").append(text).append(", new pos = ").append(newCursorPosition).toString());
        this.setComposingText(text, newCursorPosition);
        this.finishComposingText();
        this.informIMM();
        return true;
    }

    public boolean deleteSurroundingText(int beforeLength, int afterLength) {
        Log.d(this.TAG, "deleteSurroundingText: " + beforeLength + ":" + afterLength);
        Pair selection = this.getSelection();
        int shift = 0;
        if (beforeLength > 0) {
            this.mEditable.delete(Math.max(0, selection.first - beforeLength), selection.first);
            shift = beforeLength;
        }

        if (afterLength > 0) {
            this.mEditable.delete(Math.max(0, selection.second - shift), Math.min(this.mEditable.length(), selection.second - shift + afterLength));
        }

        this.stateUpdated(false);
        return true;
    }

    public boolean deleteSurroundingTextInCodePoints(int beforeLength, int afterLength) {
        Log.d(this.TAG, "deleteSurroundingTextInCodePoints: " + beforeLength + ":" + afterLength);
        return super.deleteSurroundingTextInCodePoints(beforeLength, afterLength);
    }

    public boolean sendKeyEvent(KeyEvent event) {
        if (VERSION.SDK_INT >= 24 && this.settings.getEditorInfo().inputType == 0 && event != null) {
            this.imm.dispatchKeyEventFromInputMethod(this.targetView, event);
        }

        Log.d(this.TAG, "sendKeyEvent");
        Pair selection = this.getSelection();
        if (selection.first == -1) {
            return true;
        } else if (event == null) {
            return false;
        } else if (event.getAction() != 0) {
            return true;
        } else {
            boolean handled = false;
            if (selection.first != selection.second) {
                this.mEditable.delete(selection.first, selection.second);
            } else if (event.getKeyCode() == 67 && selection.first > 0) {
                this.mEditable.delete(selection.first - 1, selection.first);
            } else if (event.getKeyCode() == 112 && selection.first < this.mEditable.length() - 1) {
                this.mEditable.delete(selection.first, selection.first + 1);
            }

            if (event.getKeyCode() != 112 && event.getKeyCode() != 67) {
                if (event.getKeyCode() == 59) {
                    handled = true;
                } else if (event.getKeyCode() == 60) {
                    handled = true;
                } else if (event.getKeyCode() == 23 || event.getKeyCode() == 20 || event.getKeyCode() == 19 || event.getKeyCode() == 21 || event.getKeyCode() == 22 || event.getKeyCode() == 269 || event.getKeyCode() == 271 || event.getKeyCode() == 268 || event.getKeyCode() == 270) {
                    handled = true;
                }
            } else {
                handled = true;
            }

            if (!handled) {
                this.mEditable.insert(selection.first, (CharSequence) Character.toString((char) event.getUnicodeChar()));
                int new_cursor = selection.first + 1;
                GameInput.setSelection(this.mEditable, new_cursor, new_cursor);
            }

            this.stateUpdated(false);
            return true;
        }
    }

    public CharSequence getSelectedText(int flags) {
        Pair selection = this.getSelection();
        return selection.first == -1 ? (CharSequence) "" : this.mEditable.subSequence(selection.first, selection.second);
    }

    public CharSequence getTextAfterCursor(int length, int flags) {
        Log.d(this.TAG, "getTextAfterCursor: " + length + ":" + flags);
        Pair selection = this.getSelection();
        if (selection.first == -1) {
            return (CharSequence) "";
        } else {
            int n = Math.min(selection.second + length, this.mEditable.length());
            CharSequence seq = this.mEditable.subSequence(selection.second, n);
            return (CharSequence) seq.toString();
        }
    }

    public CharSequence getTextBeforeCursor(int length, int flags) {
        Log.d(this.TAG, "getTextBeforeCursor: " + length + ", flags=" + flags);
        Pair selection = this.getSelection();
        if (selection.first == -1) {
            return (CharSequence) "";
        } else {
            int start = Math.max(selection.first - length, 0);
            CharSequence seq = this.mEditable.subSequence(start, selection.first);
            return (CharSequence) seq.toString();
        }
    }

    public boolean requestCursorUpdates(int cursorUpdateMode) {
        Log.d(this.TAG, "Request cursor updates: " + cursorUpdateMode);
        return super.requestCursorUpdates(cursorUpdateMode);
    }

    public void closeConnection() {
        Log.d(this.TAG, "closeConnection");
        super.closeConnection();
    }

    private final void stateUpdated(boolean dismissed) {
        Pair selection = this.getSelection();
        Pair cr = this.getComposingRegion();
        State state = new State(this.mEditable.toString(), selection.first, selection.second, cr.first, cr.second);
        Listener var10000 = this.listener;
        if (var10000 != null) {
            var10000.stateChanged(state, dismissed);
        }

    }

    public final View getTargetView() {
        return this.targetView;
    }

    public final Settings getSettings() {
        return this.settings;
    }

}
