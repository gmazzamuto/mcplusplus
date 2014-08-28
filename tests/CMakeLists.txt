cmake_minimum_required (VERSION 2.8)

include_directories(. ../lib)

add_executable(testBilayer testBilayer.cpp tests.cpp)
target_link_libraries(testBilayer MCPlusPlus)

add_test(NAME "testBilayer" COMMAND testBilayer)
set_tests_properties(testBilayer PROPERTIES PASS_REGULAR_EXPRESSION "testBilayer PASSED")

add_executable(testH5Append testH5Append.cpp tests.cpp)
target_link_libraries(testH5Append MCPlusPlus)

add_test(NAME "testH5Append" COMMAND testH5Append)
set_tests_properties(testH5Append PROPERTIES PASS_REGULAR_EXPRESSION "testH5Append PASSED")
