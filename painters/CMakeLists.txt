cmake_minimum_required(VERSION 3.16)
project(ptepainters VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui Network PrintSupport Sql Svg Widgets Xml)

qt_standard_project_setup()

qt_add_library(ptepainters STATIC
    antialiasedpathitem.cpp antialiasedpathitem.h
    barlinepainter.cpp barlinepainter.h
    beamgroup.cpp beamgroup.h
    caretpainter.cpp caretpainter.h
    chorddiagrampainter.cpp chorddiagrampainter.h
    clickableitem.cpp clickableitem.h
    directions.cpp
    keysignaturepainter.cpp keysignaturepainter.h
    layoutinfo.cpp layoutinfo.h
    musicfont.cpp musicfont.h
    notestem.cpp notestem.h
    scoreclickevent.h
    scoreinforenderer.cpp scoreinforenderer.h
    simpletextitem.cpp simpletextitem.h
    staffpainter.cpp staffpainter.h
    stdnotationnote.cpp stdnotationnote.h
    styles.cpp styles.h
    systemrenderer.cpp systemrenderer.h
    timesignaturepainter.cpp timesignaturepainter.h
    verticallayout.cpp verticallayout.h
)

target_include_directories(ptepainters PUBLIC
    ..
    ../third_part
    ../third_part/minizip-tools
    ../third_part/pugi-xml
    ${BOOST_INCLUDE_PATH}
)

target_link_libraries(ptepainters PUBLIC
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

install(TARGETS ptepainters
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
