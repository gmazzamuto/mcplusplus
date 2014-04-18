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
QT4_WRAP_CPP(OutputFileViewer_HEADERS_MOC outputfileviewer.h)

add_executable(sourceViewer sourceviewer.cpp mainwindow.cpp glwidget.cpp ${sourceViewer_HEADERS_MOC} ${GLWidget_HEADERS_MOC})
target_link_libraries(sourceViewer MCPlusPlus GL ${QT_LIBRARIES})
install (TARGETS sourceViewer DESTINATION bin)

add_executable(MCPPSimulation mcppsimulation.cpp glwidget.cpp simulationviewer.cpp ${GLWidget_HEADERS_MOC} ${SimulationViewer_HEADERS_MOC})
target_link_libraries(MCPPSimulation MCPlusPlus GL ${QT_LIBRARIES})
install (TARGETS MCPPSimulation DESTINATION bin)

add_executable(gaussianBeamTransformation gaussianBeamTransformation.cpp glwidget.cpp simulationviewer.cpp ${GLWidget_HEADERS_MOC} ${SimulationViewer_HEADERS_MOC})
target_link_libraries(gaussianBeamTransformation MCPlusPlus GL ${QT_LIBRARIES})
install (TARGETS gaussianBeamTransformation DESTINATION bin)

add_executable(mcpp mcpp.cpp glwidget.cpp simulationviewer.cpp ${GLWidget_HEADERS_MOC} ${SimulationViewer_HEADERS_MOC})
target_link_libraries(mcpp MCPlusPlus GL ${QT_LIBRARIES})
install (TARGETS mcpp DESTINATION bin)

add_executable(mcplot mcplot.cpp glwidget.cpp outputfileviewer.cpp ${GLWidget_HEADERS_MOC} ${OutputFileViewer_HEADERS_MOC})
target_link_libraries(mcplot MCPlusPlus GL ${QT_LIBRARIES})
install (TARGETS mcplot DESTINATION bin)

add_executable(mcwalktimehist mcwalktimehist.cpp)
target_link_libraries(mcwalktimehist MCPlusPlus)
install (TARGETS mcwalktimehist DESTINATION bin)

add_executable(mcexitpointsradialhist mcexitpointsradialhist.cpp)
target_link_libraries(mcexitpointsradialhist MCPlusPlus)
install (TARGETS mcexitpointsradialhist DESTINATION bin)
