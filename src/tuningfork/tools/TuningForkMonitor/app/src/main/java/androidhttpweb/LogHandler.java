package androidhttpweb;

public interface LogHandler {
    enum Level {
        VERBOSE, DEBUG, INFO, WARN, ERROR, ASSERT
    }
    void log(Level level, String text);
}
