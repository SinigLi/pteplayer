cmake_minimum_required(VERSION 3.16)
project(ptedialogs VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui Network PrintSupport Sql Svg Widgets Xml)

qt_standard_project_setup()

qt_add_library(ptedialogs STATIC
    alterationofpacedialog.cpp alterationofpacedialog.h alterationofpacedialog.ui
    alternateendingdialog.cpp alternateendingdialog.h
    artificialharmonicdialog.cpp artificialharmonicdialog.h artificialharmonicdialog.ui
    barlinedialog.cpp barlinedialog.h barlinedialog.ui
    benddialog.cpp benddialog.h benddialog.ui
    bulkconverterdialog.cpp bulkconverterdialog.h bulkconverterdialog.ui
    chorddiagramdialog.cpp chorddiagramdialog.h chorddiagramdialog.ui
    chordnamedialog.cpp chordnamedialog.h chordnamedialog.ui
    crashdialog.cpp crashdialog.h crashdialog.ui
    directiondialog.cpp directiondialog.h directiondialog.ui
    dynamicdialog.cpp dynamicdialog.h dynamicdialog.ui
    fileinformationdialog.cpp fileinformationdialog.h fileinformationdialog.ui
    filterrulewidget.cpp filterrulewidget.h filterrulewidget.ui
    gotobarlinedialog.cpp gotobarlinedialog.h gotobarlinedialog.ui
    gotorehearsalsigndialog.cpp gotorehearsalsigndialog.h gotorehearsalsigndialog.ui
    infodialog.cpp infodialog.h infodialog.ui
    irregulargroupingdialog.cpp irregulargroupingdialog.h irregulargroupingdialog.ui
    keyboardsettingsdialog.cpp keyboardsettingsdialog.h keyboardsettingsdialog.ui
    keysignaturedialog.cpp keysignaturedialog.h keysignaturedialog.ui
    lefthandfingeringdialog.cpp lefthandfingeringdialog.h lefthandfingeringdialog.ui
    multibarrestdialog.cpp multibarrestdialog.h multibarrestdialog.ui
    playerchangedialog.cpp playerchangedialog.h playerchangedialog.ui
    preferencesdialog.cpp preferencesdialog.h preferencesdialog.ui
    rehearsalsigndialog.cpp rehearsalsigndialog.h rehearsalsigndialog.ui
    staffdialog.cpp staffdialog.h staffdialog.ui
    tappedharmonicdialog.cpp tappedharmonicdialog.h tappedharmonicdialog.ui
    tempomarkerdialog.cpp tempomarkerdialog.h tempomarkerdialog.ui
    textitemdialog.cpp textitemdialog.h textitemdialog.ui
    timesignaturedialog.cpp timesignaturedialog.h timesignaturedialog.ui
    tremolobardialog.cpp tremolobardialog.h tremolobardialog.ui
    trilldialog.cpp trilldialog.h trilldialog.ui
    tuningdialog.cpp tuningdialog.h tuningdialog.ui
    tuningdictionarydialog.cpp tuningdictionarydialog.h tuningdictionarydialog.ui
    viewfilterdialog.cpp viewfilterdialog.h viewfilterdialog.ui
    viewfilterpresenter.cpp viewfilterpresenter.h
    volumeswelldialog.cpp volumeswelldialog.h volumeswelldialog.ui
)
target_include_directories(ptedialogs PUBLIC
    ..
    ../third_part
    ${BOOST_INCLUDE_PATH}
)

target_link_libraries(ptedialogs PUBLIC
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

install(TARGETS ptedialogs
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
