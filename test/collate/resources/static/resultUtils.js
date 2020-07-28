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

export function getValues(combined, object) {
  for (const [key, value] of Object.entries(object)) {
    if (Number.isInteger(value)) {
      combined[key] = value;
    } else if (value.constructor === Object) {
      getValues(combined, value);
    }
  }
}