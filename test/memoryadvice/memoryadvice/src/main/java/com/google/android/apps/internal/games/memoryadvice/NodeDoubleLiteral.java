package com.google.android.apps.internal.games.memoryadvice;

class NodeDoubleLiteral extends NodeDouble {
  private final double value;

  NodeDoubleLiteral(double value) {
    this.value = value;
  }

  @Override
  Double evaluate(Lookup lookup) {
    return value;
  }
}
