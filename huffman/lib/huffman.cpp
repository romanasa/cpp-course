#include "sstream"
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <cstring>
#include "huffman.h"
#include "trie.h"

const uint huffman::buff_size = 4096;
const uint huffman::len = 256;

void huffman::compress(std::istream &in, std::ostream &out) {
    in.seekg(std::istream::beg);

    unsigned char buffer[buff_size];
    unsigned char out_buffer[buff_size];
    std::vector<ull> cnt(len);

    uint cur_size;
    while ((cur_size = in.readsome((char *) buffer, buff_size)) > 0) {
        for (size_t j = 0; j < cur_size; j++) {
            cnt[buffer[j]]++;
        }
    }

    uint buff_ind = 0;
    ull len_stream = 0;
    out.write((char *) &len_stream, sizeof(len_stream));

    auto write = [&](auto x) {
        if (buff_ind + sizeof(x) >= buff_size) {
            out.write((char *) out_buffer, buff_ind);
            buff_ind = 0;
        }
        memcpy(out_buffer + buff_ind, &x, sizeof(x));
        buff_ind += sizeof(x);
        len_stream += 8 * sizeof(x);
    };

    for (uint i = 0; i < len; i++) {
        write(static_cast<unsigned char>(i));
        write(cnt[i]);
    }
    len_stream = 0;

    auto codes = code(cnt);
    in.seekg(std::istream::beg);

    ull cur_out = 0;
    uint cur_cnt = 0;

    while ((cur_size = in.readsome((char *) buffer, buff_size)) > 0) {
        for (size_t j = 0; j < cur_size; j++) {
            auto crev = rev(codes[buffer[j]].second, codes[buffer[j]].first);
            cur_out |= crev << cur_cnt;
            cur_cnt += codes[buffer[j]].first;
            if (cur_cnt >= 64) {
                write(cur_out);
                cur_cnt -= 64;
                cur_out = crev >> (codes[buffer[j]].first - cur_cnt);
            }
        }
    }
    if (cur_cnt) {
        write(cur_out);
        len_stream -= (64 - cur_cnt);
    }
    if (buff_ind) {
        out.write((char *) out_buffer, buff_ind);
    }
    out.seekp(0);
    out.write((char *) &len_stream, sizeof(len_stream));
}

bool huffman::decompress(std::istream &in, std::ostream &out) {
    in.seekg(std::istream::beg);

    ull len_stream = 0;
    in.read((char *) &len_stream, sizeof(len_stream));

    std::vector<ull> cnt(len);
    for (uint i = 0; i < len; i++) {
        unsigned char num;
        in.read((char *) &num, sizeof(num));
        if (num != i) {
            return false;
        }
        ull val;
        in.read((char *) &val, sizeof(val));
        cnt[num] = val;
    }

    auto codes = code(cnt);
    trie code_trie(codes);

    unsigned char buffer[buff_size];
    unsigned char out_buffer[buff_size];

    auto buff_ind = 0;
    auto write = [&](auto x) {
        if (buff_ind + sizeof(x) >= buff_size) {
            out.write((char *) out_buffer, buff_ind);
            buff_ind = 0;
        }
        memcpy(out_buffer + buff_ind, &x, sizeof(x));
        buff_ind += sizeof(x);
    };

    uint cur_size;
    while ((cur_size = in.readsome((char *) buffer, buff_size)) > 0) {
        for (size_t j = 0; j < cur_size; j++) {
            for (uint bit = 0; bit < std::min<ull>(8, len_stream); bit++) {
                char c = (buffer[j] >> bit) & 1u;
                if (code_trie.step(c)) {
                    if (code_trie.end()) {
                        write(code_trie.get());
                        code_trie.init();
                    }
                } else {
                    return false;
                }
            }
            len_stream = (len_stream >= 8 ? len_stream - 8 : 0);
        }
    }

    if (buff_ind) {
        out.write((char *) out_buffer, buff_ind);
    }
    return true;
}

std::vector<std::pair<char, ull>> huffman::code(const std::vector<ull> &cnt) {
    uint cur_len = cnt.size();
    std::vector<std::pair<char, ull>> res(cur_len);
    std::vector<std::pair<uint, char>> par(2 * cur_len, std::make_pair(-1, -1));

    std::priority_queue<std::pair<ull, int>, std::vector<std::pair<ull, int>>, std::greater<>> counts;
    for (int i = 0; i < cur_len; i++) {
        counts.push({cnt[i], i});
    }

    int ind = cur_len;
    while (counts.size() > 1) {
        auto first = counts.top();
        counts.pop();
        auto second = counts.top();
        counts.pop();

        par[first.second] = {ind, 0};
        par[second.second] = {ind, 1};
        counts.push({first.first + second.first, ind++});
    }

    for (int i = 0; i < cur_len; i++) {
        ull cur_code = 0;
        std::vector<char> codes;
        for (int j = i; par[j].first != -1; j = par[j].first) {
            codes.push_back(par[j].second);
        }
        reverse(codes.begin(), codes.end());
        for (int code : codes) {
            cur_code = cur_code * 2 + code;
        }
        res[i] = {(char) codes.size(), cur_code};
    }
    return res;
}

ull huffman::rev(ull num, char bits) {
    ull res = 0;
    for (uint i = 0; i < bits; i++) {
        uint bit = (num >> i) & 1u;
        res |= (bit << (bits - i - 1));
    }
    return res;
}
