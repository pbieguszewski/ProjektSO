TEMPLATE = app

CONFIG += C++11
CONFIG -= QT
CONFIG += P1

OBJECTS_DIR = $$PWD/../ProjektSO/build/object/
DESTDIR = $$PWD/../ProjektSO/build/

HEADERS += bufor.h

P0 {
    TARGET = p0
    SOURCES += p0.cpp
}

P1 {
    TARGET = p1
    SOURCES += p1.cpp
}

P2 {
    TARGET = p2
    SOURCES += p2.cpp
}

P3 {
    TARGET = p3
    SOURCES += p3.cpp
}
