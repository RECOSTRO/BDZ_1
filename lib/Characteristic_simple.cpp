#include "Characteristic_simple.h"
#include <algorithm>
#include <cctype>
#include <format>
#include <istream>
#include <ostream>
#include <stdexcept>

bool Characteristic_simple::is_in_range(int number){
    return number >= 1 && number <= 20;
}

void Characteristic_simple::check_value_conditions(const std::string& what, int number){
    if (!is_in_range(number)){
        throw std::invalid_argument(std::format("{} must be between 1 and 20", what));
    }
}

void Characteristic_simple::check_name(const std::string& name_to_check){
    bool has_space = std::any_of(name_to_check.begin(), name_to_check.end(),
        [](unsigned char c){ return std::isspace(c) != 0; });

    if (name_to_check.empty() || has_space){
        throw std::invalid_argument("Name must be non-empty and without spaces");
    }
}

const std::string& Characteristic_simple::get_name() const{
    return name;
}

int Characteristic_simple::get_value() const{
    return value;
}

int Characteristic_simple::get_modifier() const{
    return value / 2 - 5;
}

void Characteristic_simple::set_name(const std::string& new_name){
    check_name(new_name);
    name = new_name;
}

void Characteristic_simple::set_value(int new_value){
    check_value_conditions("Value", new_value);
    value = new_value;
}

Characteristic_simple::Characteristic_simple(const std::string& new_name)
    : name(new_name){
    check_name(name);
}

Characteristic_simple::Characteristic_simple(const std::string& new_name, int new_value)
    : name(new_name), value(new_value){
    check_name(name);
    check_value_conditions("Value", value);
}

bool Characteristic_simple::check_difficulty(int cube, int difficulty) const{
    check_value_conditions("Cube", cube);

    if (difficulty < 1){
        throw std::invalid_argument("Difficulty must be a natural number");
    }

    if (cube == 1) return false;
    if (cube == 20) return true;

    return get_modifier() + cube >= difficulty;
}

bool Characteristic_simple::check_difficulty_advantage(int first_cube, int second_cube, int difficulty) const{
    check_value_conditions("First_cube", first_cube);
    check_value_conditions("Second_cube", second_cube);

    int cube = std::max(first_cube, second_cube);

    return check_difficulty(cube, difficulty);
}

bool Characteristic_simple::check_difficulty_disadvantage(int first_cube, int second_cube, int difficulty) const{
    check_value_conditions("First_cube", first_cube);
    check_value_conditions("Second_cube", second_cube);

    int cube = std::min(first_cube, second_cube);

    return check_difficulty(cube, difficulty);
}

Characteristic_simple& Characteristic_simple::operator+=(int add_value){
    check_value_conditions("Add_value", add_value);

    int new_value = value + add_value;
    if (new_value > 20) new_value = 20;

    set_value(new_value);
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Characteristic_simple& obj){
    os << obj.name << " " << obj.value;
    return os;
}

std::istream& operator>>(std::istream& is, Characteristic_simple& obj){
    std::string temp_name;
    int temp_value = 0;
    if (is >> temp_name >> temp_value){
        if (!Characteristic_simple::is_in_range(temp_value)){
            is.setstate(std::ios::failbit);
        } else {
            obj.name = temp_name;
            obj.value = temp_value;
        }
    }
    return is;
}
