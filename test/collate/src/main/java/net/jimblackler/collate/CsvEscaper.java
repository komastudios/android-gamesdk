package net.jimblackler.collate;

public class CsvEscaper {
  public static String escape(String value) {
    if (!value.contains(",")) {
      return value;
    }
    return "\"" + value + "\"";
  }
}
