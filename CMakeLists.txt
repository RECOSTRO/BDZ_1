cmake_minimum_required(VERSION 3.20)
project(Characteristics CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
endif()

#--------- biblioteca of characteristic -----------
add_library(characteristics STATIC
    lib/Characteristic_simple.cpp
    lib/Characteristic_hard.cpp
)
target_include_directories(characteristics PUBLIC lib)

#---------- application ---------
add_executable(dialog
    app/main.cpp
    app/app.cpp
)
target_link_libraries(dialog PRIVATE characteristics)

# ---------- tests --------
enable_testing()
add_subdirectory(tests)