CONFIG += qt

QT += core sql network gui svg widgets printsupport concurrent  xml

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



# Default rules for deployment.
#unix {
#    target.path = $$[QT_INSTALL_PLUGINS]/generic
#}
#!isEmpty(target.path): INSTALLS += target
DESTDIR = ../../../libs

INCLUDEPATH+= ../ \
../third_part \
../third_part/pugi-xml \
../third_part/minizip-tools

HEADERS += \
    antialiasedpathitem.h \
    barlinepainter.h \
    beamgroup.h \
    caretpainter.h \
    chorddiagrampainter.h \
    clickableitem.h \
    keysignaturepainter.h \
    layoutinfo.h \
    musicfont.h \
    notestem.h \
#    rhythmslashpainter.h \
    scoreclickevent.h \
    scoreinforenderer.h \
    simpletextitem.h \
    staffpainter.h \
    stdnotationnote.h \
    styles.h \
    systemrenderer.h \
    timesignaturepainter.h \
    verticallayout.h

SOURCES += \
    antialiasedpathitem.cpp \
    barlinepainter.cpp \
    beamgroup.cpp \
    caretpainter.cpp \
    chorddiagrampainter.cpp \
    clickableitem.cpp \
    directions.cpp \
    keysignaturepainter.cpp \
    layoutinfo.cpp \
    musicfont.cpp \
    notestem.cpp \
#    rhythmslashpainter.cpp \
    scoreinforenderer.cpp \
    simpletextitem.cpp \
    staffpainter.cpp \
    stdnotationnote.cpp \
    styles.cpp \
    systemrenderer.cpp \
    timesignaturepainter.cpp \
    verticallayout.cpp
