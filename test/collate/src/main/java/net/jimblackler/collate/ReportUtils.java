package net.jimblackler.collate;

import java.util.Map;

public class ReportUtils {
  static Map<String, Object> rowMetrics(Map<String, Object> row) {
    if (row.containsKey("advice")) {
      return (Map<String, Object>) ((Map<String, Object>) row.get("advice")).get("metrics");
    } else if (row.containsKey("deviceInfo")) {
      return (Map<String, Object>) ((Map<String, Object>) row.get("deviceInfo")).get("baseline");
    } else if (row.containsKey("metrics")) {
      return (Map<String, Object>) row.get("metrics");
    } else {
      return null;
    }
  }

  static long rowTime(Map<String, Object> row) {
    Map<String, Object> metrics = rowMetrics(row);
    if (metrics == null) {
      return 0;
    }
    return ((Number) ((Map<String, Object>) metrics.get("meta")).get("time")).longValue();
  }

  static Map<String, Object> getDeviceInfo(Iterable<Object> result) {
    Map<String, Object> deviceInfo = null;
    for (Object data : result) {
      Map<String, Object> line = (Map<String, Object>) data;
      if (line.containsKey("deviceInfo")) {
        deviceInfo = (Map<String, Object>) line.get("deviceInfo");
        break;
      }
    }
    return deviceInfo;
  }
}
