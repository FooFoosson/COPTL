cmake_minimum_required(VERSION 2.8)

project(coptl LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_library(coptl SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp)

set_target_properties(coptl PROPERTIES CXX_STANDARD 11) 

add_executable(coptl_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp)
target_link_libraries(coptl_test coptl)
