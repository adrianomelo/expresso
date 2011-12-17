TEMPLATE = lib
CONFIG += plugin
TARGET = expresso

QT += declarative

HEADERS += plugin.h \
           sprite.h \
           rawconnection.h

SOURCES += plugin.cpp \
           sprite.cpp \
           rawconnection.cpp

QML_SOURCES += qmldir

INSTALL_PATH = $$[QT_INSTALL_IMPORTS]/Expresso

target.path = $$INSTALL_PATH

qmlfiles.files = $$QML_SOURCES
qmlfiles.path = $$INSTALL_PATH

INSTALLS += target qmlfiles
