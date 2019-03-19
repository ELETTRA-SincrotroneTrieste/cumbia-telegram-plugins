#-------------------------------------------------
#
# Project created by QtCreator 2019-03-19T16:46:46
#
#-------------------------------------------------

include(../cumbia-telegram-plugins.pri)

include(/usr/local/cumbia-libs/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

# for qwt
QT += gui

VERSION = 1.0.0
TARGET = plot-generator-plugin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        botplot.cpp \
        botplotgenerator.cpp

HEADERS +=  \
        botplot.h \
        botplotgenerator.h

# DISTFILES += cumbia-telegram-plugins.json

unix {
    target.path = $${PLUGIN_TARGET_DIR}
    INSTALLS += target
}
