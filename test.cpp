#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <random>
#include <future>
#include "gateway.hpp"  // Include your gateway implementation

class GatewayTester {
private:
    APIGateway& gateway;
    std::vector<std::string> testEndpoints;
    
    struct TestMetrics {
        double avgLatency;
        double p95Latency;
        double p99Latency;
        int requestCount;
        int cacheHits;
        
        void print() {
            std::cout << "\nTest Results:" << std::endl;
            std::cout << "Average Latency: " << avgLatency * 1000 << "ms" << std::endl;
            std::cout << "P95 Latency: " << p95Latency * 1000 << "ms" << std::endl;
            std::cout << "P99 Latency: " << p99Latency * 1000 << "ms" << std::endl;
            std::cout << "Total Requests: " << requestCount << std::endl;
            std::cout << "Cache Hit Rate: " << (cacheHits * 100.0 / requestCount) << "%" << std::endl;
        }
    };

public:
    GatewayTester(APIGateway& gw) : gateway(gw) {
        // Initialize test endpoints
        testEndpoints = {
            "/api/users/1",
            "/api/users/2",
            "/api/products/1",
            "/api/orders/1",
            "/api/inventory/1"
        };
    }

    TestMetrics runLoadTest(int numRequests, int concurrentUsers) {
        std::vector<double> latencies;
        int cacheHits = 0;
        std::vector<std::future<void>> futures;
        std::mutex latencyMutex;

        auto testFunction = [&]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, testEndpoints.size() - 1);

            for (int i = 0; i < numRequests / concurrentUsers; i++) {
                std::string endpoint = testEndpoints[dis(gen)];
                
                auto start = std::chrono::high_resolution_clock::now();
                std::string response = gateway.handleRequest(endpoint);
                auto end = std::chrono::high_resolution_clock::now();
                
                double latency = std::chrono::duration<double>(end - start).count();
                
                {
                    std::lock_guard<std::mutex> lock(latencyMutex);
                    latencies.push_back(latency);
                    if (response.find("CACHE_HIT") != std::string::npos) {
                        cacheHits++;
                    }
                }
            }
        };

        // Launch concurrent users
        for (int i = 0; i < concurrentUsers; i++) {
            futures.push_back(std::async(std::launch::async, testFunction));
        }

        // Wait for all tests to complete
        for (auto& future : futures) {
            future.wait();
        }

        // Calculate metrics
        TestMetrics metrics;
        std::sort(latencies.begin(), latencies.end());
        metrics.avgLatency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        metrics.p95Latency = latencies[static_cast<int>(latencies.size() * 0.95)];
        metrics.p99Latency = latencies[static_cast<int>(latencies.size() * 0.99)];
        metrics.requestCount = numRequests;
        metrics.cacheHits = cacheHits;

        return metrics;
    }

    void runEndpointTest() {
        std::cout << "\nTesting individual endpoints:" << std::endl;
        for (const auto& endpoint : testEndpoints) {
            auto start = std::chrono::high_resolution_clock::now();
            std::string response = gateway.handleRequest(endpoint);
            auto end = std::chrono::high_resolution_clock::now();
            
            double latency = std::chrono::duration<double>(end - start).count();
            std::cout << "Endpoint: " << endpoint << std::endl;
            std::cout << "Response: " << response << std::endl;
            std::cout << "Latency: " << latency * 1000 << "ms\n" << std::endl;
        }
    }

    void runCacheTest() {
        std::cout << "\nTesting cache effectiveness:" << std::endl;
        std::string testEndpoint = "/api/users/1";
        
        // First request (cache miss)
        auto start1 = std::chrono::high_resolution_clock::now();
        gateway.handleRequest(testEndpoint);
        auto end1 = std::chrono::high_resolution_clock::now();
        double latency1 = std::chrono::duration<double>(end1 - start1).count();
        
        // Second request (should be cache hit)
        auto start2 = std::chrono::high_resolution_clock::now();
        gateway.handleRequest(testEndpoint);
        auto end2 = std::chrono::high_resolution_clock::now();
        double latency2 = std::chrono::duration<double>(end2 - start2).count();
        
        std::cout << "First request (cache miss): " << latency1 * 1000 << "ms" << std::endl;
        std::cout << "Second request (cache hit): " << latency2 * 1000 << "ms" << std::endl;
        std::cout << "Cache speedup: " << (latency1 / latency2) << "x" << std::endl;
    }
};
