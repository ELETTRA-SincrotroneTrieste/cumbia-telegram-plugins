#-------------------------------------------------
#
# Project created by QtCreator 2019-03-19T16:46:46
#
#-------------------------------------------------

#
#       customization section
#
INSTALL_ROOT = /usr/local/cumbia-telegram
CUMBIA_TELEGRAM_ROOT = /usr/local/cumbia-telegram
PLUGIN_TARGET_DIR = $${INSTALL_ROOT}/lib/plugins
#
#      end of customization section
#

CUMBIA_TELEGRAM_INCLUDES = $${CUMBIA_TELEGRAM_ROOT}/include
CUMBIA_TELEGRAM_LIB = $${CUMBIA_TELEGRAM_ROOT}/lib

QT       += core

DISTFILES += cumbia-telegram-plugins.json

unix:INCLUDEPATH += $${CUMBIA_TELEGRAM_INCLUDES}
unix:LIBS += -L$${CUMBIA_TELEGRAM_LIB} -lcumbia-telegram

TEMPLATE = lib
