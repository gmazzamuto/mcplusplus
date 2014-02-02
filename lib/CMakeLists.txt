cmake_minimum_required (VERSION 2.8)

project(MCPlusPlus)
find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )

include_directories(.)
file(GLOB SRC . *.cpp)
file(GLOB HEADERS . *.h)

add_library(MCPlusPlus SHARED ${SRC})
install (TARGETS MCPlusPlus DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)

#-- Add an Option to toggle the generation of the API documentation
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if(BUILD_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  #-- Configure the Template Doxyfile for our specific project
  set(DOXYGEN_OUTDIR ${CMAKE_INSTALL_PREFIX}/share/doc/libMCPlusPlus)
  configure_file(Doxyfile.in
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY)
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (documentation
                     COMMAND  mkdir -p  ${DOXYGEN_OUTDIR} && ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
