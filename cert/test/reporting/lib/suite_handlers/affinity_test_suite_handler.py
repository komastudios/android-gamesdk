import sys
from typing import Any, Callable

from lib.chart_components import *


class AffinityChartRenderer(ChartRenderer):

    def __init__(self, chart: Chart):
        super().__init__(chart)

    def is_event_chart(self):
        return True

    def plot(self, fig, index, count, start_time_seconds, end_time_seconds):
        for i, d in enumerate(self.data):
            s = (d.timestamp / NS_PER_S) - start_time_seconds
            y = fig.get_ylim()[1]

            label = None
            color = [0.5, 0.5, 0.5]
            if d.field == "expected_cpu":
                expected = d.csv_entry.cpu_id
                actual = int(d.numeric_value)
                if expected != actual:
                    label = f"exp: {expected} act: {actual}"
                    color = [1, 0, 0]

            elif d.field == "message":
                if d.value == "work_started":
                    label = f"work_started cpu_id: {d.csv_entry.cpu_id}"
                elif d.value == "work_finished":
                    label = f"work_finished cpu_id: {d.csv_entry.cpu_id}"

            if label:
                line = fig.axvline(s, color=color, label=label)


class AffinityTestSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Affinity Test" in suite.suite_name

    def assign_renderer(self, chart: Chart):
        chart.set_renderer(AffinityChartRenderer(chart))
        return True

    def analyze(self, cb: Callable[[Any, Datum, str], None]):
        """Looks for interesting/warnable events in the cpu affinity data

        Looks for thread affinity errors inside the range between
        the last "work_started" datum, and first "work_finished" datum
        returns true if no warnings are issued
        """

        warnings_issued = False
        message_datums = []
        affinity_error_datums = []
        for topic in self.suite.datums_by_topic:
            op_id, field = topic
            for datum in self.suite.datums_by_topic[topic]:
                if field == "message":
                    message_datums.append(datum)
                elif field == "expected_cpu":
                    if datum.csv_entry.cpu_id != int(datum.numeric_value):
                        affinity_error_datums.append(datum)

        if affinity_error_datums:
            first_ts = 0
            last_ts = sys.maxsize

            for md in message_datums:
                if md.value == "work_started":
                    first_ts = max(first_ts, md.timestamp)
                elif md.value == "work_finished":
                    last_ts = min(last_ts, md.timestamp)

            for ed in affinity_error_datums:
                if ed.timestamp > first_ts and ed.timestamp < last_ts:
                    warnings_issued = True
                    cb(
                        self.suite, ed, f"Affinity error occurred at "
                        f"{ed.timestamp/NS_PER_S} inside of expected safe range "
                        f"by {(ed.timestamp-first_ts)/NS_PER_S}s")

        return warnings_issued
