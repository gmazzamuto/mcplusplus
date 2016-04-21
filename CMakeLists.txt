cmake_minimum_required (VERSION 2.8)

project(MCPlusPlus)
enable_testing()

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

#-- Add an Option to toggle the generation of the documentation
option(ENABLE_DOCUMENTATION "Use Doxygen to create the HTML based documentation" OFF)
if(ENABLE_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
endif()

option(SHARED_LIB "Build shared instead of static library" OFF)
option(ENABLE_TRAJECTORY "Enable saving of trajectory points" OFF)
if(ENABLE_TRAJECTORY)
    message(STATUS "Saving of trajectories enabled")
endif()

set(DOCUMENTATION_DIR ${CMAKE_INSTALL_PREFIX}/share/doc/MCPlusPlus)
set(LIBSRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

add_subdirectory(doc)
add_subdirectory(lib)
add_subdirectory(tests)

add_custom_target(documentation)
add_dependencies(documentation main-documentation libMCPlusPlus-documentation)

add_custom_target(run_install COMMAND make install)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
