cmake_minimum_required (VERSION 2.8)

project(MCPlusPlus)

FIND_PACKAGE(Qt4)
SET(QT_USE_QTCORE TRUE)
SET(QT_USE_QTGUI TRUE)
SET(QT_USE_QTOPENGL TRUE)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

include_directories(. ../lib)

QT4_WRAP_CPP(GLWidget_HEADERS_MOC glwidget.h)
QT4_WRAP_CPP(SimulationViewer_HEADERS_MOC simulationviewer.h)
QT4_WRAP_CPP(sourceViewer_HEADERS_MOC mainwindow.h)

add_executable(sourceViewer sourceviewer.cpp mainwindow.cpp glwidget.cpp ${sourceViewer_HEADERS_MOC} ${GLWidget_HEADERS_MOC})
target_link_libraries(sourceViewer MCPlusPlus GL ${QT_LIBRARIES})
install (TARGETS sourceViewer DESTINATION bin)

add_executable(MCPPSimulation mcppsimulation.cpp glwidget.cpp simulationviewer.cpp ${GLWidget_HEADERS_MOC} ${SimulationViewer_HEADERS_MOC})
target_link_libraries(MCPPSimulation MCPlusPlus GL ${QT_LIBRARIES})
install (TARGETS MCPPSimulation DESTINATION bin)
