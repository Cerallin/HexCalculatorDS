cmake_minimum_required(VERSION 3.18)
project(HexCalculatorDS-test LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set(CMAKE_C_FLAGS "-g -Wall -O0 -fsanitize=undefined")
set(CMAKE_CXX_FLAGS "-g -Wall -O0 -fsanitize=undefined")

include(FetchContent)

# ===========================================
# CppUTest
# ===========================================

FetchContent_Declare(
    cpputest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG v4.0
)

# Disable examples
set(TESTS OFF CACHE BOOL "" FORCE)
set(EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cpputest)

# ===========================================
# test formula tree
# ===========================================

add_executable(test_formula_tree
    test_formula_tree.cc
)

target_include_directories(test_formula_tree PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../source
)

target_link_libraries(test_formula_tree PRIVATE
    CppUTest
    CppUTestExt
)

add_test(NAME test_formula_tree COMMAND test_formula_tree)
