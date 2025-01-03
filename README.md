# API-Gateway

Implementation
A modern C++ API Gateway implementation featuring thread-safe request handling, LRU caching, and round-robin load balancing. Designed for high throughput and low latency in microservices architectures.

Architecture
The gateway utilizes a thread pool for concurrent request processing and an LRU cache for response optimization. Core components include:
* Thread-safe request handling with configurable worker pool
* LRU caching with O(1) lookup performance
* Round-robin load balancing across backend servers
* Comprehensive performance monitoring

Implementation Details
Key components:
* `ThreadPool` with `std::condition_variable` for efficient worker management
* `LRUCache` combining hash map and doubly-linked list for O(1) operations
* `std::mutex` protection for thread-safe concurrent access
* Performance metrics tracking for latency and cache effectiveness

Data Structures
* Cache: `std::unordered_map<std::string, Node*>` with linked list for LRU ordering
* Thread Pool: `std::queue<std::function<void()>>` for task management
* Backend Servers: `std::vector<std::string>` with round-robin selection

Features
* Thread-safe request processing
* Automatic response caching
* Load balancing across multiple backends
* Comprehensive performance monitoring
* Hardware-optimized thread pool sizing

Usage Example
```cpp
// Initialize gateway with cache size and thread count
APIGateway gateway(1000, std::thread::hardware_concurrency());

// Process requests
std::string response = gateway.handleRequest("/api/users/1");

// Run performance tests
GatewayTester tester(gateway);
auto metrics = tester.runLoadTest(1000, 10);
metrics.print();
```

Requirements
* C++17 or later compiler
* Standard Template Library
* POSIX threads support

The implementation prioritizes thread safety and performance while maintaining clean code structure through careful synchronization and memory management practices.
