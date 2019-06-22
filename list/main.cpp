#include <iostream>
#include "list.h"

int main() {
    list<int> a;
    a.push_back(3);
    std::cout << a.back() << std::endl;
    return 0;
}