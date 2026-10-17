#pragma once

#include <iosfwd>
#include <string>

class Characteristic_simple{
    std::string name = "Unnamed";
    int value = 1;

    static bool is_in_range(int number);
    static void check_value_conditions(const std::string& name, int number);
    static void check_name(const std::string& name);

    public:
        Characteristic_simple() = default;
        explicit Characteristic_simple(const std::string& name);
        explicit Characteristic_simple(const std::string& name, int value);

        const std::string& get_name() const;
        int get_value() const;
        int get_modifier() const;

        void set_name(const std::string& new_name);
        void set_value(int new_value);

        bool check_difficulty(int cube, int difficulty) const;
        bool check_difficulty_advantage(int first_cube, int second_cube, int difficulty) const;
        bool check_difficulty_disadvantage(int first_cube, int second_cube, int difficulty) const;

        Characteristic_simple& operator+=(int add_value);

        friend std::ostream& operator<<(std::ostream& os, const Characteristic_simple& obj);
        friend std::istream& operator>>(std::istream& is, Characteristic_simple& obj);
};