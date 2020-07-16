package net.jimblackler.collate;

import java.util.Iterator;
import org.json.JSONArray;
import org.json.JSONObject;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.Text;

public class DataExplorer {
  static Node getDataExplorer(Document document, Object obj) {
    if (obj instanceof JSONArray) {
      Element ol = document.createElement("ol");
      JSONArray array = (JSONArray) obj;
      ol.setAttribute("start", "0");
      for (int idx = 0; idx != array.length(); idx++) {
        Element li = document.createElement("li");
        li.appendChild(getDataExplorer(document, array.get(idx)));
        ol.appendChild(li);
      }
      return ol;
    } else if (obj instanceof JSONObject) {
      Element table = document.createElement("table");
      JSONObject object = (JSONObject) obj;
      Iterator<String> it = object.keys();
      while (it.hasNext()) {
        String key = it.next();
        Object value = object.get(key);

        Element tr = document.createElement("tr");
        Element td0 = document.createElement("td");
        td0.appendChild(document.createTextNode(key));
        td0.setAttribute("class", "key");
        tr.appendChild(td0);
        Element td1 = document.createElement("td");
        if (value instanceof JSONObject || value instanceof JSONArray) {
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
      String string = obj.toString();
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
