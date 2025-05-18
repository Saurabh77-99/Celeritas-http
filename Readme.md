# High-Performance Custom HTTP Server in C++

Welcome to my **High-Performance Custom HTTP Server** project!  
This is a minimal HTTP/1.1 server built from scratch in C++ to explore low-level networking, concurrency, and protocol design — inspired by the kind of ultra-fast servers used in High-Frequency Trading (HFT) and performance-critical systems.

---

## 🛠️ Project Overview

This project is a hands-on journey to build a lightweight, efficient HTTP server with:

- **Manual socket programming** for accepting and handling TCP connections.
- **Basic HTTP/1.1 protocol parsing** supporting GET and POST methods.
- **Static file serving** for assets like HTML, CSS, JS, images, etc.
- **Custom routing** with dynamic endpoints using a simple router abstraction.
- **Multithreading with a thread pool** for concurrent client handling.
- **MIME type detection** based on file extensions.

---

## ✅ What’s Implemented So Far

- TCP socket server listening on a user-specified port.
- Parsing of incoming HTTP requests (method, path, headers, body).
- Serving static files under `/static/` path, with content-type detection.
- Basic routing with dynamic handler functions for custom endpoints (e.g., `/hello`, `/status`, `/echo`).
- Thread pool to efficiently handle multiple simultaneous connections.
- Proper HTTP response construction including status codes and headers.
- Safe resource handling (file streams, sockets).
- Basic error handling (404 for missing files/routes).

---

## 🚧 What’s Left To Implement

- **Non-blocking, event-driven IO** using `epoll` or similar to replace blocking threads, maximizing throughput and lowering latency.
- More **robust and full HTTP/1.1 compliance**, including:
  - Persistent connections (`Connection: keep-alive`)
  - More complete header parsing
  - Handling chunked transfer encoding
  - Supporting other HTTP methods beyond GET and POST
- **Advanced routing features**:
  - URL parameters
  - Query string parsing
  - Middleware support
- **Benchmarking and profiling tools** to measure and optimize throughput and latency.
- **Security features** like input validation, request size limits, and HTTPS support.
- **Static file caching and compression** (gzip, brotli).
- **Graceful shutdown** and signal handling.
- **Logging and monitoring** for production readiness.

---

## 🚀 How to Run and Test

1. Build the project using CMake:

    ```bash
    mkdir build && cd build
    cmake ..
    make
    ```

2. Start the server:

    ```bash
    ./server 8080
    ```

3. Test static file serving:

    ```bash
    curl http://localhost:8080/static/index.html
    ```

4. Test dynamic routes:

    ```bash
    curl http://localhost:8080/hello
    curl http://localhost:8080/status
    curl -X POST -d "test body" http://localhost:8080/echo
    ```

---

Feel free to contribute ideas or improvements!  
Questions or feedback? Just ask.

**Happy serving!** 🚀  
