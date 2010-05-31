OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src \
               ../stubs \

SRC_DIR = ../../src


# Input
HEADERS += \
    ut_mtoolbarmanager.h \
    ../stubs/mgconfitem_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/minputcontextconnection_stub.h \
    $$SRC_DIR/mtoolbaritem.h \
    $$SRC_DIR/mtoolbardata.h \
    $$SRC_DIR/minputmethodnamespace.h \
    $$SRC_DIR/mtoolbarmanager.h \

SOURCES += \
    ut_mtoolbarmanager.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/minputcontextconnection_stub.cpp \
    $$SRC_DIR/mtoolbaritem.cpp \
    $$SRC_DIR/mtoolbardata.cpp \
    $$SRC_DIR/mtoolbarmanager.cpp \


CONFIG += debug plugin meegotouch qdbus

LIBS += \
    ../../src/libmeegoimframework.so.0 \

target.files += \
    toolbar1.xml \
    toolbar2.xml \

include(../common_check.pri)