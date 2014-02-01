cmake_minimum_required (VERSION 2.8)

project(MCPlusPlus)
find_package( Boost REQUIRED COMPONENTS random )
include_directories( ${Boost_INCLUDE_DIRS} )

FIND_PACKAGE(Qt4)
SET(QT_USE_QTCORE TRUE)
SET(QT_USE_QTGUI TRUE)
SET(QT_USE_QTOPENGL TRUE)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

include_directories(.)
file(GLOB SRC . *.cpp)
file(GLOB HEADERS . *.h)

add_library(MCPlusPlus SHARED ${SRC})
install (TARGETS MCPlusPlus DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)
