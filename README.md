# C++ Multithreaded HTTP Server

A tiny multithreaded HTTP server written in C++17 that serves static files from the `./www` folder. Each connection is handled in a detached thread. Default port is `8080`.

## Features
- Serves files from `./www` (e.g., `/` maps to `./www/index.html`)
- Thread-per-connection handling
- Minimal HTTP request parsing (sufficient for simple GET requests)

## Project layout
```
server            # compiled binary (after build)
server.cpp        # server source
www/
  index.html      # example static file
```

## Build
Requires a C++17-capable compiler.

```bash
# Build (Linux)
g++ -std=c++17 -O2 -pthread server.cpp -o server
```

## Run
```bash
./server
```
Then open http://localhost:8080/ in your browser.

- Static root: `./www`
- Default port: `8080` (change by editing `#define PORT "8080"` in `server.cpp`)

## Benchmark
ApacheBench against `/` with 1000 requests and concurrency 10. Note: Numbers vary by machine and load.

Optional: install ApacheBench (Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install -y apache2-utils
```

Run benchmark
```bash
ab -n 1000 -c 10 http://localhost:8080/
```

Result :
```
This is ApacheBench, Version 2.3 <$Revision: 1903618 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 100 requests
Completed 200 requests
Completed 300 requests
Completed 400 requests
Completed 500 requests
Completed 600 requests
Completed 700 requests
Completed 800 requests
Completed 900 requests
Completed 1000 requests
Finished 1000 requests


Server Software:        
Server Hostname:        localhost
Server Port:            8080

Document Path:          /
Document Length:        186 bytes

Concurrency Level:      10
Time taken for tests:   0.330 seconds
Complete requests:      1000
Failed requests:        0
Total transferred:      205000 bytes
HTML transferred:       186000 bytes
Requests per second:    3029.54 [#/sec] (mean)
Time per request:       3.301 [ms] (mean)
Time per request:       0.330 [ms] (mean, across all concurrent requests)
Transfer rate:          606.50 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       2
Processing:     1    3   1.2      3       9
Waiting:        1    3   1.2      3       9
Total:          1    3   1.2      3       9

Percentage of the requests served within a certain time (ms)
  50%      3
  66%      3
  75%      4
  80%      4
  90%      5
  95%      5
  98%      7
  99%      8
 100%      9 (longest request)
```

## Notes / limitations
- Minimal HTTP: no headers/content-type; simple GET handling only.
- Thread-per-connection model; no connection keep-alive or pooling.
- Error handling is basic; unmatched paths return a simple error response.
- For best throughput, build with optimizations (as shown) and serve small static files.

---
Built and verified on Linux (build: PASS).
