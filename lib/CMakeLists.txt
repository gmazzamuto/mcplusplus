cmake_minimum_required (VERSION 2.8)

project(libMCPlusPlus)

FIND_PACKAGE(Boost REQUIRED thread system)
SET(Boost_USE_MULTITHREAD ON)
include_directories( ${Boost_INCLUDE_DIRS} )

FIND_PACKAGE(HDF5 REQUIRED CXX)
include_directories(${HDF5_INCLUDE_DIRS})

option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)

if(BUILD_PYTHON_BINDINGS)
  FIND_PACKAGE(SWIG)
  if(NOT SWIG_FOUND)
    message(FATAL_ERROR
      "SWIG is needed to build the Python bindings. Please install it correctly")
  endif()

  FIND_PACKAGE(PythonLibs)
  if(NOT PYTHONLIBS_FOUND)
    message(FATAL_ERROR
      "Cannot find PythonLibs, unable to build Python bindings.")
  endif()

    add_custom_command(OUTPUT libMCPlusPlus_wrap.cxx
                    COMMAND ${SWIG_EXECUTABLE} -c++ -Wall -python -outcurrentdir -I${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/libMCPlusPlus.i DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libMCPlusPlus.i ${HEADERS})
    include_directories(${PYTHON_INCLUDE_DIRS})
    add_library(pymcplusplus SHARED ${PROJECT_BINARY_DIR}/libMCPlusPlus_wrap.cxx)
    set_target_properties(pymcplusplus PROPERTIES PREFIX "_")
    target_link_libraries(pymcplusplus MCPlusPlus)
    install (TARGETS pymcplusplus DESTINATION lib)
    install(FILES ${PROJECT_BINARY_DIR}/pymcplusplus.py DESTINATION lib)
endif()


include_directories(include)
file(GLOB SRC . *.cpp)
file(GLOB HEADERS include/MCPlusPlus/*.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")

set(LIB_TYPE "STATIC")
if(SHARED_LIB)
   set(LIB_TYPE "SHARED")
endif()

if(ENABLE_TRAJECTORY)
    ADD_DEFINITIONS(-DENABLE_TRAJECTORY)
endif()

add_library(MCPlusPlus ${LIB_TYPE} ${SRC} ${HEADERS})
target_link_libraries(MCPlusPlus ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${HDF5_LIBRARIES})
install (TARGETS MCPlusPlus DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include/MCPlusPlus)


