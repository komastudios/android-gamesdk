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

import math
import sys

from lib.graphing_components import *


class MemoryAccessSuiteHandler(SuiteHandler):

    def __init__(self, suite):
        super().__init__(suite)
        self.our_data = [
            d for d in self.data
            if d.operation_id == "MemoryAccessOperation"
        ]

        self.subplot_idx = 1

    @classmethod
    def can_handle_suite(cls, suite: Suite):
        return "Memory Access Test" in suite.name

    def summary(self):
        return None


    def do_rendering(self, graphs):
        num_graphs = 0
        for graph in graphs:
            if not graph["append"]:
                num_graphs += 1

        subplot_idx = 1

        max_time = 0
        max_value = 0

        for graph in graphs:
            if not graph["append"]:
                num_cols = int(math.sqrt(num_graphs))
                plt.subplot(((num_graphs-1) / num_cols) + 1, num_cols, self.subplot_idx)
                self.subplot_idx += 1

                max_time = graph["max_time"]
                max_value = graph["max_value"]

                plt.title(graph["desc"])

            max_time = max(max_time, graph["max_time"])
            max_value = max(max_value, graph["max_value"])
            plt.xlim(0, max_time)
            plt.ylim(0, max_value)

            procs = graph["procs"]
            for proc in procs:
                for thread in procs[proc]:
                    cpu_id = procs[proc][thread]['cpu_id']
                    if cpu_id in self.suite.build._d['big_cores']['cpus']:
                        ls = "solid"
                    elif cpu_id in self.suite.build._d['little_cores']['cpus']:
                        ls = "dotted"
                    else:
                        ls = "dashed"

                    plt.plot(procs[proc][thread]['time'],
                             procs[proc][thread]['value'],
                             ls = ls)
            
        plt.xlabel("time in ticks")
        plt.ylabel("accesses in kb")


    def render_plot(self):
        # There may be a delay after the New Pass event;
        # make a note so we save time_offset with the
        # first actual datum.
        need_time_offset = False
        time_offset = 0

        graphs = []

        for d in self.our_data:
            if d.get_custom_field("event") != None:
                if d.get_custom_field("event") == "New Pass":
                    need_time_offset = True
                    max_pass = pass_offset = 0
                    graphs.append({
                        "desc": d.get_custom_field("description"),
                        "append": d.get_custom_field("append_graph"),
                        "max_time": 0,
                        "max_value": 0,
                        "procs": {}
                    })
                continue

            if need_time_offset:
                time_offset = d.timestamp
                need_time_offset = False
            time = d.timestamp - time_offset

            pass_id = int(d.get_custom_field("pass_id")) + pass_offset
            max_pass = max(max_pass, pass_id)

            cpu_id = int(d.cpu_id)
            thread_id = int(d.get_custom_field("thread_id"))
            value = int(str(d.get_custom_field("bytes_cumulative")).replace(' bytes', ''))
            value /= 1024

            graphs[-1]["max_time"] = max(graphs[-1]["max_time"], time)
            graphs[-1]["max_value"] = max(graphs[-1]["max_value"], value)

            if not graphs[-1]["procs"].get(pass_id):
                graphs[-1]["procs"][pass_id] = {}
            
            procs = graphs[-1]["procs"]

            if not procs[pass_id].get(thread_id):
                procs[pass_id][thread_id] = \
                        {'cpu_id': cpu_id, 'time':[], 'value':[]}
            procs[pass_id][thread_id]['time'].append(time)
            procs[pass_id][thread_id]['value'].append(value)
        
        self.do_rendering(graphs)
