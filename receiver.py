"""Local TCP receiver and browser view for tracker reports (Python 3 standard library)."""
import argparse
import json
import math
import socketserver
import threading
from collections import deque
from datetime import datetime
from html import escape
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

reports = deque(maxlen=100)
lock = threading.Lock()


def record(raw):
    msg = json.loads(raw)
    if not isinstance(msg, dict):
        raise ValueError("expected an object")
    device = msg["id"]
    if not isinstance(device, str) or len(device) != 24 or any(
        c not in "0123456789abcdefABCDEF" for c in device
    ):
        raise ValueError("invalid device ID")
    lat, lon = msg["lat"], msg["lon"]
    for value, limit in ((lat, 90), (lon, 180)):
        if type(value) not in (int, float) or not -limit <= value <= limit or not math.isfinite(value):
            raise ValueError("invalid coordinate")
    if msg["step"] is not None and (type(msg["step"]) is not int or not 0 <= msg["step"] <= 0xFFFFFF):
        raise ValueError("invalid step count")
    stamp = datetime.strptime(msg["date"], "%Y-%m-%d %H:%M:%S")
    row = (device.upper(), stamp.isoformat(" "), f"{lon:.6f}", f"{lat:.6f}", "—" if msg["step"] is None else str(msg["step"]))
    with lock:
        if row not in reports:
            reports.appendleft(row)


class Device(socketserver.StreamRequestHandler):
    timeout = 10

    def handle(self):
        try:
            while frame := self.rfile.readline(512):
                if len(frame) >= 512:
                    return
                record(frame)
        except (OSError, ValueError, KeyError, TypeError, OverflowError):
            return


class Listener(socketserver.ThreadingTCPServer):
    allow_reuse_address = True
    daemon_threads = True


class Viewer(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path != "/":
            self.send_error(404)
            return
        with lock:
            snapshot = list(reports)
        rows = "".join("<tr>" + "".join(f"<td>{escape(v)}</td>" for v in row) + "</tr>"
                       for row in snapshot)
        body = ("""<!doctype html><html lang="zh-CN"><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><meta http-equiv="refresh" content="5">
<title>定位器上报记录</title><style>body{font:16px system-ui;margin:4vw;color:#16342e}
table{border-collapse:collapse}th,td{text-align:left;padding:.7em;border-bottom:1px solid #ccd8d4}
th{background:#edf5ef}main{overflow:auto}</style><h1>定位器上报记录</h1>
<p>最近 100 条 · 每 5 秒刷新 · GPS 时间为 UTC</p><main><table><thead><tr>
<th>设备 ID</th><th>GPS 时间</th><th>经度</th><th>纬度</th><th>步数</th>
</tr></thead><tbody>""" + (rows or '<tr><td colspan="5">等待设备上报</td></tr>')
                + "</tbody></table></main></html>").encode()
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--tcp-port", type=int, default=777)
    parser.add_argument("--web-port", type=int, default=8080)
    parser.add_argument("--web-bind", default="127.0.0.1")
    args = parser.parse_args()
    with Listener(("", args.tcp_port), Device) as tcp, ThreadingHTTPServer(
        (args.web_bind, args.web_port), Viewer
    ) as web:
        threading.Thread(target=tcp.serve_forever, daemon=True).start()
        print(f"TCP {args.tcp_port} | http://{args.web_bind}:{args.web_port}/", flush=True)
        try:
            web.serve_forever()
        except KeyboardInterrupt:
            pass
