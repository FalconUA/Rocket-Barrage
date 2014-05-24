QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Game
TEMPLATE = app
CONFIG   += c++11

SOURCES += main.cpp\
        level.cpp \
        Tiny/tinyxmlerror.cpp \
        Tiny/tinyxml.cpp \
        Tiny/tinystr.cpp \
        Tiny/tinyxmlparser.cpp \
        Tiny/tinyxml2.cpp \
        server.cpp ingame.cpp\
        worldsimulator.cpp \
        worldphysic.cpp\
        evadefromtherocket.cpp\
        movetothevictim.cpp\

HEADERS  += server.h \
    ingame.h\
    level.h \
    Tiled/tinystr.h \
    Tiled/tinyxml.h \
    Tiny/tinyxml.h \
    Tiny/tinystr.h \
    Tiny/tinyxml2.h \
    game.h \
    graphicobject.h \
    worldsimulator.h \
    worldphysic.h \
    graphicobject.h \
    worldconstant.h \
    evadefromtherocket.h \
    movetothevictim.h\
 unix|win32: LIBS += -lsfml-window

 unix|win32: LIBS += -lsfml-graphics

 unix|win32: LIBS += -lsfml-system
