#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

typedef unsigned int uint;

#include <memory>
#include <vector>

class my_vector {
public:
    my_vector();

    ~my_vector();

    bool empty() const;

    uint &back();

    uint back() const;

    uint size() const;

    uint operator[] (size_t ind) const;

    uint &operator[] (size_t ind);

    void pop_back();

    void push_back(uint val);

    void resize(uint size, uint value = 0);

    void insert_begin(uint cnt);

    void erase_begin(uint i);

    friend bool operator== (my_vector const &a, my_vector const &b);

    my_vector &operator=(my_vector const &other);
private:
    union {
        std::shared_ptr<std::vector<uint>> big;
        uint small;
    };

    size_t len;

    bool is_big() const;

    void check_one();
};

bool operator== (my_vector const &a, my_vector const&b);

#endif //BIGINT_MY_VECTOR_H
