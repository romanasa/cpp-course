#include "big_integer.h"

#include <cstring>
#include <vector>
#include <stdexcept>

const uint big_integer::log_base = 32;
const uint big_integer::max_number = (uint) 1e9;
const ull big_integer::base = (ull) 1 << log_base;

bool big_integer::negative() const {
    return !data.empty() && (data.back() & ((uint) 1 << (log_base - 1)));
}

bool big_integer::negative_last() const {
    return data.size() > 1 && (data[data.size() - 2] & ((uint) 1 << (log_base - 1)));
}

uint big_integer::null_value() const {
    return negative() ? base - 1 : 0;
}

void big_integer::shrink() {
    while (!data.empty() && data.back() == null_value() && negative() == negative_last()) {
        data.pop_back();
    }
}

big_integer::big_integer() = default;

big_integer::big_integer(big_integer const &other) {
    data = other.data;
}

big_integer::big_integer(int a) {
    if (a) {
        data.push_back((uint) a);
    }
}

big_integer::big_integer(uint a) {
    if (a) {
        data.push_back((uint) a);
    }
    if (negative()) {
        data.push_back(0);
    }
}

big_integer::big_integer(std::string const &str) {
    bool neg = (str[0] == '-');
    for (char c : str) {
        if (c != '-') {
            *this = *this * 10 + (c - '0');
        }
    }
    if (neg) {
        *this = -*this;
    }
}

big_integer::~big_integer() = default;

big_integer &big_integer::operator=(big_integer const &other) = default;

big_integer &big_integer::operator+=(big_integer const &rhs) {
    size_t len = std::max(size(), rhs.size()) + 1;
    data.resize(len, null_value());

    bool carry = false;
    for (size_t i = 0; i < len; i++) {
        uint digit = i < rhs.size() ? rhs.data[i] : rhs.null_value();
        bool ncarry = ((ull) data[i] + digit + carry) >= base;
        data[i] += digit + carry;
        carry = ncarry;
    }
    shrink();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return operator+=(-rhs);
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer res;
    big_integer a = *this, b = rhs;
    if (a.negative()) {
        a = -a;
    }
    if (b.negative()) {
        b = -b;
    }
    for (size_t i = 0; i < b.size(); i++) {
        res += (a * b.data[i]) << (i * log_base);
    }
    if (negative() ^ rhs.negative()) {
        res = -res;
    }
    res.shrink();
    return *this = res;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    big_integer res;
    big_integer a = *this, b = rhs;
    if (negative()) {
        a = -a;
    }
    if (rhs.negative()) {
        b = -b;
    }
    if (a < b) {
        return *this = 0;
    }
    if (b.size() == 1) {
        res = my_div(a, b.data[0]).first;
    } else {
        size_t m = b.size();
        while (b.data[m - 1] == 0) {
            m--;
        }
        uint f = (uint) ((big_integer::base + 1) / (b.data[m - 1] + 1));
        a *= f, b *= f;

        size_t n = a.size();
        m = b.size();

        while (b.data[m - 1] == 0) {
            m--;
        }

        uint divisor = b.data[m - 1];

        big_integer cur = a >> log_base * (n - m + 1), tmp;
        res.data.resize(n - m + 1);

        for (size_t i = n - m + 1; i > 0; i--) {
            cur = (cur << log_base) + a.data[i - 1];
            uint tq = trial((m - 0 < cur.size() ? cur.data[m - 0] : (uint) 0),
                            (m - 1 < cur.size() ? cur.data[m - 1] : (uint) 0), divisor);
            tmp = b * tq;
            while (tq > 0 && cur < tmp) {
                tmp = b * (--tq);
            }
            cur -= tmp;
            res.data[i - 1] = tq;
        }
    }
    if (negative() ^ rhs.negative()) {
        res = -res;
    }
    res.shrink();
    return *this = res;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this -= (*this / rhs) * rhs;
}

big_integer &big_integer::apply_opperation(big_integer const &rhs, const std::function<uint(uint, uint)> &f) {
    size_t len = std::max(size(), rhs.size());
    data.resize(len, null_value());
    for (size_t i = 0; i < len; i++) {
        data[i] = f(data[i], i < rhs.size() ? rhs.data[i] : rhs.null_value());
    }
    shrink();
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return apply_opperation(rhs, [](uint a, uint b) { return a & b; });
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return apply_opperation(rhs, [](uint a, uint b) { return a | b; });
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return apply_opperation(rhs, [](uint a, uint b) { return a ^ b; });
}

void big_integer::shift(int rhs) {
    if (rhs > 0) {
        data.insert_begin(rhs);
    } else {
        data.erase_begin(-rhs);
    }
}

big_integer &big_integer::operator<<=(int rhs) {
    if (rhs < 0) {
        return *this >>= -rhs;
    }
    int blocks = rhs / log_base;
    if (blocks) {
        shift(blocks);
    }
    uint shift = rhs - blocks * log_base;
    if (shift) {
        data.push_back(null_value());
        for (size_t i = size(); i > 0; i--) {
            if (i < size()) {
                data[i] += data[i - 1] >> (log_base - shift);
            }
            data[i - 1] <<= shift;
        }
    }
    shrink();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (rhs < 0) {
        return *this <<= -rhs;
    }
    int blocks = rhs / log_base;
    if (blocks) {
        shift(-blocks);
    }
    uint shift = rhs - blocks * log_base;
    if (shift) {
        uint cur = null_value();
        for (size_t i = 0; i < size(); i++) {
            if (i > 0) {
                data[i - 1] += data[i] << (log_base - shift);
            }
            data[i] >>= shift;
        }
        data.back() += cur << (log_base - shift);
    }
    shrink();
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    return ~*this + 1;
}

big_integer big_integer::operator~() const {
    big_integer r = *this;
    if (r.data.empty()) {
        r.data.push_back(0);
    }
    for (size_t i = 0; i < r.data.size(); i++) {
        r.data[i] = ~r.data[i];
    }
    r.shrink();
    return r;
}

big_integer &big_integer::operator++() {
    return *this += 1;
}

const big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    return *this -= 1;
}

const big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return a.data == b.data;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.negative() != b.negative()) {
        return a.negative();
    }
    if (a.size() != b.size()) {
        return (a.size() < b.size()) ^ a.negative();
    }
    for (size_t i = a.size(); i > 0; i--) {
        if (a.data[i - 1] != b.data[i - 1]) {
            return a.data[i - 1] < b.data[i - 1];
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

big_integer operator*(big_integer const &a, uint const &b) {
    big_integer res = a;
    bool neg = res.negative();
    if (neg) {
        res = -res;
    }
    res.data.push_back(0);
    uint carry = 0;
    for (size_t i = 0; i < res.size(); i++) {
        ull cur = (ull) res.data[i] * b + carry;
        res.data[i] = (uint) (cur & (big_integer::base - 1));
        carry = (uint) (cur >> big_integer::log_base);
    }
    if (neg) {
        res = -res;
    }
    res.shrink();
    return res;
}

std::pair<big_integer, uint> my_div(big_integer const &a, uint b) {
    big_integer res = a;
    uint mod = 0;
    for (size_t i = res.size(); i > 0; i--) {
        ull cur = big_integer::base * mod + res.data[i - 1];
        res.data[i - 1] = cur / b;
        mod = cur % b;
    }
    res.shrink();
    return {res, mod};
}

std::string to_string(big_integer const &a) {
    std::string res;
    big_integer tmp = a;
    if (tmp.negative()) {
        res += "-";
        tmp = -tmp;
    }

    std::vector<std::string> out(a.size() * 2);
    size_t ind = 0;

    while (tmp > 0) {
        auto c = my_div(tmp, big_integer::max_number);
        out[ind++] = std::to_string(c.second);
        tmp = c.first;
    }
    for (size_t i = ind; i > 0; i--) {
        if (i != ind) {
            res += std::string(9 - out[i - 1].size(), '0');
        }
        res += out[i - 1];
    }
    if (res.empty()) {
        res += "0";
    }
    return res;
}

size_t big_integer::size() const {
    return data.size();
}

uint big_integer::trial(uint const &a, uint const &b, uint const &c) const {
    ull res = (((ull) a << big_integer::log_base) + b) / c;
    if (res > big_integer::base - 1) {
        res = big_integer::base - 1;
    }
    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}
