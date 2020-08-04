import {buildDygraph} from './buildDygraph.js';
import {getDataExplorer} from './dataExplorer.js';
import {rowMetrics} from './resultUtils.js';

document.body.addEventListener('click', evt => {
  if (evt.target.href) {
    evt.target.href =
        evt.target.href.replace('gs://', 'https://storage.cloud.google.com/');
  }
}, false);

function rowTime(row) {
  if ('time' in row) {
    return row.time;
  }
  return rowMetrics(row).meta.time;
}

for (const result of data) {
  const first = result[0];

  result.sort((a, b) => rowTime(a) > rowTime(b) ? 1 : -1);

  const section = document.createElement('section');
  document.body.appendChild(section);

  if ('extra' in first) {
    const extra = first.extra;
    if ('fromLauncher' in extra) {
      const fromLauncher = extra.fromLauncher;
      const h1 = document.createElement('h1');
      section.appendChild(h1);
      let fullName;
      if (fromLauncher.name.indexOf(fromLauncher.manufacturer) === -1) {
        fullName = `${fromLauncher.manufacturer} ${fromLauncher.name}`;
      } else {
        fullName = fromLauncher.name;
      }
      fullName += ` (${fromLauncher.id})`;
      h1.appendChild(document.createTextNode(fullName));
      const img = document.createElement('img');
      img.src = fromLauncher.thumbnailUrl;
      h1.appendChild(img);
    }
    {
      const paragraph = document.createElement('p');
      section.appendChild(paragraph);
      {
        const span = document.createElement('span');
        span.classList.add('link');
        const anchor = document.createElement('a');
        anchor.href = extra.resultsPage;
        anchor.appendChild(document.createTextNode('Results page'));
        span.appendChild(anchor);
        paragraph.appendChild(span);
      }
      {
        const span = document.createElement('span');
        span.classList.add('link');
        const toolLogs = extra.step.testExecutionStep.toolExecution.toolLogs;
        for (let idx = 0; idx !== toolLogs.length; idx++) {
          const anchor = document.createElement('a');
          span.appendChild(anchor);
          anchor.href = toolLogs[idx].fileUri;
          anchor.appendChild(document.createTextNode('Logs'));
        }
        paragraph.appendChild(span);
      }
    }
  }

  let deviceInfo;
  for (let idx = 0; idx !== result.length; idx++) {
    const row = result[idx];
    if ('deviceInfo' in row) {
      deviceInfo = row.deviceInfo;
      break;
    }
  }

  {
    const paragraph = document.createElement('p');
    section.appendChild(paragraph);
    paragraph.appendChild(getDataExplorer(first));
    paragraph.appendChild(getDataExplorer(deviceInfo));
    const anchor = document.createElement('a');
    const label = document.createTextNode('Download JSON');
    anchor.appendChild(label);
    anchor.setAttribute('href', '_blank');
    anchor.addEventListener('click', ev => {
      anchor.setAttribute(
          'href',
          'data:application/json,' +
              encodeURIComponent(JSON.stringify(result, null, ' ')));
    })
    anchor.setAttribute('download', 'data');
    paragraph.appendChild(anchor);
  }
  const graphDiv = document.createElement('div');
  section.appendChild(graphDiv);
  graphDiv.classList.add('graph');

  buildDygraph(graphDiv, deviceInfo, result);

  let totalDuration = 0;
  let durationCount = 0;
  for (const row of result) {
    let metrics = rowMetrics(row);
    if (!metrics) {
      continue;
    }

    const advice = row.advice;
    if (advice && 'meta' in advice) {
      totalDuration += advice.meta.duration;
      durationCount++;
    }
  }

  if (durationCount > 0) {
    const paragraph = document.createElement('p');
    section.appendChild(paragraph);
    paragraph.appendChild(document.createTextNode(
        `Average duration ${(totalDuration / durationCount).toFixed(2)}`));
  }
}