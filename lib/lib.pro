TEMPLATE = lib
CONFIG += plugin
TARGET = expresso

QT += declarative

HEADERS += plugin.h \
           sprite.h \
           common.h \
           settings.h \
           expresso.h \
           pcmsound.h \
           pixmaptext.h \
           particlesystem.h \
           imageloader.h

SOURCES += plugin.cpp \
           common.cpp \
           settings.cpp \
           sprite.cpp \
           expresso.cpp \
           pcmsound.cpp \
           pixmaptext.cpp \
           particlesystem.cpp \
           imageloader.cpp

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libpulse

    DEFINES += PCMSOUND_PULSE

    HEADERS += $$PWD/pcmsound_pulse.h
    SOURCES += $$PWD/pcmsound_pulse.cpp
} else {
    HEADERS += $$PWD/pcmsound_dummy.h
}

QML_SOURCES += qmldir

INSTALL_PATH = $$[QT_INSTALL_IMPORTS]/Expresso

target.path = $$INSTALL_PATH

qmlfiles.files = $$QML_SOURCES
qmlfiles.path = $$INSTALL_PATH

INSTALLS += target qmlfiles
