cmake_minimum_required(VERSION 3.16)
project(ptaudio VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui Network PrintSupport Sql Svg Widgets Xml)

qt_standard_project_setup()

qt_add_library(ptaudio STATIC
    midioutputdevice.cpp midioutputdevice.h
    midiplayer.cpp midiplayer.h
    settings.cpp
)
target_include_directories(ptaudio PUBLIC
    ..
    ../third_part
    ${BOOST_INCLUDE_PATH}
    # ../third_part/rtmidi-6.0.0
)

target_link_libraries(ptaudio PUBLIC
    Qt::Concurrent
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::PrintSupport
    Qt::Sql
    Qt::Svg
    Qt::Widgets
    Qt::Xml
)

install(TARGETS ptaudio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
