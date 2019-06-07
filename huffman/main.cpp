#include <iostream>
#include <string>
#include <fstream>
#include "huffman.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: <huffman> <-c | -d> <source file> <target file>" << std::endl;
        return 0;
    }

    std::string option = std::string(argv[1]);
    std::string source = std::string(argv[2]);
    std::string target = std::string(argv[3]);

    if (option != "-c" && option != "-d") {
        std::cerr << "invalid option" << std::endl;
        return 0;
    }

    std::ifstream in(source, std::ifstream::binary);
    std::ofstream out(target, std::ofstream::binary);

    if (!in.is_open() || !out.is_open()) {
        std::cerr << "File opening error" << std::endl;
        return 0;
    }
    if (option == "-c") {
        huffman::compress(in, out);
    } else {
        if (!huffman::decompress(in, out)) {
            std::cerr << "Invalid source file" << std::endl;
        }
    }
    return 0;
}