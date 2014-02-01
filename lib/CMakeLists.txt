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
