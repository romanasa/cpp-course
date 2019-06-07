#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

typedef uint32_t uint;
typedef uint64_t ull;

#include <vector>

class huffman {
public:
    huffman() = default;

    static bool decompress(std::istream &out, std::ostream &in);

    static void compress(std::istream &in, std::ostream &out);

    static std::vector<std::pair<char, ull>> code(const std::vector<ull> &cnt);

    static const uint buff_size;
    static const uint len;


    static ull rev(ull num, char bits);
};


#endif //HUFFMAN_HUFFMAN_H
