cmake_minimum_required (VERSION 2.8)

project(MCPlusPlus)

include_directories(. ../lib)

add_executable(MCPPSimulation mcppsimulation.cpp)
target_link_libraries(MCPPSimulation MCPlusPlus)
install (TARGETS MCPPSimulation DESTINATION bin)

