// gateway.cpp
#include "gateway.hpp"

// LRUCache implementations
void LRUCache::moveToFront(Node* node) {
    if (node == head) return;
    
    node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    else tail = node->prev;
    
    node->next = head;
    node->prev = nullptr;
    head->prev = node;
    head = node;
}

LRUCache::LRUCache(int capacity) : capacity(capacity), head(nullptr), tail(nullptr) {}

std::string LRUCache::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    if (cache.find(key) == cache.end()) return "";
    
    Node* node = cache[key];
    moveToFront(node);
    return node->value;
}

void LRUCache::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    if (cache.find(key) != cache.end()) {
        cache[key]->value = value;
        moveToFront(cache[key]);
        return;
    }
    
    Node* newNode = new Node(key, value);
    cache[key] = newNode;
    
    if (!head) {
        head = tail = newNode;
    } else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
    
    if (cache.size() > capacity) {
        cache.erase(tail->key);
        Node* oldTail = tail;
        tail = tail->prev;
        tail->next = nullptr;
        delete oldTail;
    }
}

// ThreadPool implementations
ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] { 
                        return stop || !tasks.empty(); 
                    });
                    if (stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers) {
        worker.join();
    }
}

// APIGateway implementations
APIGateway::APIGateway(int cacheSize, int threadCount) 
    : cache(cacheSize), 
      threadPool(threadCount),
      currentBackend(0) {
    backends = {"backend1:8080", "backend2:8080", "backend3:8080"};
}

std::string APIGateway::makeBackendRequest(const std::string& backend, const std::string& request) {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    return "Response for: " + request + " from " + backend;
}

std::string APIGateway::handleRequest(const std::string& request) {
    std::string cachedResponse = cache.get(request);
    if (!cachedResponse.empty()) {
        return "CACHE_HIT: " + cachedResponse;
    

    std::string selectedBackend;
    {
        std::lock_guard<std::mutex> lock(backendMutex);
        selectedBackend = backends[currentBackend];
        currentBackend = (currentBackend + 1) % backends.size();
    }

    std::string response = makeBackendRequest(selectedBackend, request);
    cache.put(request, response);
    return response;
}