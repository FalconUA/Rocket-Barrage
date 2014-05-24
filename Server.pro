QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rbServer
TEMPLATE = app
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += Server/
INCLUDEPATH += Engine/
INCLUDEPATH += Engine/Tiny/
INCLUDEPATH += AI/

SOURCES += \
    server-main.cpp \
    Server/server.cpp \
    Server/ingame.cpp \
    \
    Engine/level.cpp \
    Engine/Tiny/tinyxmlerror.cpp \
    Engine/Tiny/tinyxml.cpp \
    Engine/Tiny/tinystr.cpp \
    Engine/Tiny/tinyxmlparser.cpp \
    Engine/Tiny/tinyxml2.cpp \
    Engine/graphicobject.cpp \
    Engine/worldsimulator.cpp \
    Engine/worldphysic.cpp \
    Engine/graphicengine.cpp \
    \
    AI/evadefromtherocket.cpp \
    AI/movetothevictim.cpp

HEADERS  += \
    Server/server.h \
    Server/ingame.h \
    \
    Engine/level.h \
    Engine/Tiny/tinyxml.h \
    Engine/Tiny/tinystr.h \
    Engine/Tiny/tinyxml2.h \
    Engine/graphicobject.h \
    Engine/worldsimulator.h \
    Engine/worldphysic.h \
    Engine/worldconstant.h \
    Engine/graphicengine.h \
    Engine/rbwincludes.h \
    \
    AI/evadefromtherocket.h \
    AI/movetothevictim.h

 unix|win32: LIBS += -lsfml-window

 unix|win32: LIBS += -lsfml-graphics

 unix|win32: LIBS += -lsfml-system
