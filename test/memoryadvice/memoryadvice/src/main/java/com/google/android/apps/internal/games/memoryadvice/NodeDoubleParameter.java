package com.google.android.apps.internal.games.memoryadvice;

class NodeDoubleParameter extends NodeDouble {
  private final String parameter;

  NodeDoubleParameter(String parameter) {
    this.parameter = parameter;
  }

  @Override
  Double evaluate(Lookup lookup) throws LookupException {
    return lookup.lookup(parameter);
  }
}
