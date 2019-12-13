from pathlib import Path
import lib.report

if __name__ == "__main__":
    report = Path("./out/affinity_test/report_blueline_29.json")
    systrace = Path("./out/affinity_test/report_blueline_29_trace.html")
    keywords = ["MonitorOperation", "SAO::"]
    lib.report.merge_systrace(report, systrace, keywords)