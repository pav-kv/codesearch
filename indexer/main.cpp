#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <directory>\n";
        return 0;
    }

    std::cout << "Hello. This is the indexer program.\n";

    return 0;
}

