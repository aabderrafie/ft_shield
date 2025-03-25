
#include "shield.hpp"

int main() {
    try {
        shield sd;
        sd.start();
     
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}