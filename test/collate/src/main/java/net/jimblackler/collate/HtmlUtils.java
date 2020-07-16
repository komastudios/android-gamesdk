package net.jimblackler.collate;

import java.io.IOException;
import java.io.StringWriter;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class HtmlUtils {
  public static Document getDocument() {
    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    DocumentBuilder builder;
    try {
      builder = factory.newDocumentBuilder();
    } catch (ParserConfigurationException e) {
      throw new IllegalStateException(e);
    }
    return builder.newDocument();
  }

  public static String toString(Node node) {
    try {
      Transformer transformer = TransformerFactory.newInstance().newTransformer();
      transformer.setOutputProperty(OutputKeys.METHOD, "html");
      try (StringWriter stringWriter = new StringWriter()) {
        transformer.transform(new DOMSource(node), new StreamResult(stringWriter));
        return stringWriter.toString();
      }
    } catch (IOException | TransformerException e) {
      throw new IllegalStateException(e);
    }
  }
}
