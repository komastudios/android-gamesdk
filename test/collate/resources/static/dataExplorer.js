export function getDataExplorer(obj) {
  if (Array.isArray(obj)) {
    const ol = document.createElement('ol');
    ol.setAttribute('start', 0);
    for (let idx = 0; idx !== obj.length; idx++) {
      const li = document.createElement('li');
      li.appendChild(getDataExplorer(obj[idx]));
      ol.appendChild(li);
    }
    return ol;
  } else if (typeof obj === 'object') {
    const table = document.createElement('table');
    for (const [key, value] of Object.entries(obj)) {
      const tr = document.createElement('tr');
      const td0 = document.createElement('td');
      td0.classList.add('key');
      td0.appendChild(document.createTextNode(key));
      tr.appendChild(td0);
      const td1 = document.createElement('td');
      if (typeof value === 'object' || Array.isArray(value)) {
        const details = document.createElement('details');
        const summary = document.createElement('summary');
        const span = document.createElement('span');
        const text = document.createTextNode(JSON.stringify(value));
        span.appendChild(text);
        summary.appendChild(span);
        details.appendChild(summary);
        details.appendChild(getDataExplorer(value));
        td1.appendChild(details);
      } else {
        td1.appendChild(getDataExplorer(value));
      }
      tr.appendChild(td1);
      table.appendChild(tr);
    }
    return table;
  } else {
    if (typeof obj !== 'string') {
      obj = obj.toString();
    }
    const textNode = document.createTextNode(obj);
    if (/[\r\n]/.exec(obj)) {
      const pre = document.createElement('pre');
      pre.appendChild(textNode);
      return pre;
    } else {
      const a = document.createElement('a');
      a.appendChild(textNode);
      a.setAttribute('target', '_blank');
      a.href = obj;
      if (a.href === obj) {
        return a;
      } else {
        return textNode;
      }
    }
  }
}
