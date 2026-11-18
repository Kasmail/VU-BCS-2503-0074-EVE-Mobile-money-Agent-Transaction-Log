// LinkedList.h - A1: primary data structure for the Mobile Money Agent Transaction Log.
// A singly linked list with head AND tail pointers: add at the end is O(1).
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstddef>
#include <utility>

template <typename T>
class LinkedList {
    struct Node {
        T data;
        Node* next;
    };

public:
    LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~LinkedList() { clear(); }

    // Deep copy: builds a brand-new chain of nodes.
    LinkedList(const LinkedList& other) : head_(nullptr), tail_(nullptr), size_(0) {
        for (Node* c = other.head_; c != nullptr; c = c->next) push_back(c->data);
    }

    // Copy-and-swap: safe for self-assignment, no leak.
    LinkedList& operator=(const LinkedList& other) {
        LinkedList tmp(other);
        swap(tmp);
        return *this;
    }

    void swap(LinkedList& o) noexcept {
        std::swap(head_, o.head_);
        std::swap(tail_, o.tail_);
        std::swap(size_, o.size_);
    }

    // ---- adding ---------------------------------------------------------
    // O(1): thanks to tail_ we never walk the chain.
    void push_back(const T& value) {
        Node* n = new Node{value, nullptr};
        if (tail_ == nullptr) { head_ = n; tail_ = n; }      // empty list
        else { tail_->next = n; tail_ = n; }
        ++size_;
    }

    // O(1)
    void push_front(const T& value) {
        Node* n = new Node{value, head_};
        head_ = n;
        if (tail_ == nullptr) tail_ = n;                     // list was empty
        ++size_;
    }

    // ---- removing -------------------------------------------------------
    // O(1). Returns false on an empty list.
    bool pop_front(T& out) {
        if (head_ == nullptr) return false;
        Node* old = head_;
        out = old->data;
        head_ = old->next;
        if (head_ == nullptr) tail_ = nullptr;               // list became empty
        delete old;
        --size_;
        return true;
    }

    // O(n): walk to position i (0 = first). Returns false if i is invalid.
    bool removeAt(std::size_t i, T* removed = nullptr) {
        if (i >= size_) return false;
        Node* prev = nullptr;
        Node* cur = head_;
        for (std::size_t k = 0; k < i; ++k) { prev = cur; cur = cur->next; }
        unlink(prev, cur, removed);
        return true;
    }

    // O(n): remove the first element for which pred(element) is true.
    template <typename Pred>
    bool removeFirstWhere(Pred pred, T* removed = nullptr) {
        Node* prev = nullptr;
        Node* cur = head_;
        while (cur != nullptr && !pred(cur->data)) { prev = cur; cur = cur->next; }
        if (cur == nullptr) return false;
        unlink(prev, cur, removed);
        return true;
    }

    // ---- reading --------------------------------------------------------
    const T* front() const { return head_ ? &head_->data : nullptr; }       // O(1)
    const T* back()  const { return tail_ ? &tail_->data : nullptr; }       // O(1)

    // O(n). nullptr if i is out of range. The pointer lets the caller update the record.
    T* at(std::size_t i) {
        if (i >= size_) return nullptr;
        Node* c = head_;
        for (std::size_t k = 0; k < i; ++k) c = c->next;
        return &c->data;
    }
    const T* at(std::size_t i) const { return const_cast<LinkedList*>(this)->at(i); }

    // O(n): first element for which pred is true, or nullptr.
    template <typename Pred>
    T* find(Pred pred) {
        for (Node* c = head_; c != nullptr; c = c->next)
            if (pred(c->data)) return &c->data;
        return nullptr;
    }

    // ---- bulk helpers used by sorting / file I/O -------------------------
    // O(n). dest must have room for size() items.
    void copyTo(T* dest) const {
        std::size_t i = 0;
        for (Node* c = head_; c != nullptr; c = c->next) dest[i++] = c->data;
    }
    // O(n). Replace the contents with src[0..n-1].
    void assign(const T* src, std::size_t n) {
        clear();
        for (std::size_t i = 0; i < n; ++i) push_back(src[i]);
    }

    // O(n): delete every node.
    void clear() {
        Node* c = head_;
        while (c != nullptr) {
            Node* next = c->next;        // remember the rest of the chain first
            delete c;
            c = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }

private:
    // Remove node 'cur' whose predecessor is 'prev' (nullptr if cur is the head).
    void unlink(Node* prev, Node* cur, T* removed) {
        if (removed) *removed = cur->data;
        if (prev == nullptr) head_ = cur->next;       // removing the first node
        else prev->next = cur->next;                  // bypass cur
        if (cur == tail_) tail_ = prev;               // removing the last node
        delete cur;
        --size_;
    }

    Node* head_;
    Node* tail_;
    std::size_t size_;
};

#endif
