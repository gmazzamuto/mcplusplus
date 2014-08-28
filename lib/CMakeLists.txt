cmake_minimum_required (VERSION 2.8)

project(libMCPlusPlus)
FIND_PACKAGE(Boost REQUIRED thread system)
SET(Boost_USE_MULTITHREAD ON)
include_directories( ${Boost_INCLUDE_DIRS} )

find_library(LIBHDF5 hdf5)
find_library(LIBHDF5CPP hdf5_cpp)

include_directories(.)
include_directories(/usr/include/python2.7/)
file(GLOB SRC . *.cpp)
file(GLOB HEADERS . *.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(LIB_TYPE "STATIC")
if(SHARED_LIB)
   set(LIB_TYPE "SHARED")
endif()

add_library(MCPlusPlus ${LIB_TYPE} ${SRC} ${HEADERS})
add_library(pymcplusplus SHARED ${SRC} ${HEADERS} libMCPlusPlus_wrap.cxx)
set_target_properties(pymcplusplus PROPERTIES PREFIX "_")

target_link_libraries(MCPlusPlus ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${LIBHDF5} ${LIBHDF5CPP})
install (TARGETS MCPlusPlus DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include/MCPlusPlus)

target_link_libraries(pymcplusplus ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${LIBHDF5} ${LIBHDF5CPP})
install (TARGETS pymcplusplus DESTINATION lib)

if(DOXYGEN_FOUND)
    #-- Configure the Template Doxyfile for our specific project
    set(DOXYGEN_OUTDIR ${DOCUMENTATION_DIR}/libMCPlusPlus)
    configure_file(Doxyfile.in
                   ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY)
    add_custom_target(libMCPlusPlus-documentation SOURCES DoxygenLayout.xml DEPENDS run_install
                      COMMAND mkdir -p  ${DOXYGEN_OUTDIR} && ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
