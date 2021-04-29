package com.google.androidgamesdk.gameinput;

// The state of an editable text region.
public final class State {
    public State(String text_in, int selectionStart_in, int selectionEnd_in,
                 int composingRegionStart_in, int composingRegionEnd_in) {
        text = text_in;
        selectionStart = selectionStart_in;
        selectionEnd = selectionEnd_in;
        composingRegionStart = composingRegionStart_in;
        composingRegionEnd = composingRegionEnd_in;
    }

    public String text;
    public int selectionStart;
    public int selectionEnd;
    public int composingRegionStart;
    public int composingRegionEnd;
}
