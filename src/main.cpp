#include "explorer.hpp"
#include <iostream>

int main() {
    std::cout << "Console File Explorer (C++ / Linux)\nType 'help' for commands.\n";
    Explorer e;
    e.run();
    std::cout << "Goodbye.\n";
    return 0;
}
