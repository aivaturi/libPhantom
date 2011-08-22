#-------------------------------------------------
#
# Project created by QtCreator 2011-08-21T13:39:02
#
#-------------------------------------------------

QT       += network webkit

TARGET = libPhantom
TEMPLATE = lib

DEFINES += LIBPHANTOM_LIBRARY \
           QT_NO_DEBUG_OUTPUT

DESTDIR = ../bin

SOURCES += phantom.cpp \
    webpage.cpp \
    utils.cpp \
    terminal.cpp \
    registry.cpp \
    networkreplyproxy.cpp \
    networkaccessmanager.cpp \
    filesystem.cpp \
    encoding.cpp \
    csconverter.cpp \
    cookiejar.cpp
    #gif/quantize.c \
    #gif/gifwriter.cpp \
    #gif/gifalloc.c \
    #gif/gif_hash.c \
    #gif/gif_err.c \
    #gif/egif_lib.c

HEADERS += phantom.h\
        libPhantom_global.h \
    webpage.h \
    utils.h \
    terminal.h \
    registry.h \
    networkreplyproxy.h \
    networkaccessmanager.h \
    filesystem.h \
    encoding.h \
    csconverter.h \
    cookiejar.h \
    consts.h
    #gif/gifwriter.h \
    #gif/gif_lib_private.h \
    #gif/gif_lib.h \
    #gif/gif_hash.h \
    #gif/config.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    coffee-script.js \
    bootstrap.js

include(gif/gif.pri)







