TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    level.cpp \
    Tiny/tinyxmlerror.cpp \
    Tiny/tinyxml.cpp \
    Tiny/tinystr.cpp \
    Tiny/tinyxmlparser.cpp \
    Tiny/tinyxml2.cpp \
    game.cpp \
    graphicobject.cpp \
    worldsimulator.cpp \
    worldphysic.cpp \
    graphicengine.cpp

INCLUDEPATH += Tiled/

unix|win32: LIBS += -lsfml-window

unix|win32: LIBS += -lsfml-graphics

unix|win32: LIBS += -lsfml-system

HEADERS += \
    level.h \
    Tiny/tinyxml.h \
    Tiny/tinystr.h \
    Tiny/tinyxml2.h \
    game.h \
    graphicobject.h \
    worldsimulator.h \
    worldphysic.h \
    worldconstant.h \
    graphicengine.h \
    rbwincludes.h

OTHER_FILES +=
