#
# Copyright 2019 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

from typing import List, Tuple

import sys
import math
import matplotlib.pyplot as plt

from lib.common import nanoseconds_to_seconds
from lib.report import Datum, Suite
from lib.graphers.suite_handler import SuiteHandler


class FilePerformanceSuiteHandler(SuiteHandler):
    def __init__(self, suite):
        super().__init__(suite)
        self.our_data = [
            d for d in self.data
            if d.operation_id == "IOPerformanceOperation"
        ]

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Workload Configuration Test" in suite.name

    @classmethod
    def can_render_summarization_plot(cls,
                                      suites: List['SuiteHandler']) -> bool:
        return False

    @classmethod
    def render_summarization_plot(cls, suites: List['SuiteHandler']) -> str:
        return None


    def do_rendering(self, graphs):
        fig = plt.figure()
        fig.text(0.5, 0.04, "TIME IN MS", ha='center', va='center')
        fig.text(0.06, 0.5, "DATA IN KB", ha='center', va='center',
                 rotation='vertical')

        plt.subplots_adjust(hspace = 0.4)

        subplot_idx = 1

        for graph in graphs:
            desc = graph["desc"]
            procs = graph["procs"]
            min_time = graph["min_time"]
            max_time = graph["max_time"]
            max_value = graph["max_value"]

            num_cols = int(math.sqrt(len(graphs)))
            plt.subplot(((len(graphs)-1) / num_cols) + 1, num_cols, subplot_idx)
            subplot_idx += 1

            plt.title(desc)
            plt.xlim(0, max_time - min_time)
            plt.ylim(0, max_value)

            for thread in procs:
                for i in range(len(procs[thread]['time'])):
                    procs[thread]['time'][i] -= min_time

                cpu_id = procs[thread]['cpu_id']
                ls = ""
                if cpu_id in self.suite.build._d['big_cores']['cpus']:
                    ls = "solid"
                elif cpu_id in self.suite.build._d['little_cores']['cpus']:
                    ls = "dotted"
                else:
                    ls = "dashed"

                plt.plot(procs[thread]['time'], procs[thread]['value'], ls = ls)


    def render_plot(self):
        graphs = []

        for d in self.our_data:
            time = d.timestamp / 1000000

            if d.get_custom_field("event") != None:
                if d.get_custom_field("event") == "Test Start":
                    graphs.append({
                        "desc": d.get_custom_field("description"),
                        "min_time": sys.maxsize,
                        "max_time": time,
                        "max_value": -1,
                        "procs": {}
                    })
                continue

            cpu_id = int(d.cpu_id)
            thread_id = int(d.get_custom_field("thread_id"))
            value = int(str(d.get_custom_field("bytes_cumulative")).replace(' bytes', ''))
            value /= 1024

            graphs[-1]["min_time"] = min(graphs[-1]["min_time"], time)
            graphs[-1]["max_time"] = max(graphs[-1]["max_time"], time)

            if not graphs[-1]["procs"].get(thread_id):
                graphs[-1]["procs"][thread_id] = \
                        {'cpu_id':cpu_id, 'time':[], 'value':[]}
            graphs[-1]["procs"][thread_id]['time'].append(time)
            graphs[-1]["procs"][thread_id]['value'].append(value)
            graphs[-1]["max_value"] = max(graphs[-1]["max_value"], value)

        self.do_rendering(graphs)
