cmake_minimum_required(VERSION 3.16)
project(ptemidi VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)

qt_standard_project_setup()

qt_add_library(ptemidi STATIC
    midievent.cpp midievent.h
    midieventlist.cpp midieventlist.h
    midifile.cpp midifile.h
    repeatcontroller.cpp repeatcontroller.h
)
target_include_directories(ptemidi PUBLIC
    ..
    ../third_part
    ${BOOST_INCLUDE_PATH}
)

target_link_libraries(ptemidi PUBLIC
    Qt::Core
    Qt::Gui
)

install(TARGETS ptemidi
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
