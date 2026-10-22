#include "app.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Characteristic_hard.h"
#include "Characteristic_simple.h"

namespace {
    class InputClosed : public std::runtime_error{
        public:
            InputClosed() : std::runtime_error("Input closed") {}
    };
    
    
    void skip_line(){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    int read_int(const std::string& prompt){
        while (true){
            std::cout << prompt;
            int number = 0;
            if (std::cin >> number){
                skip_line();
                return number;
            }
            if (std::cin.eof() || std::cin.bad()) throw InputClosed();
            std::cout << "Enter an integer\n";
            skip_line();
        }
    }
    
    std::string read_string(const std::string& prompt){
        std::cout << prompt;
        std::string word;
        if (!(std::cin >> word)) throw InputClosed();
        skip_line();
        return word;
    }
    
    std::vector<std::string> read_strings(const std::string& prompt){
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) throw InputClosed();
    
        std::istringstream stream(line);
        std::vector<std::string> words;
        for (std::string word; stream >> word;) words.push_back(word);
        return words;
    }
    
    template <typename T>
    void read_object(T& object){
        if (std::cin >> object){
            skip_line();
            return;
        }
        if (std::cin.eof() || std::cin.bad()) throw InputClosed();
        std::cout << "Wrong format, nothing changed\n";
        skip_line();
    }
    
    checkmode read_mode(){
        while (true){
            switch (read_int("Mode (1 - normal, 2 - advantage, 3 - disadvantage): ")){
                case 1: return checkmode::normal;
                case 2: return checkmode::advantage;
                case 3: return checkmode::disadvantage;
                default: std::cout << "No such mode\n";
            }
        }
    }
    
    void print_check_result(bool passed){
        std::cout << (passed ? "Check passed\n" : "Check failed\n");
    }
    
    void print_menu_choice(){
        std::cout << "\nChoose your characteristic\n"
            "0) Exit\n"
            "1) Characteristic_simple\n"
            "2) Characteristic_hard" << std::endl;
    }
    
    void print_menu_simple(){
        std::cout << "\n-------Characteristic_simple-------\n"
            "1) Output characteristic (<<)\n"
            "2) Create by name\n"
            "3) Create by name and value\n"
            "4) Change name\n"
            "5) Change value\n"
            "6) Increase value (+=)\n"
            "7) Check difficulty\n"
            "8) Check difficulty with advantage\n"
            "9) Check difficulty with disadvantage\n"
            "10) Input characteristic (>>)\n"
            "0) Back" << std::endl;
    }
    
    void print_menu_hard(){
        std::cout << "\n-------Characteristic_hard-------\n"
            "1) Output table (<<)\n"
            "2) Create from list of names\n"
            "3) Create from one characteristic\n"
            "4) Add new characteristic (+=)\n"
            "5) Add characteristic from Characteristic_simple menu (+=)\n"
            "6) Get characteristic by name ([])\n"
            "7) Increase characteristic in table ([] and +=)\n"
            "8) Check characteristic (())\n"
            "9) Sort by name\n"
            "10) Find max among given names\n"
            "11) Input table (>>)\n"
            "0) Back" << std::endl;
    }
    
    void simple_menu(Characteristic_simple& c){
        while (true){
            print_menu_simple();
            const int choice = read_int("> ");
            if (choice == 0) return;
        
            try {
                switch (choice){
                    case 1:
                        std::cout << c << std::endl;
                        break;
                    case 2:
                        c = Characteristic_simple(read_string("Name: "));
                        break;
                    case 3: {
                        const std::string name = read_string("Name: ");
                        const int value = read_int("Value (1..20): ");
                        c = Characteristic_simple(name, value);
                        break;
                    }
                    case 4:
                        c.set_name(read_string("New name: "));
                        break;
                    case 5:
                        c.set_value(read_int("New value (1..20): "));
                        break;
                    case 6:
                        c += read_int("Add (1..20): ");
                        break;
                    case 7: {
                        const int cube = read_int("Cube (1..20): ");
                        const int difficulty = read_int("Difficulty (>= 1): ");
                        print_check_result(c.check_difficulty(cube, difficulty));
                        break;
                    }
                    case 8:
                    case 9: {
                        const int first = read_int("First cube (1..20): ");
                        const int second = read_int("Second cube (1..20): ");
                        const int difficulty = read_int("Difficulty (>= 1): ");
                        print_check_result(choice == 8
                            ? c.check_difficulty_advantage(first, second, difficulty)
                            : c.check_difficulty_disadvantage(first, second, difficulty));
                        break;
                    }
                    case 10:
                        std::cout << "Enter <name> <value>: ";
                        read_object(c);
                        break;
                    default:
                        std::cout << "No such item\n";
                }
            } catch (const std::logic_error& error) {
                std::cout << "Error: " << error.what() << std::endl;
            }
        }
    }
    
    void hard_menu(Characteristic_hard& table, const Characteristic_simple& simple){
        while (true){
            print_menu_hard();
            const int choice = read_int("> ");
            if (choice == 0) return;
        
            try {
                switch (choice){
                    case 1:
                        std::cout << table << '\n';
                        break;
                    case 2:
                        table = Characteristic_hard(read_strings("Names separated by spaces: "));
                        break;
                    case 3:
                    case 4: {
                        const std::string name = read_string("Name: ");
                        const int value = read_int("Value (1..20): ");
                        if (choice == 3) table = Characteristic_hard(name, value);
                        else table += Characteristic_simple(name, value);
                        break;
                    }
                    case 5:
                        table += simple;
                        break;
                    case 6: {
                        const Characteristic_simple& found = table[read_string("Name: ")];
                        std::cout << found << std::endl;
                        break;
                    }
                    case 7: {
                        Characteristic_simple& found = table[read_string("Name: ")];
                        found += read_int("Add (1..20): ");
                        std::cout << found << std::endl;
                        break;
                    }
                    case 8: {
                        const std::string name = read_string("Name: ");
                        const checkmode mode = read_mode();
                        const int difficulty = read_int("Difficulty (>= 1): ");
                        print_check_result(table(name, mode, difficulty));
                        break;
                    }
                    case 9:
                        table.sort_by_name(read_int("0 - ascending, 1 - descending: ") == 1);
                        break;
                    case 10: {
                        const std::vector<std::string> names = read_strings("Names separated by spaces: ");
                        std::cout << "Max: " << table.search_max_in_array(names) << std::endl;
                        break;
                    }
                    case 11:
                        std::cout << "Enter <count> <name1> <value1> <name2> <value2> ...: ";
                        read_object(table);
                        break;
                    default:
                        std::cout << "No such item\n";
                }
            } catch (const std::logic_error& error) {
                std::cout << "Error: " << error.what() << std::endl;
            }
        }
    }
    }
    
    void run_app(){
        Characteristic_simple simple;
        Characteristic_hard table;
    
        try {
            while (true){
                print_menu_choice();
                const int choice = read_int("> ");
                if (choice == 0) return;
                if (choice == 1) simple_menu(simple);
                else if (choice == 2) hard_menu(table, simple);
                else std::cout << "No such item" << std::endl;
            }
        } catch (const InputClosed&) {
            std::cout << "\nInput closed" << std::endl;
        }
}