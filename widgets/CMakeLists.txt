cmake_minimum_required(VERSION 3.16)
project(ptewidgets VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui Network PrintSupport Sql Svg Widgets Xml)

qt_standard_project_setup()

qt_add_library(ptewidgets STATIC
    clickablelabel.cpp clickablelabel.h
    common.cpp common.h
    instruments/instrumentpanel.cpp instruments/instrumentpanel.h
    instruments/instrumentpanelitem.cpp instruments/instrumentpanelitem.h instruments/instrumentpanelitem.ui
    mixer/mixer.cpp mixer/mixer.h
    mixer/mixeritem.cpp mixer/mixeritem.h mixer/mixeritem.ui
    playback/playbackwidget.cpp playback/playbackwidget.h playback/playbackwidget.ui
    toolbox/toolbox.ui toolbox/toolbox.h toolbox/toolbox.cpp
)

target_include_directories(ptewidgets PUBLIC
    ..
    ../third_part
    ../third_part/minizip-tools
    ../third_part/pugi-xml
    ${BOOST_INCLUDE_PATH}
)

target_link_libraries(ptewidgets PUBLIC
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

install(TARGETS ptewidgets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
