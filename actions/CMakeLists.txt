cmake_minimum_required(VERSION 3.16)
project(ptactions VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui PrintSupport Sql Svg Widgets Xml)

qt_standard_project_setup()

qt_add_library(ptactions STATIC
    addalternateending.cpp addalternateending.h
    addbarline.cpp addbarline.h
    addchordtext.cpp addchordtext.h
    adddirection.cpp adddirection.h
    adddynamic.cpp adddynamic.h
    addinstrument.cpp addinstrument.h
    addirregulargrouping.cpp addirregulargrouping.h
    addmultibarrest.cpp addmultibarrest.h
    addnote.cpp addnote.h
    addnoteproperty.cpp addnoteproperty.h
    addplayer.cpp addplayer.h
    addpositionproperty.cpp addpositionproperty.h
    addrest.cpp addrest.h
    addspecialnoteproperty.h
    addstaff.cpp addstaff.h
    addsystem.cpp addsystem.h
    adjustlinespacing.cpp adjustlinespacing.h
    chorddiagram.cpp chorddiagram.h
    editbarline.cpp editbarline.h
    editdynamic.cpp editdynamic.h
    editfileinformation.cpp editfileinformation.h
    editinstrument.cpp editinstrument.h
    editkeysignature.cpp editkeysignature.h
    editnoteduration.cpp editnoteduration.h
    editplayer.cpp editplayer.h
    editplayerchange.cpp editplayerchange.h
    editrehearsalsign.cpp editrehearsalsign.h
    editstaff.cpp editstaff.h
    edittabnumber.cpp edittabnumber.h
    edittempomarker.cpp edittempomarker.h
    edittextitem.cpp edittextitem.h
    edittimesignature.cpp edittimesignature.h
    editviewfilters.cpp editviewfilters.h
    insertnotes.cpp insertnotes.h
    polishscore.cpp polishscore.h
    polishsystem.cpp polishsystem.h
    removealternateending.cpp removealternateending.h
    removebarline.cpp removebarline.h
    removechordtext.cpp removechordtext.h
    removedirection.cpp removedirection.h
    removedynamic.cpp removedynamic.h
    removeinstrument.cpp removeinstrument.h
    removeirregulargrouping.cpp removeirregulargrouping.h
    removenote.cpp removenote.h
    removenoteproperty.cpp removenoteproperty.h
    removeplayer.cpp removeplayer.h
    removeposition.cpp removeposition.h
    removepositionproperty.cpp removepositionproperty.h
    removespecialnoteproperty.h
    removestaff.cpp removestaff.h
    removesystem.cpp removesystem.h
    shiftpositions.cpp shiftpositions.h
    shiftstring.cpp shiftstring.h
    tremolobar.cpp tremolobar.h
    undomanager.cpp undomanager.h
    volumeswell.cpp volumeswell.h
)

target_include_directories(ptactions PUBLIC
    ..
    ../third_part
    ${BOOST_INCLUDE_PATH}
)

target_link_libraries(ptactions PUBLIC
    Qt::Concurrent
    Qt::Core
    Qt::Gui
    Qt::PrintSupport
    Qt::Sql
    Qt::Svg
    Qt::Widgets
    Qt::Xml
)

install(TARGETS ptactions
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
