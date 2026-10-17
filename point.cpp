#include <string>
#include <stdexcept>
#include <ostream>
#include <istream>
#include <format>
#include <algorithm>
#include <vector>
#include <cstddef>
#include <random>
#include <utility>
#include <cctype>
 
 
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
 
 
enum class checkmode{
    normal,
    advantage,
    disadvantage
};
 
class Characteristic_hard{
    Characteristic_simple *data = nullptr;
    std::size_t current_size = 0;
    std::size_t capacity = 0;
 
    void upgrade_data(){
        std::size_t new_capacity = capacity == 0 ? 1 : capacity * 2;
        Characteristic_simple *new_data = new Characteristic_simple[new_capacity];
        std::copy(data, data + current_size, new_data);
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }
 
    Characteristic_simple *find(const std::string& name) const{
        for (std::size_t i = 0; i < current_size; i++){
            if (data[i].get_name() == name) return &data[i];
        }
        return nullptr;
    }
 
    void swap(Characteristic_hard& other) noexcept{
        std::swap(data, other.data);
        std::swap(current_size, other.current_size);
        std::swap(capacity, other.capacity);
    }
 
    static int roll_d20(){
        static std::mt19937 generator(std::random_device{}());
        std::uniform_int_distribution<int> distribution(1, 20);
        return distribution(generator);
    }
 
    public:
 
        ~Characteristic_hard(){
            delete[] data;
        }
 
        Characteristic_hard(){
            data = new Characteristic_simple[10];
            current_size = 0;
            capacity = 10;
        }
 
        explicit Characteristic_hard(const std::vector<std::string>& names)
            : Characteristic_hard(){
            for (const auto& name : names){
                *this += Characteristic_simple(name);
            }
        }
 
        Characteristic_hard(const std::string& name, int value)
            : Characteristic_hard(){
            *this += Characteristic_simple(name, value);
        }

        Characteristic_hard(const Characteristic_hard& other)
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

        Characteristic_hard& operator=(const Characteristic_hard& other){
            if (this != &other){
                Characteristic_hard temp(other);
                swap(temp);
            }
            return *this;
        }

        Characteristic_hard(Characteristic_hard&& other) noexcept
            : data(other.data),
              current_size(other.current_size),
              capacity(other.capacity){
            other.data = nullptr;
            other.current_size = 0;
            other.capacity = 0;
        }

        Characteristic_hard& operator=(Characteristic_hard&& other) noexcept{
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
 
        Characteristic_simple& operator[](const std::string& name){
            Characteristic_simple *a = find(name);
            if (a == nullptr) throw std::out_of_range(std::format("{} isn't finding in sorted table", name));
            return *a;
        }
 
        const Characteristic_simple& operator[](const std::string& name) const{
            const Characteristic_simple *a = find(name);
            if (a == nullptr) throw std::out_of_range(std::format("{} isn't finding in sorted table", name));
            return *a;
        }
 
        bool operator()(const std::string& name, checkmode mode, int difficulty) const{
            const Characteristic_simple& a = (*this)[name];
 
            switch (mode){
                case checkmode::normal:
                    return a.check_difficulty(roll_d20(), difficulty);
                case checkmode::advantage:
                    return a.check_difficulty_advantage(roll_d20(), roll_d20(), difficulty);
                case checkmode::disadvantage:
                    return a.check_difficulty_disadvantage(roll_d20(), roll_d20(), difficulty);
            }
            throw std::invalid_argument("Check mode is not finding");
        }
 
        Characteristic_hard& operator+=(const Characteristic_simple& new_characteristic){
            Characteristic_simple *a = find(new_characteristic.get_name());
            if (a != nullptr) throw std::invalid_argument(std::format("This characteristic({}) already use", new_characteristic.get_name()));
 
            if (current_size == capacity) upgrade_data();
 
            data[current_size] = new_characteristic;
            current_size++;
            return *this;
        }
 
        void sort_by_name(bool descending = false){
            std::sort(data, data + current_size,
                [descending](const Characteristic_simple& a, const Characteristic_simple& b){
                    return descending ? a.get_name() > b.get_name()
                                      : a.get_name() < b.get_name();
                });
        }
 
        Characteristic_simple& search_max_in_array(const std::vector<std::string>& array_of_string){
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
            if (max_value == -1){
                throw std::out_of_range("Characteristic not found in data");
            }
            return *b;
        }
 
        friend std::ostream& operator<<(std::ostream& os, const Characteristic_hard& table){
            os << table.current_size;
            for (std::size_t i = 0; i < table.current_size; i++){
                os << "\n   " << table.data[i];
            }
            return os;
        }
 
        friend std::istream& operator>>(std::istream& is, Characteristic_hard& table){
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
};