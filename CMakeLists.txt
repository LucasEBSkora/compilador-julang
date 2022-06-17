cmake_minimum_required(VERSION 3.18.0)
project(Julang-Compiler VERSION 0.1.0)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

bison_target(parser
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.y 
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp
    COMPILE_FLAGS "-Wcounterexamples"
)

flex_target(lexer 
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l 
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp
)

add_flex_bison_dependency(lexer parser)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/statements)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/expressions)


add_executable(Julang-Compiler
    ${CMAKE_SOURCE_DIR}/parser.cpp
    ${CMAKE_SOURCE_DIR}/scanner.cpp
    ${CMAKE_SOURCE_DIR}/interpreter.cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/Compiler.cpp
    ${CMAKE_SOURCE_DIR}/statements/AssignmentStatement.cpp
    ${CMAKE_SOURCE_DIR}/statements/DeclareScalarStatement.cpp
    ${CMAKE_SOURCE_DIR}/statements/IfStatement.cpp
    ${CMAKE_SOURCE_DIR}/statements/DeclareArrayStatement.cpp
    ${CMAKE_SOURCE_DIR}/statements/DeleteStatement.cpp
    ${CMAKE_SOURCE_DIR}/statements/WhileStatement.cpp
    ${CMAKE_SOURCE_DIR}/expressions/expression.cpp
)

set_target_properties(Julang-Compiler PROPERTIES 
    CXX_STANDARD 20
    OUTPUT_NAME julang
)
