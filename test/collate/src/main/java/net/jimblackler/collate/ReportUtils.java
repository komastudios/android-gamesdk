package net.jimblackler.collate;

import java.util.Map;

public class ReportUtils {
  static Map<String, Object> rowMetrics(Map<String, Object> row) {
    Map<String, Object> advice = (Map<String, Object>) row.get("advice");
    if (advice != null) {
      return (Map<String, Object>) advice.get("metrics");
    }
    Map<String, Object> deviceInfo = (Map<String, Object>) row.get("deviceInfo");
    if (deviceInfo != null) {
      return (Map<String, Object>) deviceInfo.get("baseline");
    }
    return (Map<String, Object>) row.get("metrics");
  }

  static long rowTime(Map<String, Object> row) {
    Map<String, Object> metrics = rowMetrics(row);
    if (metrics == null) {
      return 0;
    }
    return ((Number) ((Map<String, Object>) metrics.get("meta")).get("time")).longValue();
  }

  static Map<String, Object> getDeviceInfo(Iterable<Map<String, Object>> result) {
    for (Map<String, Object> line : result) {
      if (line == null) {
        continue;
      }
      Map<String, Object> deviceInfo = (Map<String, Object>) line.get("deviceInfo");
      if (deviceInfo != null) {
        return deviceInfo;
      }
    }
    return null;
  }
}
