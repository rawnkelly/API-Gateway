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