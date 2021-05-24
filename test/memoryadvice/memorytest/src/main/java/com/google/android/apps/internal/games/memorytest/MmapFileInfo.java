package com.google.android.apps.internal.games.memorytest;

import java.io.File;

class MmapFileInfo {
  private final long fileSize;
  private final String path;
  private long allocSize;
  private long offset;

  MmapFileInfo(String path) {
    this.path = path;
    fileSize = new File(path).length();
    offset = 0;
  }

  public boolean alloc(long desiredSize) {
    offset += allocSize;
    long limit = fileSize - offset;
    allocSize = Math.min(limit, desiredSize);
    return allocSize > 0;
  }

  public void reset() {
    offset = 0;
    allocSize = 0;
  }

  public String getPath() {
    return path;
  }

  public long getOffset() {
    return offset;
  }

  public long getAllocSize() {
    return allocSize;
  }
}
