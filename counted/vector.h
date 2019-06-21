#ifndef VECTOR_H
#define VECTOR_H

#include <cstdio>
#include <iterator>
#include <variant>
#include <cstring>
#include <memory>
#include <algorithm>

using std::shared_ptr;

#define align(x) (((x) + sizeof(size_t) - 1) / sizeof(size_t) * sizeof(size_t))

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

    vector();

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
    friend void swap(vector<Y> &a, vector<Y> &b);

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

    bool is_big() const noexcept;

    void erase_begin(uint cnt);

    void insert_begin(uint cnt);

private:
    static const size_t curadd = align(sizeof(size_t) + sizeof(shared_ptr<T>));
    static const size_t cursz = sizeof(size_t) + sizeof(bool);

    //size_t size_ = 0;

    char *psize_ = nullptr;

    union {
        T element;
        shared_ptr<char> *pointer;
    };


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
vector<T>::vector(vector const &other): pointer(nullptr) {
    if (other.empty()) {
        return;
    }
    if (other.is_big()) {
        char *cdata = new char[curadd + cursz];
        new(cdata) size_t(other.size());
        pointer = new(cdata + sizeof(size_t)) shared_ptr<char>(*other.pointer);
        psize_ = cdata + curadd;
    } else {
        psize_ = new char[cursz];
        if (other.size()) {
            try {
                new(&element) T(other.element);
            } catch (std::exception const &e) {
                delete[] psize_;
                throw e;
            }
        }
    }
    new(psize_) size_t(other.size());
    new(psize_ + sizeof(size_t)) bool(other.is_big());
}

template<typename T>
vector<T>::vector(): pointer(nullptr) {}

template<typename T>
vector<T>::vector(vector &&other) noexcept: pointer(nullptr) {
    swap(other);
}

template<typename T>
vector<T>::~vector() {
    if (is_big()) {
        if (big.use_count() == 1) {
            for (size_t i = 0; i < size(); i++) {
                tdata()[i].~T();
            }
            big.~shared_ptr();
        } else {
            big.~shared_ptr();
            delete[] (reinterpret_cast<char *>(pointer) - sizeof(size_t));
        }
    } else if (size()) {
        small.~T();
        delete[] psize_;
    }
}


template<typename T>
template<typename InputIterator>
vector<T>::vector(InputIterator first, InputIterator last): vector() {
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
            auto tmp = other.pointer;
            new(&other.element) T(element);
            element.~T();
            pointer = tmp;
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
    std::swap(psize_, other.psize_);
}

template<typename T>
bool vector<T>::is_big() const noexcept {
    return psize_ ? *reinterpret_cast<bool *>(psize_ + sizeof(size_t)) : false;
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
    new(psize_) size_t(0);
}

template<typename T>
void vector<T>::shrink_to_fit() {
    reserve(size());
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
        return operator[](size() - 1);
    }
    return small;
}

template<typename T>
const T &vector<T>::back() const noexcept {
    if (is_big()) {
        return operator[](size() - 1);
    }
    return small;
}

template<typename T>
void vector<T>::pop_back() {
    if (is_big()) {
        unique();
        tdata()[size() - 1].~T();
    } else {
        element.~T();
        delete[] psize_;
    }

    auto nsize = size() - 1;
    new(psize_) size_t(nsize);
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
    return size() == 0;
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
    return const_cast<iterator>(data() + size());
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

    if (len <= size()) {
        while (size() > len) {
            pop_back();
        }
        return;
    }
    if (len <= capacity()) {
        while (size() < len) {
            push_back(value);
        }
        return;
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
        if (size() == capacity()) {
            auto tmp = value;
            reserve(2 * size());
            try {
                new(tdata() + size()) T(tmp);
            } catch (std::exception const &e) {
                throw e;
            }
        } else {
            new(tdata() + size()) T(value);
        }
    } else {
        if (size() == 0) {
            psize_ = new char[cursz];
            new(psize_) size_t(0);
            new(psize_ + sizeof(size_t)) bool(false);
            try {
                new(&element) T(value);
            } catch (std::exception const &e) {
                delete[] psize_;
                psize_ = nullptr;
                throw e;
            }
        } else {
            auto tmp = value;
            reserve(2 * size());
            try {
                new(tdata() + size()) T(tmp);
            } catch (std::exception const &e) {
                throw e;
            }
        }
    }

    auto nsize = size() + 1;
    new(psize_) size_t(nsize);
}

template<typename T>
size_t vector<T>::size() const noexcept {
    return psize_ ? *reinterpret_cast<size_t *>(psize_) : 0;
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector::const_iterator pos, const T &val) {
    if (size() == 1) {
        push_back(val);
        return begin();
    } else if (size() == capacity()) {
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

    if (l == end()) {
        auto tmp(*this);
        while (tmp.size() > (size_t)(f - begin())) {
            tmp.pop_back();
        }
        *this = std::move(tmp);
        return f;
    }

    for (; l != end(); ++f, ++l) {
        try {
            *f = *l;
        } catch (std::exception const &e) {
            throw e;
        }
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
void vector<T>::reserve(size_t len) {
    if (len > 1) {
        if (is_big()) {
            unique();
        }
        bool need = is_big();
        size_t cur_size = size(), new_len = std::max(len, cur_size);

        char *cur_cdata = new char[align(curadd + sizeof(T) * new_len) + cursz];
        T *cur_tdata = reinterpret_cast<T *>(cur_cdata + curadd);

        new(cur_cdata) size_t(new_len);
        shared_ptr<char> *cur_pointer = nullptr;
        try {
            cur_pointer = new(cur_cdata + sizeof(size_t)) shared_ptr<char>(cur_cdata, deleter<char>());
        } catch (std::exception const &e) {
            //delete[] cur_cdata;  // clear in shared_prt destructor
            throw e;
        }

        for (size_t i = 0; i != cur_size; i++) {
            try {
                new(cur_tdata + i) T(operator[](i));
            } catch (std::exception const &e) {
                for (size_t j = 0; j != i; j++) {
                    cur_tdata[j].~T();
                }
                cur_pointer->~shared_ptr();
                throw e;
            }
        }

        if (need) {
            try {
                vector().swap(*this);
            } catch (std::exception const &e) {
                for (size_t j = 0; j != cur_size; j++) {
                    cur_tdata[j].~T();
                }
                delete[] cur_cdata;
                throw e;
            }
        } else {
            if (size()) {
                small.~T();
            }
        }
        pointer = cur_pointer;

        delete[] psize_;

        psize_ = cur_cdata + align(curadd + sizeof(T) * new_len);
        new(psize_) size_t(cur_size);
        new(psize_ + sizeof(size_t)) bool(true);
    }
}


template<typename T>
void vector<T>::unique() {
    if (!(*pointer).unique()) {
        size_t cur_len = align(curadd + sizeof(T) * capacity());
        size_t old_len = size();

        char *ncdata = new char[cur_len + cursz];
        T *ntdata = reinterpret_cast<T *>(ncdata + curadd);

        new(ncdata) size_t(capacity());

        shared_ptr<char> *cur_pointer = nullptr;
        try {
            cur_pointer = new(ncdata + sizeof(size_t)) shared_ptr<char>(ncdata, deleter<char>());
        } catch (std::exception const &e) {
            // delete[] ncdata;
            throw e;
        }

        for (size_t i = 0; i != size(); i++) {
            const T &tmp = *(tdata() + i);
            try {
                new(ntdata + i) T(tmp);
            } catch (std::exception const &e) {
                for (size_t j = 0; j != i; j++) {
                    ntdata[j].~T();
                }
                cur_pointer->~shared_ptr();
                throw e;
            }
        }

        (*pointer).~shared_ptr();
        delete[] (reinterpret_cast<char *>(pointer) - sizeof(size_t));
        pointer = cur_pointer;

        psize_ = ncdata + cur_len;
        new(psize_) size_t(old_len);
        new(psize_ + sizeof(size_t)) bool(true);
    }
}


template<typename T>
vector<T>::vector(size_t size, const T &value): pointer(nullptr) {
    if (size == 1) {
        psize_ = new char[cursz];
        try {
            new(&element) T(value);
        } catch (std::exception const &e) {
            delete[] psize_;
            throw e;
        }
    } else if (size > 1) {
        char *cdata = new char[align(curadd + size * sizeof(T)) + cursz];
        T *tdata = (T *) (cdata + curadd);

        try {
            pointer = new(cdata + sizeof(size_t)) shared_ptr<char>(cdata, deleter<char>());
        } catch (std::exception const &e) {
            //delete[] cdata;
            throw e;
        }

        for (size_t i = 0; i != size; i++) {
            try {
                new(tdata + i) T(value);
            } catch (std::exception const &e) {
                for (size_t j = 0; j != i; j++) {
                    tdata[j].~T();
                }
                pointer->~shared_ptr();
                pointer = nullptr;
                throw e;
            }
        }
        new(cdata) size_t(size);
        psize_ = cdata + align(curadd + size * sizeof(T));
    }
    new(psize_) size_t(size);
    new(psize_ + sizeof(size_t)) bool(size > 1);
}


template<typename T>
void vector<T>::erase_begin(uint cnt) {
    unique();
    for (size_t i = 0; i < size() - cnt; i++) {
        operator[](i) = operator[](i + cnt);
    }
    while (cnt--) {
        pop_back();
    }
}

template<typename T>
void vector<T>::insert_begin(uint cnt) {
    resize(size() + cnt, front());
    if (is_big()) {
        for (size_t i = size() - 1; i >= cnt; i--) {
            operator[](i) = operator[](i - cnt);
        }
        for (size_t i = 0; i < cnt; i++) {
            operator[](i) = 0;
        }
    } else {
        small = 0;
    }
}

#endif //VECTOR_H
