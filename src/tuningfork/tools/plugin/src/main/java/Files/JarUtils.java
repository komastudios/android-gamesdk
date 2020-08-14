package Files;

import com.google.common.io.Files;
import java.io.File;
import java.io.IOException;
import java.util.Enumeration;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import org.apache.commons.io.FileUtils;

public class JarUtils {
    private JarUtils() {}

    public static File copyProtobufToTemporaryFolder(String jarPath, String folderPathRelativeToJar)
        throws IOException {
        File temporaryFolder = Files.createTempDir();
        try (JarFile jarFile = new JarFile(jarPath)) {
            Enumeration<JarEntry> jarEntries = jarFile.entries();
            while (jarEntries.hasMoreElements()) {
                JarEntry jarEntry = jarEntries.nextElement();
                String jarEntryName = jarEntry.getName();
                // Copy operating system specific proto compiler along with license
                if (jarEntryName.startsWith(folderPathRelativeToJar)
                    || jarEntryName.endsWith("LICENSE")) {
                    File currentFile = new File(temporaryFolder, jarEntryName);
                    if (jarEntry.isDirectory()) {
                        currentFile.mkdirs();
                    } else {
                        FileUtils.copyToFile(jarFile.getInputStream(jarEntry), currentFile);
                    }
                }
            }
        }
        FileUtils.forceDeleteOnExit(temporaryFolder);
        return temporaryFolder;
    }
}
