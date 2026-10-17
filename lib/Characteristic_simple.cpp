#include "Characteristic_simple.h"
#include <algorithm>
#include <cctype>
#include <format>
#include <istream>
#include <ostream>
#include <stdexcept>
 
 
class Characteristic_simple{
    std::string name = "Unnamed";
    int value = 1;
 
    static bool is_in_range(int number){
        return number >= 1 && number <= 20;
    }
 
    static void check_value_conditions(const std::string& name, int number){
        if (!is_in_range(number)){
            throw std::invalid_argument(std::format("{} must be between 1 and 20", name));
        }
    }
 
    static void check_name(const std::string& name){
        bool has_space = std::any_of(name.begin(), name.end(),
            [](unsigned char c){ return std::isspace(c) != 0; });
 
        if (name.empty() || has_space){
            throw std::invalid_argument("Name must be non-empty and without spaces");
        }
    }
 
    public:
        const std::string& get_name() const{
            return name;
        }
 
        int get_value() const{
            return value;
        }
 
        int get_modifier() const{// функция подсчёта сложности
            return value / 2 - 5;
        }
 
        void set_name(const std::string& new_name){
            check_name(new_name);
            name = new_name;
        }
 
        void set_value(int new_value){
            check_value_conditions("Value", new_value);
            value = new_value;
        }
 
        Characteristic_simple() = default;
 
        explicit Characteristic_simple(const std::string& name)
            : name(name){
            check_name(name);
        }
 
        explicit Characteristic_simple(const std::string& name, int value)
            : name(name), value(value){
            check_name(name);
            check_value_conditions("Value", value);
        }
 
        bool check_difficulty(int cube, int difficulty) const{
            check_value_conditions("Cube", cube);
 
            if (difficulty < 1){
                throw std::invalid_argument("Difficulty is natural number");
            }
 
            if (cube == 1) return false;
            if (cube == 20) return true;
 
            return get_modifier() + cube >= difficulty;
        }
 
        bool check_difficulty_advantage(int first_cube, int second_cube, int difficulty) const{
            check_value_conditions("First_cube", first_cube);
            check_value_conditions("Second_cube", second_cube);
 
            int cube = std::max(first_cube, second_cube);
 
            return check_difficulty(cube, difficulty);
        }
 
        bool check_difficulty_disadvantage(int first_cube, int second_cube, int difficulty) const{
            check_value_conditions("First_cube", first_cube);
            check_value_conditions("Second_cube", second_cube);
 
            int cube = std::min(first_cube, second_cube);
 
            return check_difficulty(cube, difficulty);
        }
 
        Characteristic_simple& operator+=(int add_value){
            check_value_conditions("Add_value", add_value);
 
            int new_value = value + add_value;
            if (new_value > 20) new_value = 20;
 
            set_value(new_value);
            return *this;
        }
 
        friend std::ostream& operator<<(std::ostream& os, const Characteristic_simple& obj){
            os << obj.name << " " << obj.value;
            return os;
        }
 
        friend std::istream& operator>>(std::istream& is, Characteristic_simple& obj){
            std::string temp_name;
            int temp_value = 0;
            if (is >> temp_name >> temp_value){
                if (!is_in_range(temp_value)){
                    is.setstate(std::ios::failbit);
                } else {
                    obj.name = temp_name;
                    obj.value = temp_value;
                }
            }
            return is;
        }
};