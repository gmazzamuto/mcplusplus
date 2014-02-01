cmake_minimum_required (VERSION 2.8)

project(MCPlusPlus)
FIND_PACKAGE(Qt4)
SET(QT_USE_QTCORE TRUE)
SET(QT_USE_QTGUI TRUE)
SET(QT_USE_QTOPENGL TRUE)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

include_directories(.)

file(GLOB SRC . *.cpp)
QT4_WRAP_CPP(sourceViewer_HEADERS_MOC mainwindow.h glwidget.h)

add_executable(sourceViewer ${SRC} ${sourceViewer_HEADERS_MOC})
TARGET_LINK_LIBRARIES(sourceViewer ${QT_LIBRARIES})
install (TARGETS sourceViewer DESTINATION bin)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
