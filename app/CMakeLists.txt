cmake_minimum_required(VERSION 3.16)
project(pteapp VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui Multimedia Network PrintSupport Sql Svg Widgets Xml)

qt_standard_project_setup()

qt_add_library(pteapp STATIC
    PtePlayer.cpp PtePlayer.h PtePlayer.ui
    ScoreFileLoader.cpp ScoreFileLoader.h
    SettingDialog.cpp SettingDialog.h SettingDialog.ui
    TickPlayer.cpp TickPlayer.h
    appinfo.cpp appinfo.h
    caret.cpp caret.h
    clipboard.cpp clipboard.h
    command.cpp command.h
    documentmanager.cpp documentmanager.h
    paths.cpp paths.h
    powertabeditor.cpp powertabeditor.h
    recentfiles.cpp recentfiles.h
    scorearea.cpp scorearea.h
    settings.cpp settings.h
    settingsmanager.cpp settingsmanager.h
    tuningdictionary.cpp tuningdictionary.h
    viewoptions.cpp viewoptions.h
)

target_include_directories(pteapp PUBLIC
    ..
    ../third_part
    ../third_part/minizip-tools
    ../third_part/pugi-xml
    ${BOOST_INCLUDE_PATH}
)

target_link_libraries(pteapp PUBLIC
    # Remove: L../libs/
    Qt::Concurrent
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::Network
    Qt::PrintSupport
    Qt::Sql
    Qt::Svg
    Qt::Widgets
    Qt::Xml
    ptactions
)

install(TARGETS pteapp
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
