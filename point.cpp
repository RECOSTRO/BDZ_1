#include <string>
#include <stdexcept>
#include <ostream>
#include <istream>
#include <format>
#include <algorithm>


class Characteristic_simple{
    std::string name = "Unnamed";
    int value = 1;

    static void check_value_conditions(const std::string& name, int number){
        if (number < 1 || number > 20){
            throw std::invalid_argument(std::format("{} must be between 1 and 20", name));
        }
    }

    public:
        const std::string& get_name() const{
            return name;
        }

        int get_value() const{
            return value;
        }

        void set_name(const std::string& name){
            this->name = name;
        }

        void set_value(int value){
            check_value_conditions("Value", value);
            this->value = value;
        }

        Characteristic_simple() = default;

        explicit Characteristic_simple(std::string name)
            : name(name) {}
        
        explicit Characteristic_simple(std::string name, int value)
            : name(name), value(value){
                check_value_conditions("Value", value);
            }

        bool check_difficulty(int cube, int difficulty) const{
            check_value_conditions("Cube", cube);

            if (difficulty < 1){
                throw std::invalid_argument("Difficulty is natural number");
            }

            if (cube == 1) return false;
            else if (cube == 20) return true;

            int poison = value - 10;
            if (poison < 0 && poison%2 == -1) poison = poison/2 - 1;
            else poison = poison/2;

            return poison + cube >= difficulty;
        }

        bool check_difficulty_advantage(int first_cube, int second_cube, int difficulty) const{
            check_value_conditions("First_cube", first_cube);
            check_value_conditions("Second_cube", second_cube);

            int cube = std::max(first_cube, second_cube);

            return check_difficulty(cube, difficulty);
        }

        bool check_difficulty_interference(int first_cube, int second_cube, int difficulty) const{
            check_value_conditions("First_cube", first_cube);
            check_value_conditions("Second_cube", second_cube);

            int cube = std::min(first_cube, second_cube);

            return check_difficulty(cube, difficulty);
        }

        Characteristic_simple& operator+=(int add_value){
            check_value_conditions("Add_value", add_value);

            int new_value = this->value + add_value;
            if (new_value > 20) new_value = 20;

            set_value(new_value);
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, const Characteristic_simple& obj){// надо разобраться в теме
            os << obj.name << " " << obj.value;
            return os;
        }

        friend std::istream& operator>>(std::istream& is, Characteristic_simple& obj){// надо разобраться в теме
            std::string temp_name;
            int temp_value;
            if (is >> temp_name >> temp_value) {
                if (temp_value < 1 || temp_value > 20){
                    is.setstate(std::ios::failbit); // число вне границ [1, 20] — ставим ошибку потока
                } else {
                    obj.name = temp_name;
                    obj.value = temp_value;
                }
            }
            return is;
        }
};

class Characteristic_hard{
    Characteristic_simple *data = nullptr;
    size_t current_size = 0;
    size_t capicity = 0;

    public:

        ~Characteristic_hard(){
            delete[] data;
        }

        Characteristic_hard(){
            data = new Characteristic_simple[10];
            current_size = 0;
            capicity = 10;
        }

        Characteristic_hard(std::string name, int value){
            Characteristic_simple new_class(name, value);
            data = new Characteristic_simple[1];
            data[0] = new_class;
            capicity = 1;
            current_size = 1;
        }

        Characteristic_simple& search_by_name(std::string name){
            for (int i = 0; i < current_size; i++){
                if (data[i].get_name == name) return data[i];
            }
            throw std::out_of_range(std::format"{} isn't finding in sorted table", name);
        }
};