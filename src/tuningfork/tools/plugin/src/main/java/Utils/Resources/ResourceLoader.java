/*
 * Copyright (C) 2020 The Android Open Source Project
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

package Utils.Resources;

import java.util.Locale;
import java.util.ResourceBundle;

public class ResourceLoader {

  private static ResourceLoader resourceLoader;
  private final ResourceBundle resourceBundle;

  private ResourceLoader() {
    Locale locale = new Locale("en");
    resourceBundle = ResourceBundle.getBundle("strings", locale);
  }

  public static synchronized ResourceLoader getInstance() {
    if (resourceLoader == null) {
      resourceLoader = new ResourceLoader();
    }
    return resourceLoader;
  }

  public String get(String key) {
    if (resourceBundle.containsKey(key)) {
      return resourceBundle.getString(key);
    } else {
      return "";
    }
  }
}
