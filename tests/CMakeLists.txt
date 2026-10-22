include(FetchContent)

# Скачивает GoogleTest с GitHub при первой сборке
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Для Windows/MSVC: одинаковая runtime-библиотека у gtest и у тестов
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests
    test_simple.cpp
    test_hard.cpp
)
target_link_libraries(tests PRIVATE characteristics GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)