QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app
CONFIG += console
CONFIG   += c++11

SOURCES += main.cpp \
    game1.cpp \
    gamefield.cpp\
        level.cpp \
        Tiny/tinyxmlerror.cpp \
        Tiny/tinyxml.cpp \
        Tiny/tinystr.cpp \
        Tiny/tinyxmlparser.cpp \
        Tiny/tinyxml2.cpp \
        worldsimulator.cpp \
        worldphysic.cpp\
        graphicobject.cpp\
        graphicengine.cpp\
        movetothevictim.cpp\
        evadefromtherocket.cpp

HEADERS  += \
    game1.h \
    gamefield.h\
    rbwincludes.h\
    level.h \
    Tiled/tinystr.h \
    Tiled/tinyxml.h \
    Tiny/tinyxml.h \
    Tiny/tinystr.h \
    Tiny/tinyxml2.h \
    graphicobject.h \
    worldsimulator.h \
    worldphysic.h \
    graphicobject.h \
    graphicengine.h \
    worldconstant.h \
    movetothevictim.h\
    evadefromtherocket.h\

INCLUDEPATH += Tiled/

 unix|win32: LIBS += -lsfml-window

 unix|win32: LIBS += -lsfml-graphics

 unix|win32: LIBS += -lsfml-system
