cmake_minimum_required (VERSION 2.8)

project(libMCPlusPlus)
FIND_PACKAGE(Boost REQUIRED thread)
SET(Boost_USE_MULTITHREAD ON)
include_directories( ${Boost_INCLUDE_DIRS} )

include_directories(.)
file(GLOB SRC . *.cpp)
file(GLOB HEADERS . *.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_library(MCPlusPlus SHARED ${SRC} ${HEADERS})
target_link_libraries(MCPlusPlus ${Boost_THREAD_LIBRARY})
install (TARGETS MCPlusPlus DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include/MCPlusPlus)

if(DOXYGEN_FOUND)
    #-- Configure the Template Doxyfile for our specific project
    set(DOXYGEN_OUTDIR ${DOCUMENTATION_DIR}/libMCPlusPlus)
    configure_file(Doxyfile.in
                   ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY)
    add_custom_target(libMCPlusPlus-documentation SOURCES DoxygenLayout.xml
                      COMMAND  mkdir -p  ${DOXYGEN_OUTDIR} && ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
