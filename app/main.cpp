#include <exception>
#include <iostream>

#include "app.h"

int main(){
    try {
        run_app();
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }
    std::cout << "Goodbye!\n";
    return 0;
}