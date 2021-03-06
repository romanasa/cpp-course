#include "trie.h"


trie::trie(const std::vector<std::pair<char, ull>>& codes) {
    for (uint i = 0; i < codes.size(); i++) {
        insert(codes[i].first, codes[i].second, i);
    }
}

void trie::insert(unsigned char len, ull code, unsigned char symbol) {
    node *vert = root;
    for (uint i = 0; i < len; i++) {
        uint cur = (code >> (len - i - 1u)) & 1u;
        node *son = (cur ? vert->R : vert->L);
        if (!son) {
            verts.push_back(new node());
            son = (cur ? vert->R : vert->L) = verts.back();
        }
        vert = son;
    }
    vert->val = symbol;
}

bool trie::step(uint c) {
    v = (c ? v->R : v->L);
    return (v != nullptr);
}

bool trie::end() const {
    return v->L == nullptr;
}

unsigned char trie::get() const {
    return v->val;
}

void trie::init() {
    v = root;
}

trie::~trie() {
    delete root;
    for (auto c : verts) {
        delete c;
    }
}
