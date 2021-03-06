#ifndef HUFFMAN_TRIE_H
#define HUFFMAN_TRIE_H

#include <cstdint>
typedef uint32_t uint;
typedef uint64_t ull;

#include <vector>


class trie {
public:
    explicit trie(const std::vector<std::pair<char, ull>>& codes);

    bool step(uint c);
    bool end() const;
    unsigned char get() const;
    void init();
    ~trie();

private:
    struct node {
        node *L = nullptr, *R = nullptr;
        unsigned char val = 0;
    };
    node *root = new node();
    node *v = root;
    void insert(unsigned char len, ull code, unsigned char symbol);

    std::vector<node*> verts;
};


#endif //HUFFMAN_TRIE_H
