/**
 * @file Characteristic_hard.h
 * @brief Объявление сложного класса Characteristic_hard — таблица характеристик.
 */
#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "Characteristic_simple.h"

/**
 * @brief Режим проверки характеристики.
 */
enum class checkmode{
    normal,      ///< Обычная проверка: один бросок D20.
    advantage,   ///< С преимуществом: два броска D20, берётся наибольший.
    disadvantage ///< С помехой: два броска D20, берётся наименьший.
};

/**
 * @brief Таблица характеристик персонажа.
 *
 * Хранит множество характеристик в просматриваемой таблице (динамический массив).
 * Инвариант класса: названия всех характеристик в таблице различны,
 * current_size <= capacity, data указывает на массив из capacity элементов.
 */
class Characteristic_hard{
    Characteristic_simple *data = nullptr; ///< Массив характеристик размером capacity.
    std::size_t current_size = 0;          ///< Количество занятых элементов.
    std::size_t capacity = 0;              ///< Размер выделенного массива.

    /**
     * @brief Увеличивает вместимость массива вдвое (или до 1, если она была 0).
     * @throws std::bad_alloc если не удалось выделить память.
     */
    void upgrade_data();

    /**
     * @brief Ищет характеристику по названию.
     * @param name Название искомой характеристики.
     * @return Указатель на найденную характеристику или nullptr, если её нет.
     */
    Characteristic_simple *find(const std::string& name) const;

    /**
     * @brief Обменивает содержимое двух таблиц.
     * @param other Таблица, с которой выполняется обмен.
     */
    void swap(Characteristic_hard& other) noexcept;

    /**
     * @brief Бросает кубик D20.
     * @return Случайное число от 1 до 20.
     */
    static int roll_d20();

    public:
        /**
         * @brief Пустой конструктор: создаёт пустую таблицу вместимостью 10.
         * @throws std::bad_alloc если не удалось выделить память.
         */
        Characteristic_hard();

        /**
         * @brief Создаёт таблицу из характеристик с заданными названиями (значения равны 1).
         * @param names Массив названий характеристик.
         * @throws std::invalid_argument если какое-либо название пустое или содержит пробельные символы.
         * @throws std::invalid_argument если в names есть повторяющиеся названия.
         * @throws std::bad_alloc если не удалось выделить память.
         */
        explicit Characteristic_hard(const std::vector<std::string>& names);

        /**
         * @brief Создаёт таблицу из одной характеристики.
         * @param name Название характеристики.
         * @param value Значение характеристики.
         * @throws std::invalid_argument если name пустое или содержит пробельные символы.
         * @throws std::invalid_argument если value не лежит в диапазоне [1, 20].
         * @throws std::bad_alloc если не удалось выделить память.
         */
        Characteristic_hard(const std::string& name, int value);

        /**
         * @brief Деструктор: освобождает память, занятую таблицей.
         */
        ~Characteristic_hard();

        /**
         * @brief Копирующий конструктор.
         * @param other Копируемая таблица.
         * @throws std::bad_alloc если не удалось выделить память.
         */
        Characteristic_hard(const Characteristic_hard& other);

        /**
         * @brief Копирующее присваивание.
         *
         * При исключении текущая таблица не изменяется.
         *
         * @param other Копируемая таблица.
         * @return Ссылка на текущий объект.
         * @throws std::bad_alloc если не удалось выделить память.
         */
        Characteristic_hard& operator=(const Characteristic_hard& other);

        /**
         * @brief Перемещающий конструктор. После перемещения other остаётся пустым.
         * @param other Перемещаемая таблица.
         */
        Characteristic_hard(Characteristic_hard&& other) noexcept;

        /**
         * @brief Перемещающее присваивание. После перемещения other остаётся пустым.
         * @param other Перемещаемая таблица.
         * @return Ссылка на текущий объект.
         */
        Characteristic_hard& operator=(Characteristic_hard&& other) noexcept;

        /**
         * @brief Возвращает характеристику по её названию.
         * @param name Название характеристики.
         * @return Ссылка на характеристику, хранящуюся в таблице.
         * @throws std::out_of_range если характеристики с таким названием нет в таблице.
         * @warning Так как значений возвращается по ссылке, есть опаснность её изменения.
         */
        Characteristic_simple& operator[](const std::string& name);

        /**
         * @brief Возвращает характеристику по её названию (константная версия).
         * @param name Название характеристики.
         * @return Константная ссылка на характеристику, хранящуюся в таблице.
         * @throws std::out_of_range если характеристики с таким названием нет в таблице.
         */
        const Characteristic_simple& operator[](const std::string& name) const;

        /**
         * @brief Проходит проверку по заданной характеристике со случайным броском D20.
         * @param name Название характеристики.
         * @param mode Режим проверки(enum class).
         * @param difficulty Сложность проверки (натуральное число).
         * @return true, если проверка пройдена, иначе false.
         * @throws std::out_of_range если характеристики с таким названием нет в таблице.
         * @throws std::invalid_argument если difficulty < 1.
         * @throws std::invalid_argument если mode не является допустимым значением checkmode(0, 1, 2).
         */
        bool operator()(const std::string& name, checkmode mode, int difficulty) const;

        /**
         * @brief Добавляет новую характеристику в таблицу.
         *
         * При исключении таблица не изменяется.
         *
         * @param new_characteristic Добавляемая характеристика.
         * @return Ссылка на текущий объект.
         * @throws std::invalid_argument если характеристика с таким названием уже есть в таблице.
         * @throws std::bad_alloc если не удалось выделить память при расширении таблицы.
         */
        Characteristic_hard& operator+=(const Characteristic_simple& new_characteristic);

        /**
         * @brief Упорядочивает характеристики по названию (лексикографически).
         * @param descending true — по убыванию, false (по умолчанию) — по возрастанию.
         */
        void sort_by_name(bool descending = false);

        /**
         * @brief Ищет характеристику с максимальным значением среди заданных.
         *
         * Названия, которых нет в таблице, пропускаются. При равенстве значений с разными названиями
         * возвращается та характеристика, что встретилась в array_of_string раньше.
         *
         * @param array_of_string Массив названий характеристик, среди которых ведётся поиск.
         * @return Ссылка на найденную характеристику, хранящуюся в таблице.
         * @throws std::out_of_range если ни одного из названий нет в таблице.
         */
        Characteristic_simple& search_max_in_array(const std::vector<std::string>& array_of_string);

        /**
         * @brief Выводит таблицу в поток.
         *
         * Формат: сначала количество характеристик, затем каждая характеристика
         * на отдельной строке в формате "<название> <значение>".
         *
         * @param os Выходной поток.
         * @param table Выводимая таблица.
         * @return Ссылка на поток os.
         */
        friend std::ostream& operator<<(std::ostream& os, const Characteristic_hard& table);

        /**
         * @brief Читает таблицу из потока.
         *
         * Формат: количество характеристик N, затем N пар "<название> <значение>".
         * При ошибке чтения, значении вне диапазона [1, 20] или повторяющемся названии
         * у потока устанавливается failbit, а таблица table остаётся без изменений.
         *
         * @param is Входной поток.
         * @param table Таблица, в которую записывается результат.
         * @return Ссылка на поток is.
         * @throws std::bad_alloc если не удалось выделить память.
         */
        friend std::istream& operator>>(std::istream& is, Characteristic_hard& table);
};
