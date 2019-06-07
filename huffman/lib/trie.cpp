#include "trie.h"

trie::trie() {
    nv = 1;
    add_node();
}

void trie::add_node() {
    go.emplace_back(2, -1);
    val.push_back(0);
}

trie::trie(const std::vector<std::pair<char, ull>>& codes) {
    nv = 1;
    add_node();

    for (uint i = 0; i < codes.size(); i++) {
        insert(codes[i].first, codes[i].second, i);
    }
}

void trie::insert(unsigned char len, ull code, unsigned char symbol) {
    uint vert = 0;
    for (uint i = 0; i < len; i++) {
        uint cur = (code >> (len - i - 1u)) & 1u;
        if (go[vert][cur] == -1) {
            go[vert][cur] = nv++;
            add_node();
        }
        vert = go[vert][cur];
    }
    val[vert] = symbol;
}

bool trie::step(uint c) {
    uint &cur = go[v][c];
    if (cur != -1) {
        v = cur;
        return true;
    }
    return false;
}

bool trie::end() const {
    return go[v][0] == -1 && go[v][1] == -1;
}

unsigned char trie::get() const {
    return val[v];
}

void trie::init() {
    v = 0;
}