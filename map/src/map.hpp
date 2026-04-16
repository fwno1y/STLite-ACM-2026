/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less <Key>
> class map {
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
private:
  struct Node {
      value_type* data;
      Node* left;
      Node* right;
      Node* parent;
      int height;

      Node(const value_type& v, Node* p) : left(nullptr),right(nullptr),parent(p),height(0) {
          data = reinterpret_cast<value_type*>(new char[sizeof(value_type)]);
          new (data) value_type(v);
      }

      ~Node() {
          data->~value_type();
          delete[] reinterpret_cast<char*> (data);
      }
  };

  Node* root;
  size_t size_;
  Compare compare;

  int height(Node* t) {
      if (t == nullptr) {
        return 0;
      }
      return t->height;
  }

  void updateHeight(Node* t) {
      if (t == nullptr) {
          return;
      }
      t->height = std::max(height(t->left),height(t->right)) + 1;
  }
  void LL(Node* & t) {
      Node* t1 = t->left;
      t->left = t1->right;
      if (t1->right != nullptr) {
          t1->right->parent = t;
      }
      t1->right = t;
      t1->parent = t->parent;
      t->parent = t1;
      updateHeight(t);
      updateHeight(t1);
      t = t1;
  }

  void LR(Node* & t) {
      RR(t->left);
      LL(t);
  }

  void RL(Node* & t) {
      LL(t->right);
      RR(t);
  }

  void RR(Node* & t) {
      Node* t1 = t->right;
      t->right = t1->left;
      if (t1->left != nullptr) {
          t1->left->parent = t;
      }
      t1->left = t;
      t1->parent = t->parent;
      t->parent = t1;
      updateHeight(t);
      updateHeight(t1);
      t = t1;
  }

  int getBalance(Node* t) {
      if (t == nullptr) {
          return 0;
      }
      return height(t->left) - height(t->right);
  }

  void balance(Node* & t) {
      int num = getBalance(t);
      if (num > 1) {
          if (getBalance(t->left) >= 0) {
              LL(t);
          }
          else {
              LR(t);
          }
      }
      else if (num < -1) {
          if (getBalance(t->right) <= 0) {
              RR(t);
          }
          else {
              RL(t);
          }
      }
      else {
          updateHeight(t);
      }
  }

  Node* copyNode(Node* cur, Node* p) {
      if (cur == nullptr) {
          return nullptr;
      }
      Node* newNode = new Node(*(cur->data),p);
      newNode->height = cur->height;
      newNode->left = copyNode(cur->left,newNode);
      newNode->right = copyNode(cur->right,newNode);
      return newNode;
  }

  void clearNode(Node* t) {
      if (t == nullptr) {
          return;
      }
      clearNode(t->left);
      clearNode(t->right);
      delete t;
  }

  Node* insertNode(Node* & t, Node* p, const value_type& v, bool& flag) {
      if (t == nullptr) {
          t = new Node(v, p);
          size_++;
          flag = true;
          return t;
      }
      Node* res = nullptr;
      if (compare(v.first,t->data->first)) {
          res = insertNode(t->left, t, v, flag);
      }
      else if (compare(t->data->first,v.first)) {
          res = insertNode(t->right, t, v, flag);
      }
      else {
          flag = false;
          return t;
      }
      balance(t);
      return res;
  }

  void removeNode(Node* & t, const Key& key) {
      if (t == nullptr) {
          return;
      }
      if (compare(key,t->data->first)) {
          removeNode(t->left,key);
      }
      else if (compare(t->data->first,key)) {
          removeNode(t->right,key);
      }
      else {
          if (t->left != nullptr && t->right != nullptr) {
              Node* oldNode = t;
              Node* tmp = t->right;
              while (tmp->left != nullptr) {
                  tmp = tmp->left;
              }
              if (tmp->parent != t) {
                  Node* p = tmp->parent;
                  p->left = tmp->right;
                  if (tmp->right != nullptr) {
                      tmp->right->parent = p;
                  }
                  tmp->right = t->right;
                  if (tmp->right != nullptr) {
                      tmp->right->parent = tmp;
                  }
              }
              tmp->left = t->left;
              if (tmp->left != nullptr) {
                  tmp->left->parent = tmp;
              }
              tmp->parent = t->parent;
              t = tmp;
              delete oldNode;
              size_--;
          }
          else {
              Node* oldNode = t;
              Node* p = t->parent;
              if (t->left != nullptr) {
                  t = t->left;
              }
              else {
                  t = t->right;
              }
              if (t != nullptr) {
                  t->parent = p;
              }
              delete oldNode;
              size_--;
          }
      }
      balance(t);
  }

  Node* findNode(const Key& key) const {
      Node* cur = root;
      while (cur != nullptr) {
          if (compare(key,cur->data->first)) {
              cur = cur->left;
          }
          else if (compare(cur->data->first,key)) {
              cur = cur->right;
          }
          else {
              return cur;
          }
      }
      return nullptr;
  }

public:
  class const_iterator;
  class iterator {
      friend class map;
   private:
      Node* p;
      const map* map_;
    /**
     * TODO add data members
     *   just add whatever you want.
     */
   public:
    iterator(Node* ptr = nullptr, const map* m = nullptr) {
        p = ptr;
        map_ = m;
      // TODO
    }

    iterator(const iterator &other) {
        p = other.p;
        map_ = other.map_;
      // TODO
    }

    /**
     * TODO iter++
     */
    iterator operator++(int) {
        iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    /**
     * TODO ++iter
     */
    iterator &operator++() {
        if (p == nullptr) {
            throw invalid_iterator();
        }
        if (p->right != nullptr) {
            p = p->right;
            while (p->left != nullptr) {
                p = p->left;
            }
        }
        else {
            Node* p1 = p->parent;
            while (p1 != nullptr && p == p1->right) {
                p = p1;
                p1 = p1->parent;
            }
            p = p1;
        }
        return *this;
    }

    /**
     * TODO iter--
     */
    iterator operator--(int) {
        iterator tmp = *this;
        --(*this);
        return tmp;
    }

    /**
     * TODO --iter
     */
    iterator &operator--() {
        if (map_ == nullptr) {
            throw invalid_iterator();
        }
        if (p == nullptr) {
            if (map_->root == nullptr) {
                throw invalid_iterator();
            }
            p = map_->root;
            while (p->right != nullptr) {
                p = p->right;
            }
        }
        else {
            if (p->left != nullptr) {
                p = p->left;
                while (p->right != nullptr) {
                    p = p->right;
                }
            }
            else {
                Node* p1 = p->parent;
                while (p1 != nullptr && p == p1->left) {
                    p = p1;
                    p1 = p1->parent;
                }
                if (p1 == nullptr) {
                    throw invalid_iterator();
                }
                p = p1;
            }
        }
        return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    value_type &operator*() const {
        if (p == nullptr) {
            throw invalid_iterator();
        }
        return *(p->data);
    }

    bool operator==(const iterator &rhs) const {
        if (p == rhs.p && map_ == rhs.map_) {
            return true;
        }
        return false;
    }

    bool operator==(const const_iterator &rhs) const {
        if (p == rhs.p && map_ == rhs.map_) {
            return true;
        }
        return false;
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
        if (p == rhs.p && map_ == rhs.map_) {
            return false;
        }
        return true;
    }

    bool operator!=(const const_iterator &rhs) const {
        if (p == rhs.p && map_ == rhs.map_) {
            return false;
        }
        return true;
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const
    noexcept {
        if (p == nullptr) {
            return nullptr;
        }
        return p->data;
    }
  };
  class const_iterator {
      friend class map;
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
      const Node* p;
      const map* map_;
    // data members.
   public:
    const_iterator(const Node* ptr = nullptr, const map* m = nullptr) {
        p = ptr;
        map_ = m;
      // TODO
    }

    const_iterator(const const_iterator &other) {
        p = other.p;
        map_ = other.map_;
      // TODO
    }

    const_iterator(const iterator &other) {
        p = other.p;
        map_ = other.map_;
      // TODO
    }

    const_iterator& operator++() {
        if (p == nullptr) {
            throw invalid_iterator();
        }
        Node *ptr = const_cast<Node*>(p);
        if (ptr->right) {
            ptr = ptr->right;
            while (ptr->left) {
                ptr = ptr->left;
            }
        } else {
            Node *p1 = ptr->parent;
            while (p1 != nullptr && ptr == p1->right) {
                ptr = p1;
                p1 = p1->parent;
            }
            ptr = p1;
        }
        p = ptr;
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    const_iterator& operator--() {
        Node *ptr = const_cast<Node*>(p);
        if (ptr == nullptr) {
            if (map_->root == nullptr) {
                throw invalid_iterator();
            }
            ptr = map_->root;
            while (ptr->right != nullptr) {
                ptr = ptr->right;
            }
        }
        else {
            if (ptr->left != nullptr) {
                ptr = ptr->left;
                while (ptr->right != nullptr) {
                    ptr = ptr->right;
                }
            }
            else {
                Node *p1 = ptr->parent;
                while (p1 != nullptr && ptr == p1->left) {
                    ptr = p1;
                    p1 = p1->parent;
                }
                if (p1 == nullptr) {
                    throw invalid_iterator();
                }
                ptr = p1;
            }
        }
        p = ptr;
        return *this;
    }

    const_iterator operator--(int) {
        const_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    const value_type &operator*() const {
        if (p == nullptr) {
            throw invalid_iterator();
        }
        return *(p->data);
    }

    bool operator==(const const_iterator &rhs) const {
        if (p == rhs.p && map_ == rhs.map_) {
            return true;
        }
        return false;
    }

    bool operator!=(const const_iterator &rhs) const {
        if (p == rhs.p && map_ == rhs.map_) {
            return false;
        }
        return true;
    }

    const value_type *operator->() const
    noexcept {
        if (p == nullptr) {
            return nullptr;
        }
        return p->data;
    }
    // And other methods in iterator.
    // And other methods in iterator.
    // And other methods in iterator.
  };

  /**
   * TODO two constructors
   */
  map() : root(nullptr), size_(0) {}

  map(const map &other) {
      size_ = other.size_;
      root = copyNode(other.root,nullptr);
      compare = other.compare;
  }

  /**
   * TODO assignment operator
   */
  map &operator=(const map &other) {
      if (this == &other) {
          return *this;
      }
      clearNode(root);
      root = copyNode(other.root,nullptr);
      size_ = other.size_;
      compare = other.compare;
      return *this;
  }

  /**
   * TODO Destructors
   */
  ~map() {
      clearNode(root);
  }

  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
      Node* res = findNode(key);
      if (res == nullptr) {
          throw index_out_of_bound();
      }
      return res->data->second;
  }

  const T &at(const Key &key) const {
      Node* res = findNode(key); // 传入指针this为const map*,与上面不同
      if (res == nullptr) {
          throw index_out_of_bound();
      }
      return res->data->second;
  }

  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
      bool flag;
      Node* res = insertNode(root,nullptr,value_type(key,T()),flag);
      return res->data->second;
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
      Node* res = findNode(key);
      if (res == nullptr) {
          throw index_out_of_bound();
      }
      return res->data->second;
  }

  /**
   * return a iterator to the beginning
   */
  iterator begin() {
      if (root == nullptr) {
          return end();
      }
      Node* t = root;
      while (t->left != nullptr) {
          t = t->left;
      }
      return iterator(t,this);
  }

  const_iterator cbegin() const {
      if (root == nullptr) {
          return cend();
      }
      Node* t = root;
      while (t->left != nullptr) {
          t = t->left;
      }
      return const_iterator(t,this);
  }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
      return iterator(nullptr,this);
  }

  const_iterator cend() const {
      return const_iterator(nullptr,this);
  }

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {
      if (size_ == 0) {
          return true;
      }
      return false;
  }

  /**
   * returns the number of elements.
   */
  size_t size() const {
      return size_;
  }

  /**
   * clears the contents
   */
  void clear() {
      clearNode(root);
      root = nullptr;
      size_ = 0;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
      bool flag;
      Node* res = insertNode(root,nullptr,value,flag);
      return {iterator(res,this),flag};
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
      if (pos == end() || pos.map_ != this) {
          throw invalid_iterator();
      }
      removeNode(root,pos->first);
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
      if (const_cast<map*>(this)->findNode(key) != nullptr) { //含const成员函数要用const_cast把this指针转为非const  报错信息“error: passing ‘const xxx’ as ‘this’ argument discards qualifiers [-fpermissive]”
          return 1;
      }
      return 0;
  }

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
      return iterator(findNode(key),this);
  }

  const_iterator find(const Key &key) const {
      return const_iterator(const_cast<map*>(this)->findNode(key),this);
  }
};

}

#endif