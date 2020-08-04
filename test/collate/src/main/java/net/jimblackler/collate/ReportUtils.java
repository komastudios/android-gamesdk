package net.jimblackler.collate;

import org.json.JSONArray;
import org.json.JSONObject;

public class ReportUtils {
  static JSONObject rowMetrics(JSONObject row) {
    if (row.has("advice")) {
      return row.getJSONObject("advice").getJSONObject("metrics");
    } else if (row.has("deviceInfo")) {
      return row.getJSONObject("deviceInfo").getJSONObject("baseline");
    } else if (row.has("metrics")) {
      return row.getJSONObject("metrics");
    } else {
      return null;
    }
  }

  static long rowTime(JSONObject row) {
    JSONObject metrics = rowMetrics(row);
    if (metrics == null) {
      return 0;
    }
    return metrics.getJSONObject("meta").getLong("time");
  }

  static JSONObject getDeviceInfo(JSONArray result) {
    JSONObject deviceInfo = null;
    for (int idx2 = 0; idx2 != result.length(); idx2++) {
      JSONObject jsonObject = result.getJSONObject(idx2);
      if (jsonObject.has("deviceInfo")) {
        deviceInfo = jsonObject.getJSONObject("deviceInfo");
        break;
      }
    }
    return deviceInfo;
  }
}
