cmake_minimum_required (VERSION 2.8)

include_directories(. ../lib)

add_executable(testBilayer testBilayer.cpp)
target_link_libraries(testBilayer MCPlusPlus)

add_test(NAME "testBilayer" COMMAND testBilayer)
set_tests_properties(testBilayer PROPERTIES PASS_REGULAR_EXPRESSION "testBilayer PASSED")
