#include <iostream>

class Characteristic_simple{
    std::string name;
    int value = 1;

    public:
        std::string get_name(){
            return name;
        }

        int get_value(){
            return value;
        }

        void set_name(std::string name){
            this->name = name;
        }

        void set_value(int value){
            if (value < 1 || value > 20) {
                throw std::invalid_argument("Value must be between 1 and 20");
            }
            this->value = value;
        }

        Characteristic_simple(){
            name = "Indiana";
            value = 1;
        }

        Characteristic_simple(std::string name){
            this->name = name;
            value = 1;
        }

        Characteristic_simple(std::string name, int value){
            if (value < 1 || value > 20) {
                throw std::invalid_argument("Value must be between 1 and 20");
            }
            this->name = name;
            this->value = value;
        }

        bool check_difficulty(int cube = 1, int difficulty = 1){
            if (cube < 1 || cube > 20){
                throw std::invalid_argument("Cube is bigger 0 and less 21");
            }

            if (difficulty < 1){
                throw std::invalid_argument("difficulty is natural number");
            }

            if (cube == 1) return false;
            else if (cube == 20) return true;

            int poison = value - 10;
            if (poison < 0 && poison%2 == -1) poison = poison/2 - 1;
            else poison = poison/2;

            return poison + cube >= difficulty;
        }

        bool check_difficulty_advantage(int cube_1 = 1, int cube_2 = 1, int difficulty = 1){
            if ((cube_1 < 1 || cube_1 > 20) || (cube_2 < 1 || cube_2 > 20)){
                throw std::invalid_argument("Cube is bigger 0 and less 21");
            }
            int cube = cube_1 > cube_2 ? cube_1 : cube_2;
            return check_difficulty(cube, difficulty);
        }

        bool check_difficulty_interference(int cube_1 = 1, int cube_2 = 1, int difficulty = 1){
            if ((cube_1 < 1 || cube_1 > 20) || (cube_2 < 1 || cube_2 > 20)){
                throw std::invalid_argument("Cube is bigger 0 and less 21");
            }
            int cube = cube_1 > cube_2 ? cube_2 : cube_1;
            return check_difficulty(cube, difficulty);
        }

        Characteristic_simple& operator+=(int add_value){
            if (add_value < 1 || add_value > 20){
                throw std::invalid_argument("Add_value is less 21 and is bigger 1");
            }

            int value = this->value + add_value;
            if (value > 20) value = 20;

            set_value(value);
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
                if (temp_value < 1 || temp_value > 20) {
                    is.setstate(std::ios::failbit); // число вне границ [1, 20] — ставим ошибку потока
                } else {
                    obj.name = temp_name;
                    obj.value = temp_value;
                }
            }
            return is;
        }
};