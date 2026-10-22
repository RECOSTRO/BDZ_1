#include "Characteristic_hard.h"
#include <algorithm>
#include <format>
#include <istream>
#include <ostream>
#include <random>
#include <stdexcept>
#include <utility>

void Characteristic_hard::upgrade_data(){
    std::size_t new_capacity = capacity == 0 ? 1 : capacity * 2;
    Characteristic_simple *new_data = new Characteristic_simple[new_capacity];
    std::move(data, data + current_size, new_data);
    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

Characteristic_simple *Characteristic_hard::find(const std::string& name) const{
    for (std::size_t i = 0; i < current_size; i++){
        if (data[i].get_name() == name) return &data[i];
    }
    return nullptr;
}

void Characteristic_hard::swap(Characteristic_hard& other) noexcept{
    std::swap(data, other.data);
    std::swap(current_size, other.current_size);
    std::swap(capacity, other.capacity);
}

int Characteristic_hard::roll_d20(){
    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(1, 20);
    return distribution(generator);
}

Characteristic_hard::~Characteristic_hard(){
    delete[] data;
}

Characteristic_hard::Characteristic_hard(){
    data = new Characteristic_simple[10];
    current_size = 0;
    capacity = 10;
}

Characteristic_hard::Characteristic_hard(const std::vector<std::string>& names)
    : Characteristic_hard(){
    for (const auto& name : names){
        *this += Characteristic_simple(name);
    }
}

Characteristic_hard::Characteristic_hard(const std::string& name, int value)
    : Characteristic_hard(){
    *this += Characteristic_simple(name, value);
}

Characteristic_hard::Characteristic_hard(const Characteristic_hard& other)
    : data(new Characteristic_simple[other.capacity]),
      current_size(other.current_size),
      capacity(other.capacity){
    try {
        std::copy(other.data, other.data + other.current_size, data);
    } catch (...) {
        delete[] data;
        throw;
    }
}

Characteristic_hard& Characteristic_hard::operator=(const Characteristic_hard& other){
    if (this != &other){
        Characteristic_hard temp(other);
        swap(temp);
    }
    return *this;
}

Characteristic_hard::Characteristic_hard(Characteristic_hard&& other) noexcept
    : data(other.data),
      current_size(other.current_size),
      capacity(other.capacity){
    other.data = nullptr;
    other.current_size = 0;
    other.capacity = 0;
}

Characteristic_hard& Characteristic_hard::operator=(Characteristic_hard&& other) noexcept{
    if (this != &other){
        delete[] data;
        data = other.data;
        current_size = other.current_size;
        capacity = other.capacity;
        other.data = nullptr;
        other.current_size = 0;
        other.capacity = 0;
    }
    return *this;
}

Characteristic_simple& Characteristic_hard::operator[](const std::string& name){
    Characteristic_simple *a = find(name);
    if (a == nullptr) throw std::out_of_range(std::format("Characteristic '{}' not found in table", name));
    return *a;
}

const Characteristic_simple& Characteristic_hard::operator[](const std::string& name) const{
    const Characteristic_simple *a = find(name);
    if (a == nullptr) throw std::out_of_range(std::format("Characteristic '{}' not found in table", name));
    return *a;
}

bool Characteristic_hard::operator()(const std::string& name, checkmode mode, int difficulty) const{
    const Characteristic_simple& a = (*this)[name];

    switch (mode){
        case checkmode::normal:
            return a.check_difficulty(roll_d20(), difficulty);
        case checkmode::advantage:
            return a.check_difficulty_advantage(roll_d20(), roll_d20(), difficulty);
        case checkmode::disadvantage:
            return a.check_difficulty_disadvantage(roll_d20(), roll_d20(), difficulty);
    }
    throw std::invalid_argument("Unknown check mode");
}

Characteristic_hard& Characteristic_hard::operator+=(const Characteristic_simple& new_characteristic){
    Characteristic_simple *a = find(new_characteristic.get_name());
    if (a != nullptr) throw std::invalid_argument(std::format("Characteristic '{}' already exists", new_characteristic.get_name()));

    if (current_size == capacity) upgrade_data();

    data[current_size] = new_characteristic;
    current_size++;
    return *this;
}

void Characteristic_hard::sort_by_name(bool descending){ 
    std::sort(data, data + current_size,
        [descending](const Characteristic_simple& a, const Characteristic_simple& b){
            return descending ? a.get_name() > b.get_name()
                              : a.get_name() < b.get_name();
        });
}

Characteristic_simple& Characteristic_hard::search_max_in_array(const std::vector<std::string>& array_of_string){
    int max_value = -1;
    Characteristic_simple *b = nullptr;

    for (std::size_t i = 0; i < array_of_string.size(); i++){
        Characteristic_simple *a = find(array_of_string[i]);
        if (a == nullptr) continue;
        if ((*a).get_value() > max_value){
            max_value = (*a).get_value();
            b = a;
        }
    }
    if (b == nullptr){
        throw std::out_of_range("None of the given characteristics is in the table");
    }
    return *b;
}

std::ostream& operator<<(std::ostream& os, const Characteristic_hard& table){
    os << table.current_size;
    for (std::size_t i = 0; i < table.current_size; i++){
        os << "\n   " << table.data[i];
    }
    return os;
}

std::istream& operator>>(std::istream& is, Characteristic_hard& table){
    std::size_t count = 0;
    if (!(is >> count)) return is;

    Characteristic_hard temp;
    for (std::size_t i = 0; i < count; i++){
        Characteristic_simple c;
        if (!(is >> c)) return is;

        try {
            temp += c;
        } catch (const std::invalid_argument&) {
            is.setstate(std::ios::failbit);
            return is;
        }
    }

    table = std::move(temp);
    return is;
}