#include <iostream>
#include <random>
#include "huffman.h"
#include "gtest/gtest.h"

TEST(correctness, empty) {
    std::stringstream in(""), code, out;

    huffman::compress(in, code);
    huffman::decompress(code, out);
    EXPECT_EQ(in.str(), out.str());
}

TEST(correctness, one_char) {
    std::stringstream in("a"), code, out;

    huffman::compress(in, code);
    huffman::decompress(code, out);
    EXPECT_EQ(in.str(), out.str());
}


TEST(correctness, alphabet) {
    std::stringstream in("abcdefghijklmnopqrstuvwxyz"), code, out;

    huffman::compress(in, code);
    huffman::decompress(code, out);
    EXPECT_EQ(in.str(), out.str());
}

TEST(correctness, all_chars) {
    std::stringstream in, code, out;
    for (int i = -128; i < 128; i++) {
        in << (char)i;
    }
    huffman::compress(in, code);
    huffman::decompress(code, out);
    EXPECT_EQ(in.str(), out.str());
}

TEST(correctness, big_text) {
    std::stringstream in, code, out;

    std::mt19937 rnd(1);
    for (int i = 0; i < 10 * 1024 * 1024; i++) {
        in << (char)rnd() % 256;
    }
    huffman::compress(in, code);
    huffman::decompress(code, out);
    EXPECT_EQ(in.str(), out.str());
}

TEST(corectness, invalid_file) {
    std::stringstream code("dafhk41 41"), out;
    EXPECT_FALSE(huffman::decompress(code, out));
}


TEST(correctness, one_char_text) {
    std::stringstream in, code, out;

    std::mt19937 rnd(1);
    for (int i = 0; i < 10 * 1024 * 1024; i++) {
        in << 'a';
    }
    huffman::compress(in, code);
    huffman::decompress(code, out);
    EXPECT_EQ(in.str(), out.str());
}


TEST(correctness, two_chars_text) {
    std::stringstream in, code, out;

    std::mt19937 rnd(1);
    for (int i = 0; i < 10 * 1024 * 1024; i++) {
        in << (char)('a' + rnd() % 2);
    }
    huffman::compress(in, code);
    huffman::decompress(code, out);
    EXPECT_EQ(in.str(), out.str());
}


TEST(correctness, compress_compressed) {
    std::stringstream in, code1, code2, out2, out;

    std::mt19937 rnd(1);
    for (int i = 0; i < 10 * 1024 * 1024; i++) {
        in << (char)(rnd() % 256);
    }
    huffman::compress(in, code1);
    huffman::compress(code1, code2);

    huffman::decompress(code2, out2);
    huffman::decompress(out2, out);
    EXPECT_EQ(in.str(), out.str());
}