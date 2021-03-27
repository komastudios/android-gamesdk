package net.jimblackler.collate;

import com.google.gson.Gson;
import java.util.List;
import java.util.Map;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.Text;

public class DataExplorer {
  static Node getDataExplorer(Document document, Object obj) {
    Gson gson = new Gson();
    if (obj instanceof List) {
      Element ol = document.createElement("ol");
      List<Object> array = (List<Object>) obj;
      ol.setAttribute("start", "0");
      for (int idx = 0; idx != array.size(); idx++) {
        Element li = document.createElement("li");
        li.appendChild(getDataExplorer(document, array.get(idx)));
        ol.appendChild(li);
      }
      return ol;
    } else if (obj instanceof Map) {
      Element table = document.createElement("table");
      Map<String, Object> object = (Map<String, Object>) obj;
      for (Map.Entry<String, Object> entry : object.entrySet()) {
        Object value = entry.getValue();
        Element tr = document.createElement("tr");
        Element td0 = document.createElement("td");
        td0.appendChild(document.createTextNode(entry.getKey()));
        td0.setAttribute("class", "key");
        tr.appendChild(td0);
        Element td1 = document.createElement("td");
        if (value instanceof Map || value instanceof List) {
          Element details = document.createElement("details");
          Element summary = document.createElement("summary");
          Element span = document.createElement("span");
          Text text = document.createTextNode(value.toString());
          span.appendChild(text);
          summary.appendChild(span);
          details.appendChild(summary);
          details.appendChild(getDataExplorer(document, value));
          td1.appendChild(details);
        } else {
          td1.appendChild(getDataExplorer(document, value));
        }
        tr.appendChild(td1);
        table.appendChild(tr);
      }
      return table;
    } else {
      String string = gson.toJson(obj);
      Text textNode = document.createTextNode(string);
      if (string.contains(System.lineSeparator())) {
        Element pre = document.createElement("pre");
        pre.appendChild(textNode);
        return pre;
      } else if (string.startsWith("http://")) {
        Element a = document.createElement("a");
        a.appendChild(textNode);
        a.setAttribute("target", "_blank");
        a.setAttribute("href", string);
        return a;
      } else {
        return textNode;
      }
    }
  }
}
