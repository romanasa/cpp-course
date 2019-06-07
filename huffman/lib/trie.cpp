#include "trie.h"

trie::trie() {
    nv = 1;
    add_node();
}

void trie::add_node() {
    go.emplace_back(2, -1);
    val.push_back(0);
    term.push_back(false);
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
    term[vert] = true;
}

bool trie::step(uint c) {
    if (c != 0 && c != 1) return false;
    if (!was && go[v][c] != -1) {
        v = go[v][c];
        return true;
    }
    was = true;
    return c == 0;
}

bool trie::end() const {
    return term[v];
}

unsigned char trie::get() const {
    return val[v];
}

void trie::init() {
    v = 0;
}
