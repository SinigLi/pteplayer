cmake_minimum_required(VERSION 3.16)
project(pteplayer VERSION 1.01 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Core5Compat Gui Multimedia Network PrintSupport Quick QuickControls2 Sql Svg Widgets Xml)

qt_standard_project_setup()

qt_add_executable(pteplayer
    ScoreGroupMan.cpp ScoreGroupMan.h
    LayoutManager.cpp LayoutManager.h
    main.cpp
    scorechart.cpp scorechart.h
)
qt_add_qml_module(pteplayer
    URI Scores
    VERSION 1.0
    QML_FILES
        gallery.qml
        # pages/BusyIndicatorPage.qml
        # pages/ButtonPage.qml
        # pages/CheckBoxPage.qml
        # pages/ComboBoxPage.qml
        # pages/DelayButtonPage.qml
        # pages/DelegatePage.qml
        # pages/DialogPage.qml
        # pages/DialPage.qml
        # pages/FramePage.qml
        # pages/GroupBoxPage.qml
        # pages/PageIndicatorPage.qml
        # pages/ProgressBarPage.qml
        # pages/RadioButtonPage.qml
        # pages/RangeSliderPage.qml
        # pages/ScrollablePage.qml
        # pages/ScrollBarPage.qml
        # pages/ScrollIndicatorPage.qml
        # pages/SliderPage.qml
        # pages/SpinBoxPage.qml
        # pages/StackViewPage.qml
        # pages/SwipeViewPage.qml
        # pages/SwitchPage.qml
        # pages/TabBarPage.qml
        # pages/TextAreaPage.qml
        # pages/TextFieldPage.qml
        # pages/ToolTipPage.qml
        # pages/TumblerPage.qml
        pages/ScorePlayer.qml
        pages/PlayerSetDelegate.qml
        pages/PlayerSetModel.qml
        ToolBar.qml
        +Material/ToolBar.qml
    RESOURCES
        qmldir
        qtquickcontrols2.conf
        icons/gallery/index.theme
        # icons/gallery/20x20/back.png
        # icons/gallery/20x20/drawer.png
        icons/gallery/20x20/menu.png
        # icons/gallery/20x20@2/back.png
        # icons/gallery/20x20@2/drawer.png
        icons/gallery/20x20@2/menu.png
        # icons/gallery/20x20@3/back.png
        # icons/gallery/20x20@3/drawer.png
        icons/gallery/20x20@3/menu.png
        # icons/gallery/20x20@4/back.png
        # icons/gallery/20x20@4/drawer.png
        icons/gallery/20x20@4/menu.png
        # images/arrow.png
        # images/arrow@2x.png
        # images/arrow@3x.png
        # images/arrow@4x.png
        # images/arrows.png
        # images/arrows@2x.png
        # images/arrows@3x.png
        # images/arrows@4x.png
        # images/qt-logo.png
        # images/qt-logo@2x.png
        # images/qt-logo@3x.png
        # images/qt-logo@4x.png
    NO_RESOURCE_TARGET_PATH
)
target_include_directories(pteplayer PRIVATE
    ..
    ../third_part
    ../third_part/minizip-tools
    ../third_part/pugi-xml
    ${BOOST_INCLUDE_PATH}
)

# link_directories(G:/music_projs/pteplayer/source/libs G:/music_projs/pteplayer/source/third_part/boost-lib/arm64-v8a G:/music_projs/pteplayer/source/third_part/lib)
# find_package(PTE_QT_W CONFIG REQUIRED)
# find_package(BOOST_ARM_W CONFIG REQUIRED)
# find_package(THIRD_PART_W CONFIG REQUIRED)
find_library(boost_container_lib boost_container HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_date_time_lib boost_date_time HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_exception_lib boost_exception HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_filesystem_lib boost_filesystem HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_iostreams_lib boost_iostreams HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_random_lib boost_random HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_regex_lib boost_regex HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_system_lib boost_system HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)

# find_library(minizip_lib minizipd HINTS ../minzip.a)
# find_library(pugixml_lib pugixml HINTS ../minzip.a)
# find_library(rtmidi_lib rtmidi HINTS ../rtmidi.a)

# find_library(alsa_lib alsa HINTS ../rtmidi.a)

# find_library(alsa_atop_lib alsa_atop HINTS ../rtmidi.a)

#find_library(ptactions_lib ptactions_arm64-v8a HINTS ../rtmidi.a)
#find_library(ptaudio_lib ptaudio_arm64-v8a HINTS ../rtmidi.a)
#find_library(pteapp_lib pteapp_arm64-v8a HINTS ../rtmidi.a)
#find_library(ptedialogs_lib ptedialogs_arm64-v8a HINTS ../rtmidi.a)
#find_library(pteformats_lib pteformats_arm64-v8a HINTS ../rtmidi.a)
#find_library(ptemidi_lib ptemidi_arm64-v8a HINTS ../rtmidi.a)
#find_library(ptepainters_lib ptepainters_arm64-v8a HINTS ../rtmidi.a)
#find_library(ptescore_lib ptescore_arm64-v8a HINTS ../rtmidi.a)
#find_library(pteutil_lib pteutil_arm64-v8a HINTS ../rtmidi.a)
#find_library(ptewidgets_lib ptewidgets_arm64-v8a HINTS ../rtmidi.a)

set(LINKLIBS)
if(ANDROID)
    find_package(JNI)
    list(APPEND LINKLIBS log ${JNI_LIBRARIES} amidi z m)
endif(ANDROID)

# string(REPLACE ";" " " linklist "${LINKLIBS}")
# message(STATUS "linklist with support for: ${linklist}")

# target_link_libraries(pteplayer PRIVATE ${LINKLIBS})

target_link_libraries(pteplayer PRIVATE
    # Remove: L../../../libs/
    # Remove: L../../../third_part/boost-lib/arm64-v8a
    # Remove: L../../../third_part/lib/
    Qt::Concurrent
    Qt::Core
    Qt::Core5Compat
    Qt::Gui
    Qt::Multimedia
    Qt::Network
    Qt::PrintSupport
    Qt::Quick
    Qt::QuickControls2
    Qt::Sql
    Qt::Svg
    Qt::Widgets
    Qt::Xml
	ptactions
	pteapp
	ptaudio
	ptedialogs
	pteformats
	ptemidi
	ptepainters
	ptescore
	pteutil
	ptewidgets
        minizip
        pugixml-static
        rtmidi
    ${boost_container_lib}
    ${boost_date_time_lib}
    ${boost_exception_lib}
    ${boost_filesystem_lib}
    ${boost_iostreams_lib}
    ${boost_random_lib}
    ${boost_regex_lib}
    ${boost_system_lib}
    #${ptactions_lib}
    #${ptaudio_lib}
    #${pteapp_lib}
    #${ptedialogs_lib}
    #${pteformats_lib}
    #${ptemidi_lib}
    #${ptepainters_lib}
    #${ptescore_lib}
    #${pteutil_lib}
    #${ptewidgets_lib}
    # ${pugixml_lib}
    # ${minizip_lib}
    ${LINKLIBS}
)

# if(ANDROID)
# target_link_libraries(pteplayer PRIVATE
#     alsa
# )
# endif(ANDROID)

# Resources:
set_source_files_properties("../fonts/LiberationSans-Regular.ttf"
    PROPERTIES QT_RESOURCE_ALIAS "LiberationSans-Regular.ttf"
)
set_source_files_properties("../fonts/LiberationSerif-Regular.ttf"
    PROPERTIES QT_RESOURCE_ALIAS "LiberationSerif-Regular.ttf"
)
set_source_files_properties("../fonts/emmentaler-13.otf"
    PROPERTIES QT_RESOURCE_ALIAS "emmentaler-13.otf"
)
set(resources_resource_files
    "../fonts/LiberationSans-Regular.ttf"
    "../fonts/LiberationSerif-Regular.ttf"
    "../fonts/emmentaler-13.otf"
)

qt_add_resources(pteplayer "resources"
    PREFIX
        "/fonts"
    FILES
        ${resources_resource_files}
)
set_source_files_properties("../images/15ma.png"
    PROPERTIES QT_RESOURCE_ALIAS "15ma.png"
)
set_source_files_properties("../images/15mb.png"
    PROPERTIES QT_RESOURCE_ALIAS "15mb.png"
)
set_source_files_properties("../images/16th_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "16th_note"
)
set_source_files_properties("../images/16th_note_dotted.png"
    PROPERTIES QT_RESOURCE_ALIAS "16th_note_dotted"
)
set_source_files_properties("../images/16th_rest.png"
    PROPERTIES QT_RESOURCE_ALIAS "16th_rest"
)
set_source_files_properties("../images/32nd_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "32nd_note"
)
set_source_files_properties("../images/32nd_note_dotted.png"
    PROPERTIES QT_RESOURCE_ALIAS "32nd_note_dotted"
)
set_source_files_properties("../images/32nd_rest.png"
    PROPERTIES QT_RESOURCE_ALIAS "32nd_rest"
)
set_source_files_properties("../images/64th_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "64th_note"
)
set_source_files_properties("../images/64th_rest.png"
    PROPERTIES QT_RESOURCE_ALIAS "64th_rest"
)
set_source_files_properties("../images/8th_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "8th_note"
)
set_source_files_properties("../images/8th_note_dotted.png"
    PROPERTIES QT_RESOURCE_ALIAS "8th_note_dotted"
)
set_source_files_properties("../images/8th_rest.png"
    PROPERTIES QT_RESOURCE_ALIAS "8th_rest"
)
set_source_files_properties("../images/8va.png"
    PROPERTIES QT_RESOURCE_ALIAS "8va.png"
)
set_source_files_properties("../images/8vb.png"
    PROPERTIES QT_RESOURCE_ALIAS "8vb.png"
)
set_source_files_properties("../images/accent_heavy.png"
    PROPERTIES QT_RESOURCE_ALIAS "accent_heavy.png"
)
set_source_files_properties("../images/accent_normal.png"
    PROPERTIES QT_RESOURCE_ALIAS "accent_normal.png"
)
set_source_files_properties("../images/addinstrument.gif"
    PROPERTIES QT_RESOURCE_ALIAS "addinstrument.gif"
)
set_source_files_properties("../images/addpercussiontrack.gif"
    PROPERTIES QT_RESOURCE_ALIAS "addpercussiontrack.gif"
)
set_source_files_properties("../images/arpeggio_down.png"
    PROPERTIES QT_RESOURCE_ALIAS "arpeggio_down.png"
)
set_source_files_properties("../images/arpeggio_up.png"
    PROPERTIES QT_RESOURCE_ALIAS "arpeggio_up.png"
)
set_source_files_properties("../images/barline_repeatend.png"
    PROPERTIES QT_RESOURCE_ALIAS "barline_repeatend.png"
)
set_source_files_properties("../images/barline_single.png"
    PROPERTIES QT_RESOURCE_ALIAS "barline_single.png"
)
set_source_files_properties("../images/bend.png"
    PROPERTIES QT_RESOURCE_ALIAS "bend"
)
set_source_files_properties("../images/coda.png"
    PROPERTIES QT_RESOURCE_ALIAS "coda.png"
)
set_source_files_properties("../images/decreasetablineheight.gif"
    PROPERTIES QT_RESOURCE_ALIAS "decreasetablineheight.gif"
)
set_source_files_properties("../images/dotted_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "dotted_note"
)
set_source_files_properties("../images/double_flat.png"
    PROPERTIES QT_RESOURCE_ALIAS "double_flat.png"
)
set_source_files_properties("../images/double_sharp.png"
    PROPERTIES QT_RESOURCE_ALIAS "double_sharp.png"
)
set_source_files_properties("../images/doubledotted_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "doubledotted_note"
)
set_source_files_properties("../images/dynamic_f.png"
    PROPERTIES QT_RESOURCE_ALIAS "dynamic_f.png"
)
set_source_files_properties("../images/dynamic_ff.png"
    PROPERTIES QT_RESOURCE_ALIAS "dynamic_ff.png"
)
set_source_files_properties("../images/dynamic_fff.png"
    PROPERTIES QT_RESOURCE_ALIAS "dynamic_fff.png"
)
set_source_files_properties("../images/dynamic_mf.png"
    PROPERTIES QT_RESOURCE_ALIAS "dynamic_mf.png"
)
set_source_files_properties("../images/dynamic_mp.png"
    PROPERTIES QT_RESOURCE_ALIAS "dynamic_mp.png"
)
set_source_files_properties("../images/dynamic_p.png"
    PROPERTIES QT_RESOURCE_ALIAS "dynamic_p.png"
)
set_source_files_properties("../images/dynamic_pp.png"
    PROPERTIES QT_RESOURCE_ALIAS "dynamic_pp.png"
)
set_source_files_properties("../images/dynamic_ppp.png"
    PROPERTIES QT_RESOURCE_ALIAS "dynamic_ppp.png"
)
set_source_files_properties("../images/fermata.png"
    PROPERTIES QT_RESOURCE_ALIAS "fermata"
)
set_source_files_properties("../images/flat.png"
    PROPERTIES QT_RESOURCE_ALIAS "flat.png"
)
set_source_files_properties("../images/ghost.png"
    PROPERTIES QT_RESOURCE_ALIAS "ghost.png"
)
set_source_files_properties("../images/grace.png"
    PROPERTIES QT_RESOURCE_ALIAS "grace.png"
)
set_source_files_properties("../images/group_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "group_note"
)
set_source_files_properties("../images/group_note_irregular.png"
    PROPERTIES QT_RESOURCE_ALIAS "group_note_irregular.png"
)
set_source_files_properties("../images/half_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "half_note"
)
set_source_files_properties("../images/half_note_dotted.png"
    PROPERTIES QT_RESOURCE_ALIAS "half_note_dotted"
)
set_source_files_properties("../images/half_rest.png"
    PROPERTIES QT_RESOURCE_ALIAS "half_rest"
)
set_source_files_properties("../images/harmonic_artificial.png"
    PROPERTIES QT_RESOURCE_ALIAS "harmonic_artificial.png"
)
set_source_files_properties("../images/harmonic_natural.png"
    PROPERTIES QT_RESOURCE_ALIAS "harmonic_natural.png"
)
set_source_files_properties("../images/harmonic_tapped.png"
    PROPERTIES QT_RESOURCE_ALIAS "harmonic_tapped.png"
)
set_source_files_properties("../images/increasetablineheight.gif"
    PROPERTIES QT_RESOURCE_ALIAS "increasetablineheight.gif"
)
set_source_files_properties("../images/keysignature.png"
    PROPERTIES QT_RESOURCE_ALIAS "keysignature.png"
)
set_source_files_properties("../images/lefthandfingering.png"
    PROPERTIES QT_RESOURCE_ALIAS "lefthandfingering.png"
)
set_source_files_properties("../images/legato.png"
    PROPERTIES QT_RESOURCE_ALIAS "legato"
)
set_source_files_properties("../images/legatoslide.png"
    PROPERTIES QT_RESOURCE_ALIAS "legatoslide"
)
set_source_files_properties("../images/let_ring.png"
    PROPERTIES QT_RESOURCE_ALIAS "let_ring.png"
)
set_source_files_properties("../images/muted.png"
    PROPERTIES QT_RESOURCE_ALIAS "muted.png"
)
set_source_files_properties("../images/palm_mute.png"
    PROPERTIES QT_RESOURCE_ALIAS "palm_mute.png"
)
set_source_files_properties("../images/pickstroke_down.png"
    PROPERTIES QT_RESOURCE_ALIAS "pickstroke_down.png"
)
set_source_files_properties("../images/pickstroke_up.png"
    PROPERTIES QT_RESOURCE_ALIAS "pickstroke_up.png"
)
set_source_files_properties("../images/quarter_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "quarter_note"
)
set_source_files_properties("../images/quarter_rest.png"
    PROPERTIES QT_RESOURCE_ALIAS "quarter_rest"
)
set_source_files_properties("../images/rest_multibar.png"
    PROPERTIES QT_RESOURCE_ALIAS "rest_multibar.png"
)
set_source_files_properties("../images/sharp.png"
    PROPERTIES QT_RESOURCE_ALIAS "sharp.png"
)
set_source_files_properties("../images/shiftslide.png"
    PROPERTIES QT_RESOURCE_ALIAS "shiftslide"
)
set_source_files_properties("../images/slideinabove.png"
    PROPERTIES QT_RESOURCE_ALIAS "slideinabove"
)
set_source_files_properties("../images/slideinbelow.png"
    PROPERTIES QT_RESOURCE_ALIAS "slideinbelow"
)
set_source_files_properties("../images/slideoutdown.png"
    PROPERTIES QT_RESOURCE_ALIAS "slideoutdown"
)
set_source_files_properties("../images/slideoutup.png"
    PROPERTIES QT_RESOURCE_ALIAS "slideoutup"
)
set_source_files_properties("../images/staccato.png"
    PROPERTIES QT_RESOURCE_ALIAS "staccato.png"
)
set_source_files_properties("../images/tab_close_button.png"
    PROPERTIES QT_RESOURCE_ALIAS "tab_close_button.png"
)
set_source_files_properties("../images/tap.png"
    PROPERTIES QT_RESOURCE_ALIAS "tap.png"
)
set_source_files_properties("../images/tempo.png"
    PROPERTIES QT_RESOURCE_ALIAS "tempo.png"
)
set_source_files_properties("../images/text.png"
    PROPERTIES QT_RESOURCE_ALIAS "text.png"
)
set_source_files_properties("../images/tie_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "tie_note"
)
set_source_files_properties("../images/timesignature.png"
    PROPERTIES QT_RESOURCE_ALIAS "timesignature.png"
)
set_source_files_properties("../images/tremolo.png"
    PROPERTIES QT_RESOURCE_ALIAS "tremolo.png"
)
set_source_files_properties("../images/tremolobar.png"
    PROPERTIES QT_RESOURCE_ALIAS "tremolobar"
)
set_source_files_properties("../images/trill.png"
    PROPERTIES QT_RESOURCE_ALIAS "trill.png"
)
set_source_files_properties("../images/triplet_feel_eighth.png"
    PROPERTIES QT_RESOURCE_ALIAS "triplet_feel_eighth.png"
)
set_source_files_properties("../images/triplet_feel_eighth_off.png"
    PROPERTIES QT_RESOURCE_ALIAS "triplet_feel_eighth_off.png"
)
set_source_files_properties("../images/triplet_feel_sixteenth.png"
    PROPERTIES QT_RESOURCE_ALIAS "triplet_feel_sixteenth.png"
)
set_source_files_properties("../images/triplet_feel_sixteenth_off.png"
    PROPERTIES QT_RESOURCE_ALIAS "triplet_feel_sixteenth_off.png"
)
set_source_files_properties("../images/vibrato.png"
    PROPERTIES QT_RESOURCE_ALIAS "vibrato.png"
)
set_source_files_properties("../images/whole_note.png"
    PROPERTIES QT_RESOURCE_ALIAS "whole_note"
)
set_source_files_properties("../images/whole_rest.png"
    PROPERTIES QT_RESOURCE_ALIAS "whole_rest"
)
set_source_files_properties("../images/widevibrato.png"
    PROPERTIES QT_RESOURCE_ALIAS "widevibrato.png"
)

set_source_files_properties("../images/back.svg"
    PROPERTIES QT_RESOURCE_ALIAS "back.svg"
)
set_source_files_properties("../images/note.svg"
    PROPERTIES QT_RESOURCE_ALIAS "note.svg"
)
set_source_files_properties("../images/reflesh_set.svg"
    PROPERTIES QT_RESOURCE_ALIAS "reflesh_set.svg"
)
set_source_files_properties("../images/score_list.svg"
    PROPERTIES QT_RESOURCE_ALIAS "score_list.svg"
)
set_source_files_properties("../images/score_setting.svg"
    PROPERTIES QT_RESOURCE_ALIAS "score_setting.svg"
)
set_source_files_properties("../images/startPlay.svg"
    PROPERTIES QT_RESOURCE_ALIAS "startPlay.svg"
)
set_source_files_properties("../images/stopPlay.svg"
    PROPERTIES QT_RESOURCE_ALIAS "stopPlay.svg"
)
set_source_files_properties("../images/revertPlay.svg"
    PROPERTIES QT_RESOURCE_ALIAS "revertPlay.svg"
)
set(resources1_resource_files
    "../images/15ma.png"
    "../images/15mb.png"
    "../images/16th_note.png"
    "../images/16th_note_dotted.png"
    "../images/16th_rest.png"
    "../images/32nd_note.png"
    "../images/32nd_note_dotted.png"
    "../images/32nd_rest.png"
    "../images/64th_note.png"
    "../images/64th_rest.png"
    "../images/8th_note.png"
    "../images/8th_note_dotted.png"
    "../images/8th_rest.png"
    "../images/8va.png"
    "../images/8vb.png"
    "../images/accent_heavy.png"
    "../images/accent_normal.png"
    "../images/addinstrument.gif"
    "../images/addpercussiontrack.gif"
    "../images/arpeggio_down.png"
    "../images/arpeggio_up.png"
    "../images/barline_repeatend.png"
    "../images/barline_single.png"
    "../images/bend.png"
    "../images/coda.png"
    "../images/decreasetablineheight.gif"
    "../images/dotted_note.png"
    "../images/double_flat.png"
    "../images/double_sharp.png"
    "../images/doubledotted_note.png"
    "../images/dynamic_f.png"
    "../images/dynamic_ff.png"
    "../images/dynamic_fff.png"
    "../images/dynamic_mf.png"
    "../images/dynamic_mp.png"
    "../images/dynamic_p.png"
    "../images/dynamic_pp.png"
    "../images/dynamic_ppp.png"
    "../images/fermata.png"
    "../images/flat.png"
    "../images/ghost.png"
    "../images/grace.png"
    "../images/group_note.png"
    "../images/group_note_irregular.png"
    "../images/half_note.png"
    "../images/half_note_dotted.png"
    "../images/half_rest.png"
    "../images/harmonic_artificial.png"
    "../images/harmonic_natural.png"
    "../images/harmonic_tapped.png"
    "../images/increasetablineheight.gif"
    "../images/keysignature.png"
    "../images/lefthandfingering.png"
    "../images/legato.png"
    "../images/legatoslide.png"
    "../images/let_ring.png"
    "../images/muted.png"
    "../images/palm_mute.png"
    "../images/pickstroke_down.png"
    "../images/pickstroke_up.png"
    "../images/quarter_note.png"
    "../images/quarter_rest.png"
    "../images/rest_multibar.png"
    "../images/sharp.png"
    "../images/shiftslide.png"
    "../images/slideinabove.png"
    "../images/slideinbelow.png"
    "../images/slideoutdown.png"
    "../images/slideoutup.png"
    "../images/staccato.png"
    "../images/tab_close_button.png"
    "../images/tap.png"
    "../images/tempo.png"
    "../images/text.png"
    "../images/tie_note.png"
    "../images/timesignature.png"
    "../images/tremolo.png"
    "../images/tremolobar.png"
    "../images/trill.png"
    "../images/triplet_feel_eighth.png"
    "../images/triplet_feel_eighth_off.png"
    "../images/triplet_feel_sixteenth.png"
    "../images/triplet_feel_sixteenth_off.png"
    "../images/vibrato.png"
    "../images/whole_note.png"
    "../images/whole_rest.png"
    "../images/widevibrato.png"
    "../images/back.svg"
    "../images/note.svg"
    "../images/reflesh_set.svg"
    "../images/score_list.svg"
    "../images/score_setting.svg"
    "../images/startPlay.svg"
    "../images/stopPlay.svg"
    "../images/revertPlay.svg"
)
qt_add_resources(pteplayer "resources1"
    PREFIX
        "/images"
    FILES
        ${resources1_resource_files}
)
set_source_files_properties("../icons/app_icon.png"
    PROPERTIES QT_RESOURCE_ALIAS "app_icon.png"
)
set_source_files_properties("../icons/app_icon.svg"
    PROPERTIES QT_RESOURCE_ALIAS "app_icon.svg"
)
set_source_files_properties("../icons/r_app_icon.svg"
    PROPERTIES QT_RESOURCE_ALIAS "r_app_icon.svg"
)
set(resources2_resource_files
    "../icons/app_icon.png"
    "../icons/app_icon.svg"
    "../icons/r_app_icon.svg"
)

qt_add_resources(pteplayer "resources2"
    PREFIX
        "/icons"
    FILES
        ${resources2_resource_files}
)


# set_source_files_properties("../music/tick-4-4-120bp.wav"
#     PROPERTIES QT_RESOURCE_ALIAS "tick-4-4-120bp"
# )
# set(resources3_resource_files
#     "../music/tick-4-4-120bp.wav"
# )

qt_add_resources(pteplayer "resources3"
    PREFIX
        "/music"
    FILES
        ${resources3_resource_files}
)
# install(TARGETS pteplayer
#     BUNDLE DESTINATION ./
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# install(TARGETS pteplayer
#     BUNDLE DESTINATION .
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

qt_generate_deploy_qml_app_script(
    TARGET pteplayer
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
    MACOS_BUNDLE_POST_BUILD
)
install(SCRIPT ${deploy_script})

install(TARGETS pteplayer
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    BUNDLE DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

if(ANDROID)
set_property(TARGET pteplayer APPEND PROPERTY
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)
endif(ANDROID)

if(WIN32)
INCLUDE(FindZLIB)

IF(NOT ZLIB_FOUND)
        message(FATAL_ERROR "pteplayer library is required for libminizip!")
ENDIF(NOT ZLIB_FOUND)

include_directories(${ZLIB_INCLUDE_DIR})
link_directories(${ZLIB_LIBRARIES})
endif(WIN32)
