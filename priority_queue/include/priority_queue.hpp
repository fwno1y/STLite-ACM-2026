#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cmath>       // in case you need it
#include <cstddef>     // for size_t
#include <functional>  // for std::less

#include "exceptions.hpp"

namespace sjtu {

/**
 * @brief A container automatically sorting its contents, similar to
 * std::priority_queue but with extra functionalities.
 *
 * The extra functionalities are:
 * - Merge two priority queues into one (with good time complexity).
 * - Clear all elements in the queue.
 * - Limited exception safety for some operations (e.g. push, pop, top, merge)
 * when the comparator throws exceptions from `Compare` only.
 *
 * This @priority_queue does not support passing an underlying container as a template parameter.
 * Also, it does not support passing a comparator object as a constructor argument.
 *
 */
template <class T, class Compare = std::less<T>>
class priority_queue {
public:
    //斜堆
    struct Node {
        T data;
        Node* lson;
        Node* rson;
        Node() = default;
        Node(const T& data) : data(data), lson(nullptr), rson(nullptr) {}
        ~Node() = default;
    };
private:
    Node* root;//根节点
    int cur_size;//当前大小
    Compare compare;//比较器
    //辅助函数
    //拷贝
    Node* copyNode(Node* other) {
        if (other == nullptr) {
            return nullptr;
        }
        Node* new_node = new Node(other->data);
        try {
            new_node->lson = copyNode(other->lson);
            new_node->rson = copyNode(other->rson);
        } catch (...) {
            delete new_node;
            throw;
        }
        return new_node;
    }
    //析构
    void clearNode(Node* node) {
        if (node == nullptr) {
            return;
        }
        clearNode(node->lson);
        clearNode(node->rson);
        delete node;
    }
    //合并
    Node* mergeNode(Node* a, Node* b) {
        if (a == nullptr) {
            return b;
        }
        if (b == nullptr) {
            return a;
        }
        if (compare(a->data,b->data)) {
            Node* tmp = a;
            a = b;
            b = tmp;
        }
        a->rson = mergeNode(a->rson,b);
        Node* tmp = a->rson;
        a->rson = a->lson;
        a->lson = tmp;
        return a;
    }

public:
    priority_queue() : root(nullptr), cur_size(0) {}
    priority_queue(const priority_queue& other) {
        root = copyNode(other.root);
        cur_size = other.cur_size;
    }
    ~priority_queue() {
        clear();
    }

    priority_queue& operator=(const priority_queue& other) {
        if (this != &other) {
            Node* new_node = copyNode(other.root);
            clear();
            root = new_node;
            cur_size = other.cur_size;
        }
        return *this;
    }

    /** Adds one element to the queue. */
    void push(const T& data) {
        Node* new_node = new Node(data);
        try {
            root = mergeNode(root,new_node);
        } catch (...) {
            delete new_node;
            throw;
        }
        cur_size++;
    }

    /**
     * Returns a read-only reference of the first element in the queue.
     *
     * @throws container_is_empty when the first element does not exist.
     */
    const T& top() const {
        if (empty()) {
            throw container_is_empty();
        }
        return root->data;
    }

    /**
     * Removes the first element in the queue.
     *
     * @throws container_is_empty when the first element does not exist.
     */
    void pop() {
        if (empty()) {
            throw container_is_empty();
        }
        Node* old_node = root;
        root = mergeNode(root->lson,root->rson);
        delete old_node;
        cur_size--;
    }

    /** Returns the number of elements in the queue. */
    size_t size() const {
        return cur_size;
    }

    /** Returns whether there is any element in the queue. */
    bool empty() const {
        if (cur_size == 0) {
            return true;
        }
        return false;
    }

    /** Clears all elements in the queue. */
    void clear() {
        clearNode(root);
        root = nullptr;
        cur_size = 0;
    }

    /**
     * @brief Merges two priority queues into one.
     *
     * The merged data shall be stored in the current priority queue and the
     * other priority queue shall be cleared after merging.
     *
     * The time complexity shall be O(log n) or better.
     */
    void merge(priority_queue& other) {
        if (this == &other) {
            return;
        }
        root = mergeNode(root,other.root);
        cur_size += other.cur_size;
        other.root = nullptr;
        other.cur_size = 0;
    }
};

}  // namespace sjtu

#endif