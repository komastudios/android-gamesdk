package com.google.android.apps.internal.games.memorytest;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import org.apache.commons.io.FileUtils;

class MmapFileGroup {
  private final ArrayList<MmapFileInfo> files = new ArrayList<>();
  private int current;

  MmapFileGroup(String mmapPath, int mmapFileCount, long mmapFileSize) throws IOException {
    int digits = Integer.toString(mmapFileCount - 1).length();
    FileUtils.cleanDirectory(new File(mmapPath));
    for (int i = 0; i < mmapFileCount; ++i) {
      String filename = mmapPath + "/" + String.format("test%0" + digits + "d.dat", i);
      MemoryTest.writeRandomFile(filename, mmapFileSize);
      files.add(new MmapFileInfo(filename));
    }
  }

  public MmapFileInfo alloc(long desiredSize) {
    while (!files.get(current).alloc(desiredSize)) {
      current = (current + 1) % files.size();
      files.get(current).reset();
    }
    return files.get(current);
  }
}
