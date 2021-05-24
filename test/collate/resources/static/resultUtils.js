export function rowMetrics(row) {
  if ('advice' in row) {
    return row.advice.metrics;
  } else if ('deviceInfo' in row) {
    return row.deviceInfo.baseline;
  } else if ('metrics' in row) {
    return row.metrics;
  } else {
    return undefined;
  }
}

export function rowTime(row) {
  if ('time' in row) {
    return row.time;
  }
  return rowMetrics(row).meta.time;
}

export function getValues(combined, object, path) {
  for (const [key, value] of Object.entries(object)) {
    if (typeof value === 'number') {
      combined[path + key] = value;
    } else if (value.constructor === Object) {
      getValues(combined, value, key + '/');
    }
  }
}