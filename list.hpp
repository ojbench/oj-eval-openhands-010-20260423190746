#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev, *next;

        node() : data(nullptr), prev(nullptr), next(nullptr) {}
        node(const T &value) : data(new T(value)), prev(nullptr), next(nullptr) {}
        ~node() {
            if (data) delete data;
        }
    };

protected:
    node *dummy;
    size_t _size;

    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        _size++;
        return cur;
    }
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        _size--;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        node *ptr;
        const list *owner;
    public:
        iterator() : ptr(nullptr), owner(nullptr) {}
        iterator(node *p, const list *o) : ptr(p), owner(o) {}
        iterator(const iterator &other) : ptr(other.ptr), owner(other.owner) {}

        iterator operator++(int) {
            if (!ptr || ptr == owner->dummy) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        iterator & operator++() {
            if (!ptr || ptr == owner->dummy) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        iterator operator--(int) {
            if (!ptr || ptr->prev == owner->dummy->prev && ptr == owner->dummy->next && owner->_size == 0) throw invalid_iterator(); // Empty list
            if (ptr->prev == owner->dummy) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        iterator & operator--() {
            if (!ptr || ptr->prev == owner->dummy) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        T & operator *() const {
            if (!ptr || ptr == owner->dummy) throw invalid_iterator();
            return *(ptr->data);
        }
        T * operator ->() const {
            if (!ptr || ptr == owner->dummy) throw invalid_iterator();
            return ptr->data;
        }
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        friend class list;
        friend class const_iterator;
    };
    class const_iterator {
    private:
        const node *ptr;
        const list *owner;
    public:
        const_iterator() : ptr(nullptr), owner(nullptr) {}
        const_iterator(const node *p, const list *o) : ptr(p), owner(o) {}
        const_iterator(const iterator &other) : ptr(other.ptr), owner(other.owner) {}
        const_iterator(const const_iterator &other) : ptr(other.ptr), owner(other.owner) {}

        const_iterator operator++(int) {
            if (!ptr || ptr == owner->dummy) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (!ptr || ptr == owner->dummy) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (!ptr || ptr->prev == owner->dummy) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        const_iterator & operator--() {
            if (!ptr || ptr->prev == owner->dummy) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        const T & operator *() const {
            if (!ptr || ptr == owner->dummy) throw invalid_iterator();
            return *(ptr->data);
        }
        const T * operator ->() const {
            if (!ptr || ptr == owner->dummy) throw invalid_iterator();
            return ptr->data;
        }
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        friend class list;
    };
    list() {
        dummy = new node();
        dummy->next = dummy->prev = dummy;
        _size = 0;
    }
    list(const list &other) {
        dummy = new node();
        dummy->next = dummy->prev = dummy;
        _size = 0;
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }
    virtual ~list() {
        clear();
        delete dummy;
    }
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
        return *this;
    }
    const T & front() const {
        if (_size == 0) throw container_is_empty();
        return *(dummy->next->data);
    }
    const T & back() const {
        if (_size == 0) throw container_is_empty();
        return *(dummy->prev->data);
    }
    iterator begin() {
        return iterator(dummy->next, this);
    }
    const_iterator cbegin() const {
        return const_iterator(dummy->next, this);
    }
    iterator end() {
        return iterator(dummy, this);
    }
    const_iterator cend() const {
        return const_iterator(dummy, this);
    }
    virtual bool empty() const {
        return _size == 0;
    }
    virtual size_t size() const {
        return _size;
    }

    virtual void clear() {
        node *cur = dummy->next;
        while (cur != dummy) {
            node *next = cur->next;
            delete cur;
            cur = next;
        }
        dummy->next = dummy->prev = dummy;
        _size = 0;
    }
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.owner != this) throw invalid_iterator();
        node *new_node = new node(value);
        insert(pos.ptr, new_node);
        return iterator(new_node, this);
    }
    virtual iterator erase(iterator pos) {
        if (pos.owner != this || pos.ptr == dummy || _size == 0) throw invalid_iterator();
        node *next_node = pos.ptr->next;
        erase(pos.ptr);
        delete pos.ptr;
        return iterator(next_node, this);
    }
    void push_back(const T &value) {
        insert(end(), value);
    }
    void pop_back() {
        if (_size == 0) throw container_is_empty();
        erase(--end());
    }
    void push_front(const T &value) {
        insert(begin(), value);
    }
    void pop_front() {
        if (_size == 0) throw container_is_empty();
        erase(begin());
    }
    void sort() {
        if (_size <= 1) return;
        T **arr = new T*[_size];
        node *cur = dummy->next;
        for (size_t i = 0; i < _size; ++i) {
            arr[i] = cur->data;
            cur = cur->next;
        }
        sjtu::sort<T*>(arr, arr + _size, [](T* const &a, T* const &b) {
            return *a < *b;
        });
        cur = dummy->next;
        for (size_t i = 0; i < _size; ++i) {
            cur->data = arr[i];
            cur = cur->next;
        }
        delete[] arr;
    }
    void merge(list &other) {
        if (this == &other) return;
        node *it1 = dummy->next;
        node *it2 = other.dummy->next;
        while (it1 != dummy && it2 != other.dummy) {
            if (*(it2->data) < *(it1->data)) {
                node *next_it2 = it2->next;
                other.erase(it2);
                insert(it1, it2);
                it2 = next_it2;
            } else {
                it1 = it1->next;
            }
        }
        while (it2 != other.dummy) {
            node *next_it2 = it2->next;
            other.erase(it2);
            insert(dummy, it2);
            it2 = next_it2;
        }
    }
    void reverse() {
        if (_size <= 1) return;
        node *cur = dummy;
        do {
            node *tmp = cur->next;
            cur->next = cur->prev;
            cur->prev = tmp;
            cur = tmp;
        } while (cur != dummy);
    }
    void unique() {
        if (_size <= 1) return;
        node *cur = dummy->next->next;
        while (cur != dummy) {
            if (*(cur->data) == *(cur->prev->data)) {
                node *next_node = cur->next;
                erase(cur);
                delete cur;
                cur = next_node;
            } else {
                cur = cur->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
