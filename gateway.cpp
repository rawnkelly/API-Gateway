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