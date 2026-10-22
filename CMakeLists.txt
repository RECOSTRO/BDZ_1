cmake_minimum_required(VERSION 3.20)
project(Characteristics CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------- статическая библиотека с классами ----------
add_library(characteristics STATIC
    lib/Characteristic_simple.cpp
    lib/Characteristic_hard.cpp
)
target_include_directories(characteristics PUBLIC lib)

# ---------- консольное приложение (диалог) ----------
# Раскомментируй, когда диалог будет готов
# add_executable(dialog
#     app/main.cpp
#     app/app.cpp
# )
# target_link_libraries(dialog PRIVATE characteristics)

# ---------- модульные тесты ----------
enable_testing()
add_subdirectory(tests)