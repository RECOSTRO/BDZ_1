#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "Characteristic_simple.h"

enum class checkmode{
    normal,
    advantage,
    disadvantage
};

class Characteristic_hard{
    Characteristic_simple *data = nullptr;
    std::size_t current_size = 0;
    std::size_t capacity = 0;

    void upgrade_data();
    Characteristic_simple *find(const std::string& name) const;
    void swap(Characteristic_hard& other) noexcept;
    static int roll_d20();

    public:
        Characteristic_hard();
        explicit Characteristic_hard(const std::vector<std::string>& names);
        Characteristic_hard(const std::string& name, int value);

        // правило пяти
        ~Characteristic_hard();
        Characteristic_hard(const Characteristic_hard& other);
        Characteristic_hard& operator=(const Characteristic_hard& other);
        Characteristic_hard(Characteristic_hard&& other) noexcept;
        Characteristic_hard& operator=(Characteristic_hard&& other) noexcept;

        Characteristic_simple& operator[](const std::string& name);
        const Characteristic_simple& operator[](const std::string& name) const;

        bool operator()(const std::string& name, checkmode mode, int difficulty) const;

        Characteristic_hard& operator+=(const Characteristic_simple& new_characteristic);

        void sort_by_name(bool descending = false);

        Characteristic_simple& search_max_in_array(const std::vector<std::string>& array_of_string);

        friend std::ostream& operator<<(std::ostream& os, const Characteristic_hard& table);
        friend std::istream& operator>>(std::istream& is, Characteristic_hard& table);
};