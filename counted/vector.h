#ifndef VECTOR_H
#define VECTOR_H

#include <cstdio>
#include <iterator>
#include <variant>
#include <cstring>
#include <memory>
#include <algorithm>

using std::shared_ptr;

template<typename T>
struct deleter {
    void operator()(T const *p) {
        delete[] p;
    }
};

template<typename T>
class vector {
public:
    typedef T value_type;

    typedef T *iterator;
    typedef const T *const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    vector() : size_(0), pointer(nullptr) {}

    ~vector();

    vector(vector const &other);

    vector(vector &&other) noexcept;

    explicit vector(size_t size, const T &value = T());

    template<typename InputIterator>
    vector(InputIterator first, InputIterator last);

    vector &operator=(vector const &other);

    vector &operator=(vector &&other) noexcept;

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last);

    T &operator[](size_t i);

    const T &operator[](size_t i) const noexcept;

    T &front();

    const T &front() const noexcept;

    T &back();

    const T &back() const noexcept;

    void push_back(const T &value);

    void pop_back();

    T *data();

    const T *data() const noexcept;

    iterator begin() const noexcept;

    iterator end() const noexcept;

    reverse_iterator rbegin() const noexcept;

    reverse_iterator rend() const noexcept;

    bool empty() const noexcept;

    void reserve(size_t len);

    size_t capacity() const noexcept;

    void shrink_to_fit();

    void resize(size_t len, const T &value = T());

    void clear();

    void swap(vector &other);

    size_t size() const noexcept;

    iterator insert(const_iterator pos, T const &val);

    iterator erase(const_iterator pos);

    iterator erase(const_iterator first, const_iterator last);

    template<typename Y>
    friend void swap(vector<Y> &a, vector<Y> &b) noexcept;

    template<typename Y>
    friend bool operator==(const vector<Y> &a, const vector<Y> &b) noexcept;

    template<typename Y>
    friend bool operator!=(const vector<Y> &a, const vector<Y> &b) noexcept;

    template<typename Y>
    friend bool operator<(const vector<Y> &a, const vector<Y> &b) noexcept;

    template<typename Y>
    friend bool operator<=(const vector<Y> &a, const vector<Y> &b) noexcept;

    template<typename Y>
    friend bool operator>(const vector<Y> &a, const vector<Y> &b) noexcept;

    template<typename Y>
    friend bool operator>=(const vector<Y> &a, const vector<Y> &b) noexcept;

private:
    static const size_t curadd = sizeof(size_t) + sizeof(shared_ptr<T>);

    size_t size_ = 0;

    union {
        T element;
        shared_ptr<char> *pointer;
    };

    bool is_big() const noexcept;

    char *cdata() const noexcept;

    T *tdata() const noexcept;

    void unique();
};


template<typename T>
void swap(vector<T> &A, vector<T> &B) {
    A.swap(B);
}

#define big (*pointer)
#define small (element)

template<typename T>
vector<T>::vector(vector const &other): size_(other.size_) {
    if (other.is_big()) {
        char *cdata = new char[curadd];
        new(cdata) size_t(size_);
        pointer = new(cdata + sizeof(size_t)) shared_ptr<char>(*other.pointer);
    } else if (other.size_) {
        new(&element) T(other.element);
    }
}

template<typename T>
vector<T>::vector(vector &&other) noexcept {
    swap(other);
}

template<typename T>
vector<T>::vector(size_t size, const T &value): size_(size) {
    if (size == 1) {
        new(&element) T(value);
    } else if (size > 1) {

        char *cdata = new char[curadd + size_ * sizeof(T)];
        T *tdata = (T *) (cdata + curadd);

        for (size_t i = 0; i != size; i++) {
            new(tdata + i) T(value);
        }
        new(cdata) size_t(size);
        pointer = new(cdata + sizeof(size_t)) shared_ptr<char>(cdata, deleter<char>());
    }
}


template<typename T>
vector<T>::~vector() {
    if (is_big()) {
        for (size_t i = 0; i < size(); i++) {
            tdata()[i].~T();
        }
        big.~shared_ptr();
    } else if (size() == 1) {
        small.~T();
    }
}


template<typename T>
template<typename InputIterator>
vector<T>::vector(InputIterator first, InputIterator last) {
    while (first != last) {
        push_back(*first);
        ++first;
    }
}

template<typename T>
vector<T> &vector<T>::operator=(vector const &other) {
    if (this != &other) {
        vector(other).swap(*this);
    }
    return *this;
}

template<typename T>
vector<T> &vector<T>::operator=(vector &&other) noexcept {
    swap(other);
    return *this;
}

template<typename T>
void vector<T>::swap(vector &other) {
    if (is_big()) {
        unique();
    }
    if (other.is_big()) {
        other.unique();
    }

    if (is_big()) {
        if (other.is_big()) {
            std::swap(pointer, other.pointer);
        } else if (other.size()) {
            auto tmp = pointer;

            new(&element) T(other.element);
            other.element.~T();

            other.pointer = tmp;
        } else {
            other.pointer = pointer;
            pointer = nullptr;
        }
    } else if (size()) {
        if (other.is_big()) {
            T tmp(element);
            element.~T();

            pointer = other.pointer;
            new(&other.element) T(tmp);
        } else if (other.size()) {
            std::swap(element, other.element);
        } else {
            new(&other.element) T(element);
            element.~T();
            pointer = nullptr;
        }
    } else {
        if (other.is_big()) {
            pointer = other.pointer;
            other.pointer = nullptr;
        } else if (other.size()) {
            new(&element) T(other.element);
            other.element.~T();
            other.pointer = nullptr;
        } else {

        }
    }
    std::swap(size_, other.size_);
}

template<typename T>
bool vector<T>::is_big() const noexcept {
    return size_ > 1;
}

template<typename T>
size_t vector<T>::capacity() const noexcept {
    if (is_big()) {
        return *reinterpret_cast<size_t *>(cdata());
    }
    return 1;
}

template<typename T>
const T &vector<T>::operator[](size_t i) const noexcept {
    if (is_big()) {
        return tdata()[i];
    }
    return small;
}

template<typename T>
T &vector<T>::operator[](size_t i) {
    if (is_big()) {
        unique();
        return tdata()[i];
    }
    return small;
}

template<typename T>
void vector<T>::clear() {
    size_ = 0;
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (is_big()) {
        unique();
        bool need = is_big();
        size_t cur_size = size();

        char *cur_cdata = new char[curadd + sizeof(T) * cur_size];
        T *cur_tdata = reinterpret_cast<T *>(cur_cdata + curadd);

        for (size_t i = 0; i != cur_size; i++) {
            new(cur_tdata + i) T(operator[](i));
        }

        if (need) {
            vector().swap(*this);
        }
        size_ = cur_size;

        new(cur_cdata) size_t(size_);
        pointer = new(cur_cdata + sizeof(size_t)) shared_ptr<char>(cur_cdata, deleter<char>());
    }
}

template<typename T>
template<typename InputIterator>
void vector<T>::assign(InputIterator first, InputIterator last) {
    vector(first, last).swap(*this);
}

template<typename T>
T &vector<T>::front() {
    if (is_big()) {
        unique();
        return operator[](0);
    }
    return small;
}

template<typename T>
const T &vector<T>::front() const noexcept {
    if (is_big()) {
        return operator[](0);
    }
    return small;
}


template<typename T>
T &vector<T>::back() {
    if (is_big()) {
        unique();
        return operator[](size_ - 1);
    }
    return small;
}

template<typename T>
const T &vector<T>::back() const noexcept {
    if (is_big()) {
        return operator[](size_ - 1);
    }
    return small;
}

template<typename T>
void vector<T>::pop_back() {
    if (is_big()) {
        unique();
        tdata()[size_ - 1].~T();
    } else {
        element.~T();
    }
    if (size_ == 2) {
        auto tmp = front();
        tdata()[0].~T();
        new(&element) T(tmp);
    }
    size_--;
}

template<typename T>
T *vector<T>::data() {
    if (is_big()) {
        unique();
        return tdata();
    }
    return &small;
}

template<typename T>
const T *vector<T>::data() const noexcept {
    if (is_big()) {
        return tdata();
    }
    return const_cast<const T *>(&small);
}

template<typename T>
bool vector<T>::empty() const noexcept {
    return size_ == 0;
}

template<typename T>
void vector<T>::reserve(size_t len) {
    if (len > 1) {
        if (is_big()) {
            unique();
        }
        bool need = is_big();
        size_t cur_size = size();
        size_t new_len = std::max(len, cur_size);

        char *cur_cdata = new char[curadd + sizeof(T) * new_len];
        T *cur_tdata = reinterpret_cast<T *>(cur_cdata + curadd);

        for (size_t i = 0; i != cur_size; i++) {
            new(cur_tdata + i) T(operator[](i));
        }

        if (need) {
            vector().swap(*this);
        } else if (size_) {
            small.~T();
        }
        size_ = cur_size;

        new(cur_cdata) size_t(new_len);
        pointer = new(cur_cdata + sizeof(size_t)) shared_ptr<char>(cur_cdata, deleter<char>());
    }
}

template<typename T>
bool operator==(const vector<T> &a, const vector<T> &b) noexcept {
    if (a.size() != b.size()) return false;
    return memcmp(a.data(), b.data(), sizeof(T) * a.size()) == 0;
}

template<typename T>
bool operator!=(const vector<T> &a, const vector<T> &b) noexcept {
    return !(a == b);
}

template<typename T>
bool operator<(const vector<T> &a, const vector<T> &b) noexcept {
    size_t cur_len = (a.size() < b.size() ? a.size() : b.size());
    for (size_t i = 0; i != cur_len; i++) {
        if (a[i] != b[i]) {
            return a[i] < b[i];
        }
    }
    return a.size() < b.size();
}

template<typename T>
bool operator>(const vector<T> &a, const vector<T> &b) noexcept {
    return (b < a);
}

template<typename T>
bool operator<=(const vector<T> &a, const vector<T> &b) noexcept {
    return !(a > b);
}

template<typename T>
bool operator>=(const vector<T> &a, const vector<T> &b) noexcept {
    return !(a < b);
}

template<typename T>
typename vector<T>::iterator vector<T>::begin() const noexcept {
    return const_cast<iterator>(data());
}

template<typename T>
typename vector<T>::iterator vector<T>::end() const noexcept {
    return const_cast<iterator>(data() + size_);
}


template<typename T>
typename vector<T>::reverse_iterator vector<T>::rbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

template<typename T>
typename vector<T>::reverse_iterator vector<T>::rend() const noexcept {
    return std::make_reverse_iterator(begin());
}


template<typename T>
void vector<T>::resize(size_t len, const T &value) {
    if (is_big()) {
        unique();
    }
    vector cur(len, value);
    for (size_t i = 0; i != size() && i != cur.size(); i++) {
        cur[i] = operator[](i);
    }
    cur.swap(*this);
}

template<typename T>
void vector<T>::push_back(const T &value) {
    if (is_big()) {
        unique();
    }
    if (size_ == 0) {
        new(&element) T(value);
    } else {
        if (size_ == 1 || size_ == capacity()) {
            auto tmp = value;
            reserve(2 * size_);
            new(tdata() + size_) T(tmp);
        } else {
            new(tdata() + size_) T(value);
        }
    }
    size_++;
}

template<typename T>
size_t vector<T>::size() const noexcept {
    return size_;
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector::const_iterator pos, const T &val) {
    if (size_ == 1) {
        push_back(val);
        return begin();
    } else if (size_ == capacity()) {
        auto dist = pos - begin();
        push_back(val);
        pos = begin() + dist;
    } else {
        push_back(val);
    }
    for (auto it = end() - 1; it != pos; it--) {
        *it = *(it - 1);
    }
    auto cur = const_cast<iterator>(pos);
    *cur = val;
    return cur;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator pos) {
    if (is_big()) {
        auto dist = pos - begin();
        unique();
        pos = begin() + dist;
    }
    auto tmp = const_cast<iterator>(pos);
    for (auto it = tmp; it + 1 != end(); it++) {
        *it = *(it + 1);
    }
    pop_back();
    return tmp;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator first, vector::const_iterator last) {
    if (is_big()) {
        auto dist1 = first - begin(), dist2 = last - begin();
        unique();
        first = begin() + dist1, last = begin() + dist2;
    }
    auto f = const_cast<iterator>(first);
    auto l = const_cast<iterator>(last);
    size_t cnt = 0;
    for (; l != end(); ++f, ++l) {
        *f = *l;
        cnt++;
    }

    auto dist = f - begin();
    resize(size() - (l - f), front());
    return begin() + dist;
}

template<typename T>
char *vector<T>::cdata() const noexcept {
    return big.get();
}

template<typename T>
T *vector<T>::tdata() const noexcept {
    return reinterpret_cast<T *>(cdata() + curadd);
}

template<typename T>
void vector<T>::unique() {
    if (!(*pointer).unique()) {

        size_t cur_len = curadd + sizeof(T) * capacity();
        char *ncdata = new char[cur_len];
        new(ncdata) size_t(capacity());
        T *ntdata = reinterpret_cast<T *>(ncdata + curadd);

        for (size_t i = 0; i != size(); i++) {
            const T &tmp = *(tdata() + i);
            new(ntdata + i) T(tmp);
        }

        (*pointer).~shared_ptr();
        pointer = new(ncdata + sizeof(size_t)) shared_ptr<char>(ncdata, deleter<char>());
    }
}


#endif //VECTOR_H
