#include <memory>
#include "my_vector.h"
#include <cassert>

bool my_vector::empty() const {
    return len == 0;
}

uint &my_vector::back() {
    if (is_big()) {
        check_unique();
        return big->back();
    }
    return small;
}

uint my_vector::back() const {
    if (is_big()) {
        return big->back();
    }
    return small;
}

uint my_vector::size() const {
    return len;
}

uint my_vector::operator[](size_t ind) const {
    if (is_big()) {
        return big->operator[](ind);
    }
    return small;
}

uint &my_vector::operator[](size_t ind) {
    if (is_big()) {
        check_unique();
        return big->operator[](ind);
    }
    return small;
}

void my_vector::pop_back() {
    if (len > 2) {
        check_unique();
        big->pop_back();
    } else if (len > 1) {
        uint tmp = big->operator[](0);
        big.reset();
        small = tmp;
    } else {
        small = 0;
    }
    len--;
}

void my_vector::push_back(uint val) {
    if (is_big()) {
        check_unique();
        big->push_back(val);
    } else if (len > 0) {
        big = std::make_shared<std::vector<uint>>(1, small);
        //new(&big) std::shared_ptr<std::vector<uint>>(new std::vector<uint>(1, small));
        big->push_back(val);
    } else {
        small = val;
    }
    len++;
}

void my_vector::resize(uint size, uint value) {
    if (is_big()) {
        if (size < 2) {
            uint tmp = operator[](0);
            big.reset();
            small = tmp;
        } else {
            check_unique();
            big->resize(size, value);
        }
    } else if (size > 1) {
        uint tmp = small;
        big = std::make_shared<std::vector<uint>>(size, value);
        //new(&big) std::shared_ptr<std::vector<uint>>(new std::vector<uint>(size, value));
        if (size > 0) {
            big->operator[](0) = tmp;
        }
    }
    len = size;
}

void my_vector::insert_begin(uint cnt) {
    resize(len + cnt);
    if (is_big()) {
        for (size_t i = len - 1; i >= cnt; i--) {
            big->operator[](i) = big->operator[](i - cnt);
        }
        for (size_t i = 0; i < cnt; i++) {
            big->operator[](i) = 0;
        }
    } else {
        small = 0;
    }
}

void my_vector::erase_begin(uint cnt) {
    check_unique();
    for (size_t i = 0; i < len - cnt; i++) {
        big->operator[](i) = big->operator[](i + cnt);
    }
    resize(len - cnt);
}

bool operator==(my_vector const &a, my_vector const &b) {
    if (a.is_big() ^ b.is_big()) {
        return false;
    } else if (a.is_big()) {
        return *a.big == *b.big;
    }
    return a.small == b.small;
}

bool my_vector::is_big() const {
    return len > 1;
}

void my_vector::check_unique() {
    if (!big.unique()) {
        big = std::make_shared<std::vector<uint>>(*big);
    }
}

my_vector::my_vector() {
    small = 0;
    len = 0;
}

my_vector::~my_vector() {
    if (is_big()) {
        big.reset();
    }
}

my_vector &my_vector::operator=(my_vector const &other) {
    if (is_big()) {
        big.reset();
    }
    if (other.is_big()) {
        //new(&big) std::shared_ptr<std::vector<uint>>(other.big);
        big = other.big;
    } else {
        small = other.small;
    }
    len = other.len;
    return *this;
}

uint *my_vector::data() {
    if (is_big()) {
        check_unique();
        return big->data();
    }
    return &small;
}

uint* const my_vector::data() const {
    if (is_big()) {
        return big->data();
    }
    return const_cast<uint*>(&small);
}
