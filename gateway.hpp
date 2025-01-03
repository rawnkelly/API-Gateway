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