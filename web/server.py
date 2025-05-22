#!/usr/bin/env python3
import http.server
import socketserver
import os
import sys
import argparse
from urllib.parse import urlparse

# Parse command line arguments
parser = argparse.ArgumentParser(description='Simple HTTP server for WebAssembly content')
parser.add_argument('--port', type=int, default=8000, help='Port to run the server on (default: 8000)')
parser.add_argument('--dir', type=str, default='.', help='Directory to serve (default: current directory)')
args = parser.parse_args()

# Change to the specified directory
os.chdir(args.dir)

# Set up custom MIME types for WebAssembly
class WASMHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {
        **http.server.SimpleHTTPRequestHandler.extensions_map,
        '.wasm': 'application/wasm',
        '.js': 'application/javascript',
    }
    
    def end_headers(self):
        # Add CORS headers
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Origin, Content-Type, Accept')
        http.server.SimpleHTTPRequestHandler.end_headers(self)
    
    def log_message(self, format, *args):
        # Enhanced logging
        path = urlparse(self.path).path
        sys.stderr.write(f"[{self.log_date_time_string()}] {self.address_string()} - {path} - {format % args}\n")

# Set up and start the server
Handler = WASMHTTPRequestHandler
with socketserver.TCPServer(("", args.port), Handler) as httpd:
    print(f"Serving WebAssembly content at http://localhost:{args.port}")
    print(f"Press Ctrl+C to stop the server")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped.")