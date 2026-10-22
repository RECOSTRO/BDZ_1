/**
 * @file Characteristic_simple.h
 * @brief Объявление простого класса Characteristic_simple — характеристика.
 */
#pragma once

#include <iosfwd>
#include <string>

/**
 * @brief Характеристика: название и значение от 1 до 20.
 *
 * Инвариант класса: название непустое и не содержит пробельных символов,
 * значение лежит в диапазоне [1, 20].
 *
 * Класс умеет выполнять проверку характеристики: к броску кубика D20
 * прибавляется модификатор floor((x - 10) / 2), где x — значение характеристики,
 * и результат сравнивается со сложностью. Бросок 1 — критический провал,
 * бросок 20 — критический успех.
 */
class Characteristic_simple{
    std::string name = "Unnamed"; ///< Название характеристики (непустое, без пробелов).
    int value = 1;                ///< Значение характеристики в диапазоне [1, 20].

    /**
     * @brief Проверяет, что число лежит в диапазоне [1, 20].
     * 
     * Не бросате исключений, а просто говорит правда или ложь.
     * 
     * @param number Проверяемое число.
     * @return true, если 1 <= number <= 20, иначе false.
     */
    static bool is_in_range(int number);

    /**
     * @brief Проверяет, что число лежит в диапазоне [1, 20].
     * @param what Название проверяемой величины (используется в тексте исключения).
     * @param number Проверяемое число.
     * @throws std::invalid_argument если number не лежит в диапазоне [1, 20].
     */
    static void check_value_conditions(const std::string& what, int number);

    /**
     * @brief Проверяет корректность названия характеристики.
     * @param name_to_check Проверяемое название.
     * @throws std::invalid_argument если название пустое или содержит пробельные символы.
     */
    static void check_name(const std::string& name_to_check);

    public:
        /**
         * @brief Пустой конструктор.
         *
         * Создаёт характеристику с названием "Unnamed" и значением 1.
         */
        Characteristic_simple() = default;

        /**
         * @brief Создаёт характеристику только с названием; значение по умолчанию равно 1.
         * @param new_name Название характеристики.
         * @throws std::invalid_argument если new_name пустое или содержит пробельные символы.
         */
        explicit Characteristic_simple(const std::string& new_name);

        /**
         * @brief Создаёт характеристику с заданными названием и значением.
         * @param new_name Название характеристики.
         * @param new_value Значение характеристики.
         * @throws std::invalid_argument если new_name пустое или содержит пробельные символы.
         * @throws std::invalid_argument если new_value не лежит в диапазоне [1, 20].
         */
        explicit Characteristic_simple(const std::string& new_name, int new_value);

        /**
         * @brief Возвращает название характеристики.
         * @return Константная ссылка на название.
         */
        const std::string& get_name() const;

        /**
         * @brief Возвращает значение характеристики.
         * @return Значение в диапазоне [1, 20].
         */
        int get_value() const;

        /**
         * @brief Вычисляет модификатор характеристики floor((value - 10) / 2).
         * @return Модификатор в диапазоне [-5, 5].
         */
        int get_modifier() const;

        /**
         * @brief Изменяет название характеристики.
         * @param new_name Новое название.
         * @throws std::invalid_argument если new_name пустое или содержит пробельные символы
         *         (при этом объект не изменяется).
         */
        void set_name(const std::string& new_name);

        /**
         * @brief Изменяет значение характеристики.
         * @param new_value Новое значение.
         * @throws std::invalid_argument если new_value не лежит в диапазоне [1, 20]
         *         (при этом объект не изменяется).
         */
        void set_value(int new_value);

        /**
         * @brief Выполняет обычную проверку характеристики.
         *
         * Результат броска 1 — критический провал, 20 — критический успех;
         * иначе проверка пройдена, если cube + get_modifier() >= difficulty.
         *
         * @param cube Результат броска кубика D20.
         * @param difficulty Сложность проверки (натуральное число).
         * @return true, если проверка пройдена, иначе false.
         * @throws std::invalid_argument если cube не лежит в диапазоне [1, 20].
         * @throws std::invalid_argument если difficulty < 1.
         */
        bool check_difficulty(int cube, int difficulty) const;

        /**
         * @brief Выполняет проверку с преимуществом: из двух бросков берётся наибольший.
         * @param first_cube Результат первого броска D20.
         * @param second_cube Результат второго броска D20.
         * @param difficulty Сложность проверки (натуральное число).
         * @return true, если проверка пройдена, иначе false.
         * @throws std::invalid_argument если first_cube или second_cube не лежит в диапазоне [1, 20].
         * @throws std::invalid_argument если difficulty < 1.
         */
        bool check_difficulty_advantage(int first_cube, int second_cube, int difficulty) const;

        /**
         * @brief Выполняет проверку с помехой: из двух бросков берётся наименьший.
         * @param first_cube Результат первого броска D20.
         * @param second_cube Результат второго броска D20.
         * @param difficulty Сложность проверки (натуральное число).
         * @return true, если проверка пройдена, иначе false.
         * @throws std::invalid_argument если first_cube или second_cube не лежит в диапазоне [1, 20].
         * @throws std::invalid_argument если difficulty < 1.
         */
        bool check_difficulty_disadvantage(int first_cube, int second_cube, int difficulty) const;

        /**
         * @brief Увеличивает значение характеристики на заданное число.
         *
         * Если сумма превышает 20, значение устанавливается РАВНЫМ 20.
         *
         * @param add_value Величина увеличения.
         * @return Ссылка на текущий объект.
         * @throws std::invalid_argument если add_value не лежит в диапазоне [1, 20]
         *         (при этом объект не изменяется).
         */
        Characteristic_simple& operator+=(int add_value);

        /**
         * @brief Выводит характеристику в поток в формате "<название> <значение>".
         * @param os Выходной поток.
         * @param obj Выводимая характеристика.
         * @return Ссылка на поток os.
         */
        friend std::ostream& operator<<(std::ostream& os, const Characteristic_simple& obj);

        /**
         * @brief Читает характеристику из потока в формате "<название> <значение>".
         *
         * Исключений не выбрасывает: при ошибке чтения или значении вне диапазона [1, 20]
         * у потока устанавливается failbit, а объект obj остаётся без изменений.
         *
         * @param is Входной поток.
         * @param obj Объект, в который записывается прочитанная характеристика.
         * @return Ссылка на поток is.
         */
        friend std::istream& operator>>(std::istream& is, Characteristic_simple& obj);
};
