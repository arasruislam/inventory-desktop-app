#include "db.hpp"
#include "gui.hpp"
#include <iostream>

int main() {
    if (!initDB("inventory.db")) {
        std::cerr << "Failed to open database." << std::endl;
        return 1;
    }

    runGUI();

    return 0;
}
