cmake_minimum_required(VERSION 3.1)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(main CXX)

add_executable(main main.cpp)

find_package(Boost REQUIRED)
add_definitions(-DBOOST_ALL_NO_LIB)

link_directories(${Boost_LIBRARY_DIRS})
target_include_directories(main PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(main ${Boost_LIBRARIES})

