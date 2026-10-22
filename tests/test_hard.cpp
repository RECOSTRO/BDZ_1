#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "Characteristic_hard.h"

namespace {

// Таблица в виде строки (через operator<<)
std::string to_text(const Characteristic_hard& table){
    std::ostringstream out;
    out << table;
    return out.str();
}

// Количество характеристик: первое число в выводе
std::size_t size_of(const Characteristic_hard& table){
    std::istringstream in(to_text(table));
    std::size_t size = 0;
    in >> size;
    return size;
}

} // namespace

// ===================== Конструкторы =====================

TEST(HardConstructor, DefaultIsEmpty){
    Characteristic_hard t;
    EXPECT_EQ(size_of(t), 0u);
    EXPECT_EQ(to_text(t), "0");
}

TEST(HardConstructor, FromNamesUsesDefaultValue){
    Characteristic_hard t({"Strength", "Dexterity", "Wisdom"});
    EXPECT_EQ(size_of(t), 3u);
    EXPECT_EQ(t["Strength"].get_value(), 1);
    EXPECT_EQ(t["Dexterity"].get_value(), 1);
    EXPECT_EQ(t["Wisdom"].get_value(), 1);
}

TEST(HardConstructor, FromEmptyNamesList){
    Characteristic_hard t(std::vector<std::string>{});
    EXPECT_EQ(size_of(t), 0u);
}

TEST(HardConstructor, FromManyNamesGrowsArray){
    std::vector<std::string> names;
    for (int i = 0; i < 100; i++) names.push_back("N" + std::to_string(i));
    Characteristic_hard t(names);
    EXPECT_EQ(size_of(t), 100u);
    EXPECT_NO_THROW(t["N0"]);
    EXPECT_NO_THROW(t["N99"]);
}

TEST(HardConstructor, FromNamesWithDuplicateThrows){
    EXPECT_THROW(Characteristic_hard({"Strength", "Strength"}), std::invalid_argument);
    EXPECT_THROW(Characteristic_hard({"A", "B", "A"}), std::invalid_argument);
}

TEST(HardConstructor, FromNamesWithBadNameThrows){
    EXPECT_THROW(Characteristic_hard({"Good", ""}), std::invalid_argument);
    EXPECT_THROW(Characteristic_hard({"two words"}), std::invalid_argument);
}

TEST(HardConstructor, SingleCharacteristic){
    Characteristic_hard t("Strength", 15);
    EXPECT_EQ(size_of(t), 1u);
    EXPECT_EQ(t["Strength"].get_value(), 15);
}

TEST(HardConstructor, SingleCharacteristicInvalidThrows){
    EXPECT_THROW(Characteristic_hard("Strength", 0), std::invalid_argument);
    EXPECT_THROW(Characteristic_hard("Strength", 21), std::invalid_argument);
    EXPECT_THROW(Characteristic_hard("", 10), std::invalid_argument);
}

// ===================== operator[] =====================

TEST(HardIndex, ReturnsExistingCharacteristic){
    Characteristic_hard t("Strength", 15);
    EXPECT_EQ(t["Strength"].get_name(), "Strength");
    EXPECT_EQ(t["Strength"].get_value(), 15);
}

TEST(HardIndex, ReturnsReferenceThatChangesTable){
    Characteristic_hard t("Strength", 10);
    t["Strength"] += 5;
    EXPECT_EQ(t["Strength"].get_value(), 15);
    t["Strength"].set_value(3);
    EXPECT_EQ(t["Strength"].get_value(), 3);
}

TEST(HardIndex, MissingNameThrows){
    Characteristic_hard t("Strength", 10);
    EXPECT_THROW(t["Dexterity"], std::out_of_range);
    EXPECT_THROW(t["strength"], std::out_of_range);   // регистр важен
    EXPECT_THROW(t[""], std::out_of_range);
}

TEST(HardIndex, EmptyTableThrows){
    Characteristic_hard t;
    EXPECT_THROW(t["Anything"], std::out_of_range);
}

TEST(HardIndex, ConstVersion){
    const Characteristic_hard t("Strength", 12);
    EXPECT_EQ(t["Strength"].get_value(), 12);
    EXPECT_THROW(t["Missing"], std::out_of_range);
}

// ===================== operator+= =====================

TEST(HardPlusEquals, AddsCharacteristic){
    Characteristic_hard t;
    t += Characteristic_simple("Strength", 15);
    EXPECT_EQ(size_of(t), 1u);
    EXPECT_EQ(t["Strength"].get_value(), 15);
}

TEST(HardPlusEquals, StoresCopy){
    Characteristic_hard t;
    Characteristic_simple c("Strength", 15);
    t += c;
    c.set_value(1);                              // меняем оригинал
    EXPECT_EQ(t["Strength"].get_value(), 15);    // в таблице не изменилось
}

TEST(HardPlusEquals, DuplicateThrowsAndTableUnchanged){
    Characteristic_hard t("Strength", 15);
    EXPECT_THROW(t += Characteristic_simple("Strength", 3), std::invalid_argument);
    EXPECT_EQ(size_of(t), 1u);
    EXPECT_EQ(t["Strength"].get_value(), 15);
}

TEST(HardPlusEquals, GrowsPastInitialCapacity){
    Characteristic_hard t;                       // начальная вместимость 10
    for (int i = 0; i < 11; i++){
        t += Characteristic_simple("N" + std::to_string(i), (i % 20) + 1);
    }
    EXPECT_EQ(size_of(t), 11u);
    for (int i = 0; i < 11; i++){
        EXPECT_EQ(t["N" + std::to_string(i)].get_value(), (i % 20) + 1);
    }
}

TEST(HardPlusEquals, ManyAdditions){
    Characteristic_hard t;
    for (int i = 0; i < 1000; i++){
        t += Characteristic_simple("N" + std::to_string(i), (i % 20) + 1);
    }
    EXPECT_EQ(size_of(t), 1000u);
    EXPECT_EQ(t["N999"].get_value(), (999 % 20) + 1);
}

TEST(HardPlusEquals, ReturnsReferenceForChaining){
    Characteristic_hard t;
    (t += Characteristic_simple("A", 1)) += Characteristic_simple("B", 2);
    EXPECT_EQ(size_of(t), 2u);
    EXPECT_EQ(&(t += Characteristic_simple("C", 3)), &t);
}

// ===================== operator() =====================
// Результат случайный, поэтому проверяем исключения
// и то, что за много бросков встречаются оба исхода.

TEST(HardCheck, MissingCharacteristicThrows){
    Characteristic_hard t("Strength", 10);
    EXPECT_THROW(t("Dexterity", checkmode::normal, 10), std::out_of_range);
    EXPECT_THROW(t("Dexterity", checkmode::advantage, 10), std::out_of_range);
    EXPECT_THROW(t("Dexterity", checkmode::disadvantage, 10), std::out_of_range);
}

TEST(HardCheck, InvalidDifficultyThrows){
    Characteristic_hard t("Strength", 10);
    EXPECT_THROW(t("Strength", checkmode::normal, 0), std::invalid_argument);
    EXPECT_THROW(t("Strength", checkmode::advantage, 0), std::invalid_argument);
    EXPECT_THROW(t("Strength", checkmode::disadvantage, -1), std::invalid_argument);
}

TEST(HardCheck, CriticalSuccessHappens){
    // Сложность 1000 проходится только критическим успехом (20 на кубике).
    // За 2000 бросков он выпадет почти наверняка (вероятность обратного ~ 1e-45).
    const Characteristic_hard t("Strength", 10);
    for (checkmode mode : {checkmode::normal, checkmode::advantage, checkmode::disadvantage}){
        int successes = 0;
        for (int i = 0; i < 2000; i++){
            if (t("Strength", mode, 1000)) successes++;
        }
        EXPECT_GT(successes, 0);
        EXPECT_LT(successes, 2000);
    }
}

TEST(HardCheck, CriticalFailHappens){
    // Значение 20, сложность 1: провалить можно только критическим провалом (1).
    const Characteristic_hard t("Strength", 20);
    for (checkmode mode : {checkmode::normal, checkmode::advantage, checkmode::disadvantage}){
        int fails = 0;
        for (int i = 0; i < 20000; i++){
            if (!t("Strength", mode, 1)) fails++;
        }
        EXPECT_GT(fails, 0);
        EXPECT_LT(fails, 20000);
    }
}

TEST(HardCheck, AdvantageBetterThanDisadvantage){
    // При преимуществе успехов заметно больше, чем при помехе.
    // Ожидаемо ~7500 против ~2500 из 10000, разрыв огромный.
    const Characteristic_hard t("Strength", 10);
    int adv = 0, dis = 0;
    for (int i = 0; i < 10000; i++){
        if (t("Strength", checkmode::advantage, 11)) adv++;
        if (t("Strength", checkmode::disadvantage, 11)) dis++;
    }
    EXPECT_GT(adv, dis);
}

TEST(HardCheck, UsesValueFromTable){
    // После изменения через [] проверка использует новое значение:
    // со значением 1 (модификатор -5) сложность 16 проходится только на 20,
    // со значением 20 (модификатор +5) сложность 16 проходится с 11 и выше.
    Characteristic_hard t("Strength", 1);
    int before = 0;
    for (int i = 0; i < 5000; i++) if (t("Strength", checkmode::normal, 16)) before++;

    t["Strength"].set_value(20);
    int after = 0;
    for (int i = 0; i < 5000; i++) if (t("Strength", checkmode::normal, 16)) after++;

    EXPECT_GT(after, before);
}

// ===================== Сортировка =====================

TEST(HardSort, AscendingByDefault){
    Characteristic_hard t({"Wisdom", "Charisma", "Strength", "Dexterity"});
    t.sort_by_name();
    EXPECT_EQ(to_text(t), "4\n   Charisma 1\n   Dexterity 1\n   Strength 1\n   Wisdom 1");
}

TEST(HardSort, Descending){
    Characteristic_hard t({"Wisdom", "Charisma", "Strength", "Dexterity"});
    t.sort_by_name(true);
    EXPECT_EQ(to_text(t), "4\n   Wisdom 1\n   Strength 1\n   Dexterity 1\n   Charisma 1");
}

TEST(HardSort, KeepsValuesWithNames){
    Characteristic_hard t;
    t += Characteristic_simple("B", 2);
    t += Characteristic_simple("C", 3);
    t += Characteristic_simple("A", 1);
    t.sort_by_name();
    EXPECT_EQ(to_text(t), "3\n   A 1\n   B 2\n   C 3");
    EXPECT_EQ(t["C"].get_value(), 3);
}

TEST(HardSort, EmptyAndSingle){
    Characteristic_hard empty;
    EXPECT_NO_THROW(empty.sort_by_name());
    EXPECT_EQ(to_text(empty), "0");

    Characteristic_hard one("A", 5);
    one.sort_by_name(true);
    EXPECT_EQ(to_text(one), "1\n   A 5");
}

TEST(HardSort, UppercaseBeforeLowercase){
    // Сравнение строк по кодам символов: 'Z' (90) < 'a' (97)
    Characteristic_hard t({"apple", "Zebra"});
    t.sort_by_name();
    EXPECT_EQ(to_text(t), "2\n   Zebra 1\n   apple 1");
}

// ===================== Поиск максимума =====================

TEST(HardSearchMax, FindsMaximum){
    Characteristic_hard t;
    t += Characteristic_simple("Strength", 15);
    t += Characteristic_simple("Dexterity", 12);
    t += Characteristic_simple("Wisdom", 18);
    EXPECT_EQ(t.search_max_in_array({"Strength", "Dexterity", "Wisdom"}).get_name(), "Wisdom");
}

TEST(HardSearchMax, OnlyAmongGivenNames){
    Characteristic_hard t;
    t += Characteristic_simple("Strength", 18);
    t += Characteristic_simple("Dexterity", 12);
    t += Characteristic_simple("Wisdom", 15);
    // Strength больше всех, но его нет в списке
    EXPECT_EQ(t.search_max_in_array({"Dexterity", "Wisdom"}).get_name(), "Wisdom");
}

TEST(HardSearchMax, IgnoresMissingNames){
    Characteristic_hard t;
    t += Characteristic_simple("Strength", 10);
    EXPECT_EQ(t.search_max_in_array({"Nope", "Strength", "AlsoNope"}).get_name(), "Strength");
}

TEST(HardSearchMax, TieReturnsFirstInList){
    Characteristic_hard t;
    t += Characteristic_simple("A", 10);
    t += Characteristic_simple("B", 10);
    EXPECT_EQ(t.search_max_in_array({"B", "A"}).get_name(), "B");
    EXPECT_EQ(t.search_max_in_array({"A", "B"}).get_name(), "A");
}

TEST(HardSearchMax, BoundaryValues){
    Characteristic_hard t;
    t += Characteristic_simple("Min", 1);
    EXPECT_EQ(t.search_max_in_array({"Min"}).get_value(), 1);
    t += Characteristic_simple("Max", 20);
    EXPECT_EQ(t.search_max_in_array({"Min", "Max"}).get_name(), "Max");
}

TEST(HardSearchMax, DuplicateNamesInList){
    Characteristic_hard t;
    t += Characteristic_simple("A", 5);
    EXPECT_EQ(t.search_max_in_array({"A", "A", "A"}).get_name(), "A");
}

TEST(HardSearchMax, NothingFoundThrows){
    Characteristic_hard t("Strength", 10);
    EXPECT_THROW(t.search_max_in_array({"Nope"}), std::out_of_range);
    EXPECT_THROW(t.search_max_in_array({}), std::out_of_range);

    Characteristic_hard empty;
    EXPECT_THROW(empty.search_max_in_array({"Strength"}), std::out_of_range);
}

TEST(HardSearchMax, ReturnsReferenceIntoTable){
    Characteristic_hard t;
    t += Characteristic_simple("A", 5);
    t += Characteristic_simple("B", 10);
    t.search_max_in_array({"A", "B"}).set_value(2);
    EXPECT_EQ(t["B"].get_value(), 2);
}

// ===================== Правило пяти =====================

TEST(HardRuleOfFive, CopyConstructorIsDeep){
    Characteristic_hard original({"A", "B"});
    Characteristic_hard copy(original);
    copy["A"].set_value(20);
    copy += Characteristic_simple("C", 5);

    EXPECT_EQ(original["A"].get_value(), 1);
    EXPECT_EQ(size_of(original), 2u);
    EXPECT_EQ(size_of(copy), 3u);
}

TEST(HardRuleOfFive, CopyOfEmptyTable){
    Characteristic_hard empty;
    Characteristic_hard copy(empty);
    EXPECT_EQ(size_of(copy), 0u);
    copy += Characteristic_simple("A", 1);
    EXPECT_EQ(size_of(copy), 1u);
    EXPECT_EQ(size_of(empty), 0u);
}

TEST(HardRuleOfFive, CopyAssignmentIsDeep){
    Characteristic_hard a({"A", "B"});
    Characteristic_hard b("Old", 7);
    b = a;
    EXPECT_EQ(to_text(b), to_text(a));
    EXPECT_THROW(b["Old"], std::out_of_range);   // старое содержимое заменено

    b["A"].set_value(20);
    EXPECT_EQ(a["A"].get_value(), 1);
}

TEST(HardRuleOfFive, CopySelfAssignment){
    Characteristic_hard t({"A", "B"});
    Characteristic_hard& alias = t;
    t = alias;
    EXPECT_EQ(size_of(t), 2u);
    EXPECT_EQ(t["A"].get_value(), 1);
}

TEST(HardRuleOfFive, CopyAssignmentReturnsSelf){
    Characteristic_hard a("A", 1), b;
    EXPECT_EQ(&(b = a), &b);
}

TEST(HardRuleOfFive, MoveConstructorTakesData){
    Characteristic_hard source({"A", "B"});
    std::string before = to_text(source);
    Characteristic_hard moved(std::move(source));
    EXPECT_EQ(to_text(moved), before);
}

TEST(HardRuleOfFive, MovedFromIsEmptyAndUsable){
    Characteristic_hard source({"A", "B"});
    Characteristic_hard moved(std::move(source));
    // После перемещения таблица пустая, capacity == 0,
    // но в неё можно добавлять (проверяет upgrade_data при capacity == 0)
    EXPECT_EQ(size_of(source), 0u);
    EXPECT_THROW(source["A"], std::out_of_range);
    source += Characteristic_simple("New", 5);
    source += Characteristic_simple("New2", 6);
    EXPECT_EQ(size_of(source), 2u);
    EXPECT_EQ(source["New2"].get_value(), 6);
}

TEST(HardRuleOfFive, MovedFromCanBeCopiedAndSorted){
    Characteristic_hard source("A", 1);
    Characteristic_hard moved(std::move(source));
    Characteristic_hard copy(source);            // копия пустой таблицы с data == nullptr
    EXPECT_EQ(size_of(copy), 0u);
    EXPECT_NO_THROW(source.sort_by_name());
    EXPECT_THROW(source.search_max_in_array({"A"}), std::out_of_range);
}

TEST(HardRuleOfFive, MoveAssignmentTakesData){
    Characteristic_hard source({"A", "B"});
    std::string before = to_text(source);
    Characteristic_hard target("Old", 7);
    target = std::move(source);
    EXPECT_EQ(to_text(target), before);
    EXPECT_THROW(target["Old"], std::out_of_range);
    EXPECT_EQ(size_of(source), 0u);
}

TEST(HardRuleOfFive, MoveSelfAssignment){
    Characteristic_hard t({"A", "B"});
    Characteristic_hard& alias = t;
    t = std::move(alias);
    EXPECT_EQ(size_of(t), 2u);
    EXPECT_EQ(t["B"].get_value(), 1);
}

TEST(HardRuleOfFive, MoveAssignmentReturnsSelf){
    Characteristic_hard a("A", 1), b;
    EXPECT_EQ(&(b = std::move(a)), &b);
}

TEST(HardRuleOfFive, MoveOperationsAreNoexcept){
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<Characteristic_hard>);
    EXPECT_TRUE(std::is_nothrow_move_assignable_v<Characteristic_hard>);
}

TEST(HardRuleOfFive, WorksInVector){
    // vector сам копирует и перемещает элементы при росте
    std::vector<Characteristic_hard> tables;
    for (int i = 0; i < 20; i++){
        tables.emplace_back("N" + std::to_string(i), (i % 20) + 1);
    }
    std::vector<Characteristic_hard> copy = tables;
    EXPECT_EQ(copy[19]["N19"].get_value(), 20);
}

// ===================== Потоки =====================

TEST(HardStream, OutputFormat){
    Characteristic_hard t;
    t += Characteristic_simple("Strength", 15);
    t += Characteristic_simple("Dexterity", 12);
    EXPECT_EQ(to_text(t), "2\n   Strength 15\n   Dexterity 12");
}

TEST(HardStream, InputValid){
    std::istringstream in("2\nStrength 15\nDexterity 12");
    Characteristic_hard t;
    in >> t;
    EXPECT_FALSE(in.fail());
    EXPECT_EQ(size_of(t), 2u);
    EXPECT_EQ(t["Strength"].get_value(), 15);
    EXPECT_EQ(t["Dexterity"].get_value(), 12);
}

TEST(HardStream, InputOnOneLine){
    std::istringstream in("2 Strength 15 Dexterity 12");
    Characteristic_hard t;
    in >> t;
    EXPECT_FALSE(in.fail());
    EXPECT_EQ(size_of(t), 2u);
}

TEST(HardStream, InputReplacesOldContent){
    std::istringstream in("1 New 5");
    Characteristic_hard t({"Old1", "Old2"});
    in >> t;
    EXPECT_FALSE(in.fail());
    EXPECT_EQ(size_of(t), 1u);
    EXPECT_THROW(t["Old1"], std::out_of_range);
}

TEST(HardStream, InputZeroCountGivesEmptyTable){
    std::istringstream in("0");
    Characteristic_hard t("Old", 5);
    in >> t;
    EXPECT_FALSE(in.fail());
    EXPECT_EQ(size_of(t), 0u);
}

TEST(HardStream, InputDuplicateFailsAndKeepsTable){
    std::istringstream in("2 A 5 A 6");
    Characteristic_hard t("Keep", 3);
    in >> t;
    EXPECT_TRUE(in.fail());
    EXPECT_EQ(to_text(t), "1\n   Keep 3");
}

TEST(HardStream, InputBadCountFailsAndKeepsTable){
    for (const char* text : {"abc", "", "-1 A 5"}){
        std::istringstream in(text);
        Characteristic_hard t("Keep", 3);
        in >> t;
        EXPECT_TRUE(in.fail()) << '"' << text << '"';
        EXPECT_EQ(to_text(t), "1\n   Keep 3") << '"' << text << '"';
    }
}

TEST(HardStream, InputBadCharacteristicFailsAndKeepsTable){
    for (const char* text : {"2 A 5 B 25", "2 A 5 B x", "2 A 0 B 5"}){
        std::istringstream in(text);
        Characteristic_hard t("Keep", 3);
        in >> t;
        EXPECT_TRUE(in.fail()) << text;
        EXPECT_EQ(to_text(t), "1\n   Keep 3") << text;
    }
}

TEST(HardStream, InputTooFewCharacteristicsFails){
    std::istringstream in("3 A 5 B 6");
    Characteristic_hard t("Keep", 3);
    in >> t;
    EXPECT_TRUE(in.fail());
    EXPECT_EQ(to_text(t), "1\n   Keep 3");
}

TEST(HardStream, RoundTrip){
    Characteristic_hard original;
    for (int i = 0; i < 15; i++){
        original += Characteristic_simple("N" + std::to_string(i), (i % 20) + 1);
    }
    std::stringstream ss;
    ss << original;
    Characteristic_hard restored;
    ss >> restored;
    EXPECT_FALSE(ss.fail());
    EXPECT_EQ(to_text(restored), to_text(original));
}

TEST(HardStream, RoundTripEmpty){
    Characteristic_hard original;
    std::stringstream ss;
    ss << original;
    Characteristic_hard restored("Old", 1);
    ss >> restored;
    EXPECT_FALSE(ss.fail());
    EXPECT_EQ(size_of(restored), 0u);
}