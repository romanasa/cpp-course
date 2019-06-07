#ifndef HUFFMAN_TRIE_H
#define HUFFMAN_TRIE_H

#include <cstdint>
typedef uint32_t uint;
typedef uint64_t ull;

#include <vector>

class trie {
public:
    trie();
    explicit trie(const std::vector<std::pair<char, ull>>& codes);

    int nv;
    bool step(uint c);
    bool end() const;
    unsigned char get() const;
    void init();
private:
    uint v = 0;
    std::vector<unsigned char> val;
    std::vector<std::vector<uint>> go;
    void insert(unsigned char len, ull code, unsigned char symbol);
    void add_node();
};


#endif //HUFFMAN_TRIE_H
