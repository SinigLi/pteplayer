cmake_minimum_required(VERSION 3.16)
project(pteformats VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui Network PrintSupport Sql Svg Widgets Xml)

qt_standard_project_setup()

qt_add_library(pteformats STATIC
    fileformat.cpp fileformat.h
    fileformatmanager.cpp fileformatmanager.h
    layoutconfig.cpp layoutconfig.h
    gp7/document.cpp gp7/document.h
    gp7/from_pt2.cpp gp7/from_pt2.h
    gp7/from_xml.cpp
    gp7/gp7exporter.cpp gp7/gp7exporter.h
    gp7/gp7importer.cpp gp7/gp7importer.h
    gp7/to_pt2.cpp gp7/to_pt2.h
    gp7/to_xml.cpp
    gpx/bitstream.cpp gpx/bitstream.h
    gpx/filesystem.cpp gpx/filesystem.h
    gpx/gpximporter.cpp gpx/gpximporter.h
    gpx/util.cpp gpx/util.h
    guitar_pro/document.cpp guitar_pro/document.h
    guitar_pro/gp345to7converter.cpp guitar_pro/gp345to7converter.h
    guitar_pro/guitarproimporter.cpp guitar_pro/guitarproimporter.h
    guitar_pro/inputstream.cpp guitar_pro/inputstream.h
    midi/midiexporter.cpp midi/midiexporter.h
    powertab/common.h
    powertab/powertabexporter.cpp powertab/powertabexporter.h
    powertab/powertabimporter.cpp powertab/powertabimporter.h
    powertab_old/powertabdocument/alternateending.cpp powertab_old/powertabdocument/alternateending.h
    powertab_old/powertabdocument/barline.cpp powertab_old/powertabdocument/barline.h
    powertab_old/powertabdocument/chorddiagram.cpp powertab_old/powertabdocument/chorddiagram.h
    powertab_old/powertabdocument/chordname.cpp powertab_old/powertabdocument/chordname.h
    powertab_old/powertabdocument/chordtext.cpp powertab_old/powertabdocument/chordtext.h
    powertab_old/powertabdocument/colour.cpp powertab_old/powertabdocument/colour.h
    powertab_old/powertabdocument/complexsymbolarray.h
    powertab_old/powertabdocument/direction.cpp powertab_old/powertabdocument/direction.h
    powertab_old/powertabdocument/dynamic.cpp powertab_old/powertabdocument/dynamic.h
    powertab_old/powertabdocument/floatingtext.cpp powertab_old/powertabdocument/floatingtext.h
    powertab_old/powertabdocument/fontsetting.cpp powertab_old/powertabdocument/fontsetting.h
    powertab_old/powertabdocument/generalmidi.h
    powertab_old/powertabdocument/guitar.cpp powertab_old/powertabdocument/guitar.h
    powertab_old/powertabdocument/guitarin.cpp powertab_old/powertabdocument/guitarin.h
    powertab_old/powertabdocument/harmonics.cpp powertab_old/powertabdocument/harmonics.h
    powertab_old/powertabdocument/keysignature.cpp powertab_old/powertabdocument/keysignature.h
    powertab_old/powertabdocument/macros.cpp powertab_old/powertabdocument/macros.h
    powertab_old/powertabdocument/note.cpp powertab_old/powertabdocument/note.h
    powertab_old/powertabdocument/position.cpp powertab_old/powertabdocument/position.h
    powertab_old/powertabdocument/powertabdocument.cpp powertab_old/powertabdocument/powertabdocument.h
    powertab_old/powertabdocument/powertabfileheader.cpp powertab_old/powertabdocument/powertabfileheader.h
    powertab_old/powertabdocument/powertabinputstream.cpp powertab_old/powertabdocument/powertabinputstream.h
    powertab_old/powertabdocument/powertabobject.h
    powertab_old/powertabdocument/powertaboutputstream.cpp powertab_old/powertabdocument/powertaboutputstream.h
    powertab_old/powertabdocument/powertabstream.h
    powertab_old/powertabdocument/rect.cpp powertab_old/powertabdocument/rect.h
    powertab_old/powertabdocument/rehearsalsign.cpp powertab_old/powertabdocument/rehearsalsign.h
    powertab_old/powertabdocument/rhythmslash.cpp powertab_old/powertabdocument/rhythmslash.h
    powertab_old/powertabdocument/score.cpp powertab_old/powertabdocument/score.h
    powertab_old/powertabdocument/staff.cpp powertab_old/powertabdocument/staff.h
    powertab_old/powertabdocument/system.cpp powertab_old/powertabdocument/system.h
    powertab_old/powertabdocument/systemsymbol.cpp powertab_old/powertabdocument/systemsymbol.h
    powertab_old/powertabdocument/tempomarker.cpp powertab_old/powertabdocument/tempomarker.h
    powertab_old/powertabdocument/timesignature.cpp powertab_old/powertabdocument/timesignature.h
    powertab_old/powertabdocument/tuning.cpp powertab_old/powertabdocument/tuning.h
    powertab_old/powertaboldimporter.cpp powertab_old/powertaboldimporter.h
)

target_include_directories(pteformats PUBLIC
    ..
    ../third_part
    ../third_part/minizip-tools
    ../third_part/pugi-xml
    ${BOOST_INCLUDE_PATH}
)

target_link_libraries(pteformats PUBLIC
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

install(TARGETS pteformats
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
INCLUDE(FindZLIB)

IF(NOT ZLIB_FOUND)
        message(FATAL_ERROR "pteformats library is required for libminizip!")
ENDIF(NOT ZLIB_FOUND)

include_directories(${ZLIB_INCLUDE_DIR})
link_directories(${ZLIB_LIBRARIES})
endif(ANDROID)
