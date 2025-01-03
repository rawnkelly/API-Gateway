// gateway.hpp
#ifndef API_GATEWAY_H
#define API_GATEWAY_H

#include <iostream>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

class LRUCache {
private:
    struct Node {
        std::string key;
        std::string value;
        Node* prev;
        Node* next;
        Node(std::string k, std::string v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };
    
    int capacity;
    std::unordered_map<std::string, Node*> cache;
    Node* head;
    Node* tail;
    std::mutex cacheMutex;

    void moveToFront(Node* node);

public:
    LRUCache(int capacity);
    std::string get(const std::string& key);
    void put(const std::string& key, const std::string& value);
};

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;

public:
    ThreadPool(size_t threads);
    template<class F>
    void enqueue(F&& f);
    ~ThreadPool();
};

class APIGateway {
private:
    LRUCache cache;
    ThreadPool threadPool;
    std::vector<std::string> backends;
    size_t currentBackend;
    std::mutex backendMutex;

    std::string makeBackendRequest(const std::string& backend, const std::string& request);

public:
    APIGateway(int cacheSize, int threadCount);
    std::string handleRequest(const std::string& request);
};

#endif