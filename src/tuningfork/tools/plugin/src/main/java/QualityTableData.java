/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

import java.util.HashSet;
import java.util.Vector;

class QualityTableData {

  private Vector<String> colNames;
  private Vector<Vector> data;
  private int previousBinFile = 0;
  private HashSet<Integer> isEnum;

  public QualityTableData() {
    colNames = new Vector<>();
    data = new Vector<>();
    colNames.add("Quality level number");
    // dummy data - will be removed once the logic is implemented
    colNames.add("Field1" + " (int32)");
    colNames.add("Field2" + " (float)");
    colNames.add("Field3" + " (ENUM)");
    isEnum = new HashSet<>();
    isEnum.add(3);
  }

  public HashSet<Integer> getEnums() {
    return isEnum;
  }

  public Vector<String> getColNames() {
    return colNames;
  }

  public Vector<Vector> getData() {
    return data;
  }

  public void addData(Vector<Object> data) {
    data.insertElementAt(++previousBinFile, 0);
    this.data.add(data);
  }

  public void removeData(int index) {
    data.remove(index);
  }
}