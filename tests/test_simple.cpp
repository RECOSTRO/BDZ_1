#include <gtest/gtest.h>

#include <climits>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Characteristic_simple.h"

// ===================== Конструкторы =====================

TEST(SimpleConstructor, DefaultValues){
    Characteristic_simple c;
    EXPECT_EQ(c.get_name(), "Unnamed");
    EXPECT_EQ(c.get_value(), 1);
}

TEST(SimpleConstructor, OnlyNameSetsDefaultValue){
    Characteristic_simple c("Strength");
    EXPECT_EQ(c.get_name(), "Strength");
    EXPECT_EQ(c.get_value(), 1);
}

TEST(SimpleConstructor, NameAndValue){
    Characteristic_simple c("Dexterity", 15);
    EXPECT_EQ(c.get_name(), "Dexterity");
    EXPECT_EQ(c.get_value(), 15);
}

TEST(SimpleConstructor, BoundaryValuesAccepted){
    EXPECT_NO_THROW(Characteristic_simple("A", 1));
    EXPECT_NO_THROW(Characteristic_simple("A", 20));
}

TEST(SimpleConstructor, OutOfRangeValueThrows){
    EXPECT_THROW(Characteristic_simple("A", 0), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple("A", 21), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple("A", -5), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple("A", INT_MAX), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple("A", INT_MIN), std::invalid_argument);
}

TEST(SimpleConstructor, BadNameThrows){
    EXPECT_THROW(Characteristic_simple(""), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple("Sleight of hand"), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple("Tab\tName"), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple(" "), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple("", 10), std::invalid_argument);
    EXPECT_THROW(Characteristic_simple("a b", 10), std::invalid_argument);
}

// ===================== Сеттеры =====================

TEST(SimpleSetters, SetValueBoundaries){
    Characteristic_simple c("A", 10);
    c.set_value(1);
    EXPECT_EQ(c.get_value(), 1);
    c.set_value(20);
    EXPECT_EQ(c.get_value(), 20);
}

TEST(SimpleSetters, SetValueOutOfRangeThrowsAndKeepsOld){
    Characteristic_simple c("A", 10);
    EXPECT_THROW(c.set_value(0), std::invalid_argument);
    EXPECT_THROW(c.set_value(21), std::invalid_argument);
    EXPECT_EQ(c.get_value(), 10);
}

TEST(SimpleSetters, SetNameChangesName){
    Characteristic_simple c("Old", 10);
    c.set_name("New");
    EXPECT_EQ(c.get_name(), "New");
}

TEST(SimpleSetters, SetBadNameThrowsAndKeepsOld){
    Characteristic_simple c("Old", 10);
    EXPECT_THROW(c.set_name(""), std::invalid_argument);
    EXPECT_THROW(c.set_name("two words"), std::invalid_argument);
    EXPECT_EQ(c.get_name(), "Old");
}

// ===================== Модификатор =====================

TEST(SimpleModifier, AllValuesMatchFloorFormula){
    // floor((x - 10) / 2) для всех x от 1 до 20
    const int expected[] = {-5, -4, -4, -3, -3, -2, -2, -1, -1, 0,
                             0,  1,  1,  2,  2,  3,  3,  4,  4, 5};
    for (int x = 1; x <= 20; x++){
        Characteristic_simple c("A", x);
        EXPECT_EQ(c.get_modifier(), expected[x - 1]) << "value = " << x;
    }
}

TEST(SimpleModifier, OddValuesBelowTenRoundDown){
    // Здесь целочисленное деление C++ дало бы неправильный ответ
    EXPECT_EQ(Characteristic_simple("A", 9).get_modifier(), -1);
    EXPECT_EQ(Characteristic_simple("A", 7).get_modifier(), -2);
    EXPECT_EQ(Characteristic_simple("A", 1).get_modifier(), -5);
}

// ===================== Обычная проверка =====================

TEST(SimpleCheck, ExactlyEqualToDifficultyPasses){
    Characteristic_simple c("A", 10);          // модификатор 0
    EXPECT_TRUE(c.check_difficulty(15, 15));   // 0 + 15 >= 15
    EXPECT_FALSE(c.check_difficulty(15, 16));  // 0 + 15 <  16
}

TEST(SimpleCheck, ModifierIsApplied){
    Characteristic_simple strong("A", 20);     // +5
    EXPECT_TRUE(strong.check_difficulty(10, 15));
    EXPECT_FALSE(strong.check_difficulty(10, 16));

    Characteristic_simple weak("B", 1);        // -5
    EXPECT_TRUE(weak.check_difficulty(10, 5));
    EXPECT_FALSE(weak.check_difficulty(10, 6));
}

TEST(SimpleCheck, CriticalFailOnOne){
    // Даже при максимальном значении и минимальной сложности
    Characteristic_simple c("A", 20);
    EXPECT_FALSE(c.check_difficulty(1, 1));
}

TEST(SimpleCheck, CriticalSuccessOnTwenty){
    // Даже при минимальном значении и огромной сложности
    Characteristic_simple c("A", 1);
    EXPECT_TRUE(c.check_difficulty(20, 1000));
    EXPECT_TRUE(c.check_difficulty(20, INT_MAX));
}

TEST(SimpleCheck, RollsNextToCriticals){
    Characteristic_simple c("A", 10);
    EXPECT_TRUE(c.check_difficulty(2, 2));     // 2 — уже не крит. провал
    EXPECT_FALSE(c.check_difficulty(19, 20));  // 19 — ещё не крит. успех
}

TEST(SimpleCheck, MinimalDifficulty){
    Characteristic_simple c("A", 1);           // -5
    EXPECT_FALSE(c.check_difficulty(5, 1));    // -5 + 5 = 0 < 1
    EXPECT_TRUE(c.check_difficulty(6, 1));     // -5 + 6 = 1 >= 1
}

TEST(SimpleCheck, InvalidCubeThrows){
    Characteristic_simple c("A", 10);
    EXPECT_THROW(c.check_difficulty(0, 10), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty(21, 10), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty(-1, 10), std::invalid_argument);
}

TEST(SimpleCheck, InvalidDifficultyThrows){
    Characteristic_simple c("A", 10);
    EXPECT_THROW(c.check_difficulty(10, 0), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty(10, -10), std::invalid_argument);
    // Неверная сложность — ошибка, даже если выпал крит
    EXPECT_THROW(c.check_difficulty(20, 0), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty(1, 0), std::invalid_argument);
}

TEST(SimpleCheck, WorksOnConstObject){
    const Characteristic_simple c("A", 10);
    EXPECT_TRUE(c.check_difficulty(10, 10));
}

// ===================== Преимущество и помеха =====================

TEST(SimpleAdvantage, TakesMaximum){
    Characteristic_simple c("A", 10);
    EXPECT_TRUE(c.check_difficulty_advantage(5, 15, 15));
    EXPECT_TRUE(c.check_difficulty_advantage(15, 5, 15));  // порядок не важен
    EXPECT_FALSE(c.check_difficulty_advantage(5, 14, 15));
}

TEST(SimpleAdvantage, CriticalsWithTwoDice){
    Characteristic_simple c("A", 10);
    EXPECT_TRUE(c.check_difficulty_advantage(1, 20, 1000));  // max = 20
    EXPECT_FALSE(c.check_difficulty_advantage(1, 1, 1));     // оба 1
    EXPECT_TRUE(c.check_difficulty_advantage(1, 5, 5));      // max = 5, без крита
}

TEST(SimpleDisadvantage, TakesMinimum){
    Characteristic_simple c("A", 10);
    EXPECT_FALSE(c.check_difficulty_disadvantage(5, 15, 15));
    EXPECT_FALSE(c.check_difficulty_disadvantage(15, 5, 15));
    EXPECT_TRUE(c.check_difficulty_disadvantage(15, 16, 15));
}

TEST(SimpleDisadvantage, CriticalsWithTwoDice){
    Characteristic_simple c("A", 20);
    EXPECT_FALSE(c.check_difficulty_disadvantage(1, 20, 1));  // min = 1
    EXPECT_TRUE(c.check_difficulty_disadvantage(20, 20, 1000));
}

TEST(SimpleAdvantageDisadvantage, InvalidCubesThrow){
    Characteristic_simple c("A", 10);
    EXPECT_THROW(c.check_difficulty_advantage(0, 10, 10), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty_advantage(10, 21, 10), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty_disadvantage(21, 10, 10), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty_disadvantage(10, 0, 10), std::invalid_argument);
    // Неверный кубик, который НЕ был бы выбран, тоже ошибка
    EXPECT_THROW(c.check_difficulty_advantage(25, 0, 10), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty_disadvantage(0, 25, 10), std::invalid_argument);
}

TEST(SimpleAdvantageDisadvantage, InvalidDifficultyThrows){
    Characteristic_simple c("A", 10);
    EXPECT_THROW(c.check_difficulty_advantage(10, 10, 0), std::invalid_argument);
    EXPECT_THROW(c.check_difficulty_disadvantage(10, 10, 0), std::invalid_argument);
}

// ===================== operator+= =====================

TEST(SimplePlusEquals, AddsValue){
    Characteristic_simple c("A", 10);
    c += 5;
    EXPECT_EQ(c.get_value(), 15);
}

TEST(SimplePlusEquals, ClampsToTwenty){
    Characteristic_simple c("A", 15);
    c += 10;
    EXPECT_EQ(c.get_value(), 20);

    Characteristic_simple d("B", 20);
    d += 20;
    EXPECT_EQ(d.get_value(), 20);
}

TEST(SimplePlusEquals, ExactlyReachesTwenty){
    Characteristic_simple c("A", 19);
    c += 1;
    EXPECT_EQ(c.get_value(), 20);
}

TEST(SimplePlusEquals, BoundaryAddValues){
    Characteristic_simple c("A", 1);
    EXPECT_NO_THROW(c += 1);
    EXPECT_EQ(c.get_value(), 2);
    EXPECT_NO_THROW(c += 20);
    EXPECT_EQ(c.get_value(), 20);
}

TEST(SimplePlusEquals, InvalidAddThrowsAndKeepsValue){
    Characteristic_simple c("A", 10);
    EXPECT_THROW(c += 0, std::invalid_argument);
    EXPECT_THROW(c += -3, std::invalid_argument);
    EXPECT_THROW(c += 21, std::invalid_argument);
    EXPECT_EQ(c.get_value(), 10);
}

TEST(SimplePlusEquals, ReturnsReferenceForChaining){
    Characteristic_simple c("A", 1);
    (c += 2) += 3;
    EXPECT_EQ(c.get_value(), 6);
    EXPECT_EQ(&(c += 1), &c);
}

// ===================== Потоки =====================

TEST(SimpleStream, OutputFormat){
    std::ostringstream out;
    out << Characteristic_simple("Strength", 15);
    EXPECT_EQ(out.str(), "Strength 15");
}

TEST(SimpleStream, OutputSupportsChaining){
    std::ostringstream out;
    out << Characteristic_simple("A", 1) << " | " << Characteristic_simple("B", 2);
    EXPECT_EQ(out.str(), "A 1 | B 2");
}

TEST(SimpleStream, InputValid){
    std::istringstream in("Wisdom 12");
    Characteristic_simple c;
    in >> c;
    EXPECT_FALSE(in.fail());
    EXPECT_EQ(c.get_name(), "Wisdom");
    EXPECT_EQ(c.get_value(), 12);
}

TEST(SimpleStream, InputBoundaryValues){
    std::istringstream in("A 1 B 20");
    Characteristic_simple a, b;
    in >> a >> b;
    EXPECT_FALSE(in.fail());
    EXPECT_EQ(a.get_value(), 1);
    EXPECT_EQ(b.get_value(), 20);
}

TEST(SimpleStream, InputOutOfRangeFailsAndKeepsObject){
    for (const char* text : {"Wisdom 0", "Wisdom 21", "Wisdom -1"}){
        std::istringstream in(text);
        Characteristic_simple c("Old", 5);
        in >> c;
        EXPECT_TRUE(in.fail()) << text;
        EXPECT_EQ(c.get_name(), "Old") << text;
        EXPECT_EQ(c.get_value(), 5) << text;
    }
}

TEST(SimpleStream, InputNotANumberFailsAndKeepsObject){
    std::istringstream in("Wisdom abc");
    Characteristic_simple c("Old", 5);
    in >> c;
    EXPECT_TRUE(in.fail());
    EXPECT_EQ(c.get_name(), "Old");
    EXPECT_EQ(c.get_value(), 5);
}

TEST(SimpleStream, InputEmptyStreamFails){
    std::istringstream in("");
    Characteristic_simple c("Old", 5);
    in >> c;
    EXPECT_TRUE(in.fail());
    EXPECT_EQ(c.get_name(), "Old");
}

TEST(SimpleStream, InputMissingValueFails){
    std::istringstream in("OnlyName");
    Characteristic_simple c("Old", 5);
    in >> c;
    EXPECT_TRUE(in.fail());
    EXPECT_EQ(c.get_name(), "Old");
}

TEST(SimpleStream, RoundTrip){
    Characteristic_simple original("Charisma", 17);
    std::stringstream ss;
    ss << original;
    Characteristic_simple restored;
    ss >> restored;
    EXPECT_FALSE(ss.fail());
    EXPECT_EQ(restored.get_name(), original.get_name());
    EXPECT_EQ(restored.get_value(), original.get_value());
}