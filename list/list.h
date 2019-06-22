//
// Created by roman on 22.06.19.
//

#ifndef LIST_LIST_H
#define LIST_LIST_H


#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>


template <typename T>
class list {
private:
    struct node_base {
        node_base *prev;
        node_base *next;

        node_base(): prev(nullptr), next(nullptr)  {}

        node_base(node_base *prev, node_base *next) : prev(prev), next(next) {}

        void clear(node_base *finish);

        virtual ~node_base() = default;
    };
    struct node : node_base {
        T value;
        node(node_base* prev, node_base* next, const T& value): node_base(prev, next), value(value) {}
    };

public:
    list() noexcept;

    list(const list& other);
    list& operator=(list other);
    ~list();


    template<typename U>
    friend void swap(list<U>& lhs, list<U>& rhs) noexcept;

    bool empty() const;
    void clear();

    void push_back(const T &value);
    void pop_back();
    T& back();
    const T& back() const;

    void push_front(const T &value);
    void pop_front();
    T& front();
    const T& front() const;



    template<typename U>
    class Iterator {
    public:
        using difference_type = ptrdiff_t ;
        using value_type = U;
        using pointer = U*;
        using reference = U&;
        using iterator_category = std::bidirectional_iterator_tag;

        friend class list;

        template <typename V>
        Iterator(const Iterator<V>& other,
                 typename std::enable_if<std::is_same<U, const V>::value>::type* = nullptr);

        Iterator& operator++();
        const Iterator operator++(int);
        Iterator& operator--();
        const Iterator operator--(int);

        U& operator*() const;
        U* operator->() const;

        friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ == rhs.ptr_;
        }
        friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ != rhs.ptr_;
        }

    private:
        explicit Iterator(node_base* ptr);
        node_base* ptr_;
    };


    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    iterator insert(const_iterator it, const T &value);
    iterator erase(const_iterator it);
    iterator erase(const_iterator first, const_iterator last);
    iterator splice(const_iterator pos, list&, const_iterator first, const_iterator last);

private:
    node_base end_node;
    node_base* end_ = &end_node;
};


template<typename T>
void list<T>::node_base::clear(list<T>::node_base* finish) {
    node_base* ptr = next;
    node_base* nxt = ptr->next;
    while (ptr != finish) {
        delete ptr;
        ptr = nxt;
        nxt = nxt->next;
    }
}

template<typename T>
typename list<T>::iterator list<T>::begin() {
    return iterator(end_->next);
}

template<typename T>
typename list<T>::const_iterator list<T>::begin() const {
    return const_iterator(end_->next);
}

template<typename T>
typename list<T>::iterator list<T>::end() {
    return iterator(end_);
}

template<typename T>
typename list<T>::const_iterator list<T>::end() const {
    return const_iterator(end_);
}

template<typename T>
list<T>::list() noexcept {
    end_->next = end_;
    end_->prev = end_;
}

template<typename T>
list<T>::list(const list &other): list() {
    node_base *ptr = other.end_->next;
    while (ptr != other.end_) {
        push_back(static_cast<node*>(ptr)->value);
        ptr = ptr->next;
    }
}

template<typename T>
list<T> &list<T>::operator=(list other) {
    swap(other, *this);
    return *this;
}

template<typename T>
list<T>::~list() {
    end_->clear(end_);
}

template <typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept {
    std::swap(lhs.end_node, rhs.end_node);

    if (lhs.end_->prev == rhs.end_) {
        lhs.end_->prev = &lhs.end_node;
        lhs.end_->next = &lhs.end_node;
    }
    if (rhs.end_->prev == lhs.end_) {
        rhs.end_->prev = &rhs.end_node;
        rhs.end_->next = &rhs.end_node;
    }

    lhs.end_->prev->next = &lhs.end_node;
    lhs.end_->next->prev = &lhs.end_node;

    rhs.end_->prev->next = &rhs.end_node;
    rhs.end_->next->prev = &rhs.end_node;
}

template<typename T>
bool list<T>::empty() const {
    return end_->next == end_;
}

template<typename T>
void list<T>::clear() {
    end_->clear(end_);
    end_->prev = end_;
    end_->next = end_;
}

template<typename T>
void list<T>::push_front(const T &value) {
    auto new_node = new node(end_, end_->next, value);
    end_->next->prev = new_node;
    end_->next = new_node;
}

template<typename T>
void list<T>::push_back(const T &value) {
    auto new_node = new node(end_->prev, end_, value);
    end_->prev->next = new_node;
    end_->prev = new_node;
}

template<typename T>
void list<T>::pop_back() {
    node_base *tmp = end_->prev;
    tmp->prev->next = end_;
    end_->prev = end_->prev->prev;
    delete tmp;
}

template<typename T>
void list<T>::pop_front() {
    node_base *tmp = end_->next;
    tmp->next->prev = end_;
    end_->next = end_->next->next;
    delete tmp;
}

template<typename T>
T &list<T>::back() {
    return static_cast<node*>(end_->prev)->value;
}

template<typename T>
const T &list<T>::back() const {
    return static_cast<node*>(end_->prev)->value;
}

template<typename T>
T &list<T>::front() {
    return static_cast<node*>(end_->next)->value;
}

template<typename T>
const T &list<T>::front() const {
    return static_cast<node*>(end_->next)->value;
}

template<typename T>
typename list<T>::iterator list<T>::insert(list::const_iterator it, const T &value) {
    node_base* new_node = new node(it.ptr_->prev, it.ptr_, value);
    it.ptr_->prev->next = new_node;
    it.ptr_->prev = new_node;
    return iterator(new_node);
}

template<typename T>
typename list<T>::iterator list<T>::erase(list::const_iterator it) {
    iterator res = iterator(it.ptr_->next);
    it.ptr_->prev->next = it.ptr_->next;
    it.ptr_->next->prev = it.ptr_->prev;
    delete it.ptr_;
    return res;
}

template<typename T>
typename list<T>::iterator list<T>::erase(list::const_iterator first, list::const_iterator last) {
    auto tmp = new node_base(last.ptr_->prev, first.ptr_);

    first.ptr_->prev->next = last.ptr_;
    last.ptr_->prev->next = tmp;
    last.ptr_->prev = first.ptr_->prev;
    first.ptr_->prev = tmp;
    tmp->clear(tmp);
    delete tmp;
    return iterator(last.ptr_);
}

template<typename T>
typename list<T>::iterator list<T>::splice(list::const_iterator pos, list&, 
        list::const_iterator first, list::const_iterator last) {
    if (first == last) {
        return iterator(pos.ptr_);
    }
    node_base *tmp = last.ptr_;
    last.ptr_->prev->next = pos.ptr_;
    pos.ptr_->prev->next = first.ptr_;
    first.ptr_->prev->next = tmp;
    tmp = pos.ptr_->prev;
    pos.ptr_->prev = last.ptr_->prev;
    last.ptr_->prev = first.ptr_->prev;
    first.ptr_->prev = tmp;
    return iterator(pos.ptr_);
}

template<typename T>
template<typename U>
list<T>::Iterator<U>::Iterator(list::node_base *ptr): ptr_(ptr) {}

template<typename T>
template<typename U>
template<typename V>
list<T>::Iterator<U>::Iterator(const list::Iterator<V> &other,
                               typename std::enable_if<std::is_same<U, const V>::value>::type *): ptr_(other.ptr_) {}

template<typename T>
template<typename U>
typename list<T>::template Iterator<U> &list<T>::Iterator<U>::operator++() {
    ptr_ = ptr_->next;
    return *this;
}

template<typename T>
template<typename U>
typename list<T>::template Iterator<U> &list<T>::Iterator<U>::operator--() {
    ptr_ = ptr_->prev;
    return *this;
}

template<typename T>
template<typename U>
const typename list<T>::template Iterator<U> list<T>::Iterator<U>::operator++(int) {
    Iterator tmp(ptr_);
    ++(*this);
    return tmp;
}


template<typename T>
template<typename U>
const typename list<T>::template Iterator<U> list<T>::Iterator<U>::operator--(int) {
    Iterator tmp(ptr_);
    --(*this);
    return tmp;
}

template<typename T>
template<typename U>
U &list<T>::Iterator<U>::operator*() const {
    return static_cast<node*>(ptr_)->value;
}

template<typename T>
template<typename U>
U* list<T>::Iterator<U>::operator->() const {
    return &static_cast<node*>(ptr_)->value;
}

#endif //LIST_LIST_H
