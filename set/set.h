//
// Created by roman on 29.06.19.
//

#include <cassert>

namespace {
    template<typename T>
    bool eq(const T &a, const T &b) {
        return !(a < b) && !(b < a);
    }
}

template<typename T>
struct set {
private:
    struct node_base {
        node_base *left = nullptr, *right = nullptr, *parent = nullptr;

        node_base() = default;

        node_base *next();

        node_base *prev();

        T &value() { return static_cast<node *>(this)->value; }

        virtual ~node_base() = default;
    };

    struct node : node_base {
        T value;
        explicit node(const T &value) : node_base(), value(value) {}
    };

public:
    template<typename U>
    struct Iterator {
        using difference_type = ptrdiff_t;
        using value_type = U;
        using pointer = U *;
        using reference = U &;
        using iterator_category = std::bidirectional_iterator_tag;

        friend class set;

        Iterator() = default;

        Iterator &operator++() {
            ptr_ = ptr_->next();
            return *this;
        }

        const Iterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        Iterator &operator--() {
            ptr_ = ptr_->prev();
            return *this;
        }

        const Iterator operator--(int) {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        const T &operator*() const { return ptr_->value(); }

        const T *operator->() const { return &(ptr_->value()); }

        friend bool operator==(Iterator const &a, Iterator const &b) {
            return a.ptr_ == b.ptr_;
        };

        friend bool operator!=(Iterator const &a, Iterator const &b) {
            return !(a == b);
        }

        template<typename V>
        Iterator(const Iterator<V> &other,
                 typename std::enable_if<std::is_same<U, const V>::value>::type * = nullptr) : ptr_(other.ptr_) {}

    private:

        explicit Iterator(node_base *ptr_) : ptr_(ptr_) {}

        node_base *ptr_;
    };

    using value_type = T;
    using iterator = Iterator<const T>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    ~set() {
        clear();
    }

    set() noexcept = default;

    set(const set &other);

    set &operator=(const set &other);

    iterator begin() {
        return iterator(first);
    };

    const_iterator begin() const {
        return const_iterator(first);
    }

    iterator end() {
        return iterator(&fake);
    };

    const_iterator end() const {
        return const_iterator(const_cast<node_base*>(&fake));
    };

    reverse_iterator rbegin() {
        return std::make_reverse_iterator(end());
    };

    const_reverse_iterator rbegin() const {
        return std::make_reverse_iterator(end());
    };

    reverse_iterator rend() {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return std::make_reverse_iterator(begin());
    };

    void swap(set &other);

    std::pair<iterator, bool> insert(const value_type &val);

    iterator erase(const_iterator pos);

    const_iterator find(const value_type &val) const;

    const_iterator lower_bound(const value_type &val) const;

    const_iterator upper_bound(const value_type &val) const;

    bool empty() const noexcept {
        return first == &fake;
    }

    void clear() {
        for (auto it = begin(); it != end(); it = erase(it));
    }



private:
    node_base fake;
    node_base *first = &fake;

    node_base *get_root() const {
        return fake.left;
    }
};

template<typename T>
set<T>::set(const set &other) {
    try {
        for (auto &a : other) {
            insert(a);
        }
    } catch (...) {
        clear();
        throw;
    }
}

template<typename T>
set<T> &set<T>::operator=(const set &other) {
    set(other).swap(*this);
    return *this;
}

template<typename T>
void set<T>::swap(set &other) {

    if (get_root() == nullptr && other.get_root() == nullptr) {
        return;
    }
    if (get_root() != nullptr && other.get_root() != nullptr) {
        std::swap(fake, other.fake);
        std::swap(fake.left->parent, other.fake.left->parent);
        std::swap(first, other.first);
    } else if (get_root() != nullptr) {
        other.fake.left = fake.left;
        other.fake.left->parent = &other.fake;
        other.first = first;

        fake.left = nullptr;
        first = &fake;
    } else {
        fake.left = other.fake.left;
        fake.left->parent = &fake;
        first = other.first;

        other.fake.left = nullptr;
        other.first = &other.fake;
    }
}


template<typename T>
typename set<T>::node_base *set<T>::node_base::next() {
    node_base *cur = this;
    if (cur->right != nullptr) {
        cur = cur->right;
        while (cur->left != nullptr) {
            cur = cur->left;
        }
        return cur;
    }
    while (cur->parent != nullptr && cur == cur->parent->right) {
        cur = cur->parent;
    }
    return cur->parent == nullptr ? nullptr : cur->parent;
}

template<typename T>
typename set<T>::node_base *set<T>::node_base::prev() {
    node_base *cur = this;
    if (cur->left != nullptr) {
        cur = cur->left;
        while (cur->right != nullptr) {
            cur = cur->right;
        }
        return cur;
    }
    while (cur->parent != nullptr && cur == cur->parent->left) {
        cur = cur->parent;
    }
    return cur->parent == nullptr ? nullptr : cur->parent;
}

template<typename T>
typename set<T>::const_iterator set<T>::find(const value_type &val) const {
    auto it = lower_bound(val);
    if (it != end() && *it == val) {
        return it;
    }
    return end();
}

template<typename T>
typename set<T>::const_iterator set<T>::lower_bound(const value_type &val) const {
    auto cur = get_root();
    while (cur != nullptr) {
        if (eq(cur->value(), val)) {
            return const_iterator(cur);
        }
        if (val < cur->value()) {
            if (cur->left == nullptr) {
                return const_iterator(cur);
            }
            cur = cur->left;
        } else {
            if (cur->right == nullptr) {
                return const_iterator(cur->next());
            }
            cur = cur->right;
        }
    }
    return end();
}

template<typename T>
typename set<T>::const_iterator set<T>::upper_bound(const value_type &val) const {
    auto it = lower_bound(val);
    if (it != end() && eq(*it, val)) {
        ++it;
    }
    return it;
}

template<typename T>
std::pair<typename set<T>::iterator, bool> set<T>::insert(const value_type &val) {

    if (get_root() == nullptr) {
        fake.left = static_cast<node_base *>(new node(val));
        fake.left->parent = &fake;
        first = fake.left;
        return {begin(), true};
    }
    if (val < *begin()) {
        node_base *cur = first;
        cur->left = static_cast<node_base *>(new node(val));
        cur->left->parent = cur;
        first = cur->left;
        return {begin(), true};
    }

    node_base *cur = get_root();
    while (true) {
        if (cur->value() == val) {
            return {iterator(cur), false};
        }
        if (val < cur->value()) {
            if (cur->left == nullptr) {
                cur->left = new node(val);
                cur->left->parent = cur;
                return {iterator(cur->left), true};
            }
            cur = cur->left;
        } else {
            if (cur->right == nullptr) {
                cur->right = new node(val);
                cur->right->parent = cur;
                return {iterator(cur->right), true};
            }
            cur = cur->right;
        }
    }
}

template<typename T>
typename set<T>::iterator set<T>::erase(const_iterator pos) {
    if (pos == end()) {
        return end();
    }

    node_base *cur = pos.ptr_;
    node_base *res = cur->next();

    if (!cur->left && !cur->right) {
        if (cur == first) {
            if (cur->parent == &fake) {
                fake.left = nullptr;
                first = &fake;
            } else {
                first = cur->parent;
            }
        }
        if (cur == cur->parent->left) {
            cur->parent->left = nullptr;
        } else {
            cur->parent->right = nullptr;
        }
    } else if (!cur->left) {
        if (cur == cur->parent->left) {
            if (cur == first) {
                first = cur->right;
                while (first->left != nullptr) {
                    first = first->left;
                }
            }
            cur->parent->left = cur->right;
        } else {
            cur->parent->right = cur->right;
        }
        cur->right->parent = cur->parent;
    } else if (!cur->right) {
        if (cur == cur->parent->left) {
            cur->parent->left = cur->left;
        } else {
            cur->parent->right = cur->left;
        }
        cur->left->parent = cur->parent;
    } else {

        if (res != cur->right) {
            res->parent->left = res->right;
            if (res->right) {
                res->right->parent = res->parent;
            }
            res->right = cur->right;
            res->right->parent = res;
        }
        if (cur == cur->parent->left) {
            cur->parent->left = res;
        } else {
            cur->parent->right = res;
        }
        res->parent = cur->parent;
        res->left = cur->left;
        res->left->parent = res;
    }

    if (cur == first) {
        first = (cur->right ? cur->right : &fake);
    }

    delete static_cast<node*>(cur);
    return iterator(res);
}

template<typename T>
void swap(set<T> &a, set<T> &b) {
    a.swap(b);
}

