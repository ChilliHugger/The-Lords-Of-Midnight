# Serve a wasm build with the COOP/COEP headers pthreads need.
# Usage: python3 scripts/wasm_serve.py [build_dir] [port]
import http.server, sys, functools
d = sys.argv[1] if len(sys.argv) > 1 else "Builds/wasm-lom/bin/midnight"
port = int(sys.argv[2]) if len(sys.argv) > 2 else 8099
class H(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        super().end_headers()
print(f"http://127.0.0.1:{port}/")
http.server.test(HandlerClass=functools.partial(H, directory=d), port=port, bind="127.0.0.1")
