cmake_minimum_required(VERSION 3.16)
project(ptescore VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)

qt_standard_project_setup()

qt_add_library(ptescore STATIC
    alternateending.cpp alternateending.h
    barline.cpp barline.h
    chorddiagram.cpp chorddiagram.h
    chordname.cpp chordname.h
    chordtext.cpp chordtext.h
    direction.cpp direction.h
    dynamic.cpp dynamic.h
    fileversion.h
    generalmidi.cpp generalmidi.h
    instrument.cpp instrument.h
    irregulargrouping.cpp irregulargrouping.h
    keysignature.cpp keysignature.h
    note.cpp note.h
    player.cpp player.h
    playerchange.cpp playerchange.h
    position.cpp position.h
    precompiled.h
    rehearsalsign.cpp rehearsalsign.h
    score.cpp score.h
    scoreinfo.cpp scoreinfo.h
    scorelocation.cpp scorelocation.h
    serialization.cpp serialization.h
    staff.cpp staff.h
    system.cpp system.h
    systemlocation.cpp systemlocation.h
    tempomarker.cpp tempomarker.h
    textitem.cpp textitem.h
    timesignature.cpp timesignature.h
    tuning.cpp tuning.h
    utils.h
    utils/directionindex.cpp utils/directionindex.h
    utils/repeatindexer.cpp utils/repeatindexer.h
    utils/scoremerger.cpp utils/scoremerger.h
    utils/scorepolisher.cpp utils/scorepolisher.h
    viewfilter.cpp viewfilter.h
    voice.cpp voice.h
    voiceutils.cpp voiceutils.h
)

target_include_directories(ptescore PUBLIC
    ..
    ../third_part
    ${BOOST_INCLUDE_PATH}
)

target_link_libraries(ptescore PUBLIC
    Qt::Core
    Qt::Gui
)

install(TARGETS ptescore
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
