cmake_minimum_required (VERSION 2.8)

include_directories(. ../lib)

add_executable(testCounters testCounters.cpp tests.cpp)
target_link_libraries(testCounters MCPlusPlus)

add_test(NAME "testCounters" COMMAND testCounters)
set_tests_properties(testCounters PROPERTIES PASS_REGULAR_EXPRESSION "testCounters PASSED")

add_executable(testHistogram testHistogram.cpp tests.cpp)
target_link_libraries(testHistogram MCPlusPlus)

add_test(NAME "testHistogram" COMMAND testHistogram)
set_tests_properties(testHistogram PROPERTIES PASS_REGULAR_EXPRESSION "testHistogram PASSED")
