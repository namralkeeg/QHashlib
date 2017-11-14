#-------------------------------------------------
#
# Project created by QtCreator 2017-10-23T20:23:11
#
#-------------------------------------------------

QT       -= gui

TARGET = QHashlib
TEMPLATE = lib
CONFIG += staticlib c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INTERMEDIATE = $${OUT_PWD}/GeneratedFiles/$${BUILD}
UI_DIR = $${INTERMEDIATE}/.ui
MOC_DIR = $${INTERMEDIATE}/.moc
RCC_DIR = $${INTERMEDIATE}/.qrc
OBJECTS_DIR = $${INTERMEDIATE}/.obj

# Debug/Release options
CONFIG(debug, debug|release) {
        # Debug Options
        BUILD = debug
        CONFIG += console
} else {
        # Release Options
        BUILD = release
        CONFIG -= console
        DEFINES += QT_NO_DEBUG_OUTPUT
}

###############################
## PROJECT SCOPES
###############################

INCLUDEPATH += src

SOURCES += \
    src/hashalgorithm.cpp \
    src/checksum/adler32.cpp \
    src/crc/crc32.cpp \
    src/cryptographic/md5.cpp \
    src/cryptographic/sha1.cpp \
    src/cryptographic/sha256.cpp \
    src/cryptographic/sha3.cpp \
    src/noncryptographic/aphash32.cpp \
    src/noncryptographic/bkdrhash32.cpp \
    src/noncryptographic/djb2hash32.cpp \
    src/noncryptographic/elfhash32.cpp \
    src/noncryptographic/joaathash32.cpp \
    src/noncryptographic/jshash32.cpp \
    src/noncryptographic/pjwhash32.cpp \
    src/noncryptographic/saxhash32.cpp \
    src/noncryptographic/sdbmhash32.cpp \
    src/noncryptographic/superfasthash32.cpp \
    src/noncryptographic/xxhash32.cpp \
    src/noncryptographic/xxhash64.cpp \
    src/noncryptographic/fnv1hash.cpp \
    src/noncryptographic/fnv1ahash.cpp \
    src/noncryptographic/fnvbase.cpp

HEADERS += \
    src/hashalgorithm.hpp \
    src/endian.hpp \
    src/macrohelpers.hpp \
    src/checksum/adler32.hpp \
    src/crc/crc32.hpp \
    src/cryptographic/md5.hpp \
    src/cryptographic/sha1.hpp \
    src/cryptographic/sha256.hpp \
    src/cryptographic/sha3.hpp \
    src/enums.hpp \
    src/noncryptographic/aphash32.hpp \
    src/noncryptographic/bkdrhash32.hpp \
    src/noncryptographic/djb2hash32.hpp \
    src/noncryptographic/elfhash32.hpp \
    src/noncryptographic/joaathash32.hpp \
    src/noncryptographic/jshash32.hpp \
    src/noncryptographic/pjwhash32.hpp \
    src/noncryptographic/saxhash32.hpp \
    src/noncryptographic/sdbmhash32.hpp \
    src/noncryptographic/superfasthash32.hpp \
    src/noncryptographic/xxhash32.hpp \
    src/noncryptographic/xxhash64.hpp \
    src/noncryptographic/fnvbase.hpp \
    src/noncryptographic/fnv1hash.hpp \
    src/noncryptographic/fnv1ahash.hpp

###############################
## COMPILER SCOPES
###############################

*msvc* {
        # So VCProj Filters do not flatten headers/source
        CONFIG -= flat

        # COMPILER FLAGS

        #  Optimization flags
        QMAKE_CXXFLAGS_RELEASE -= /O2
        QMAKE_CXXFLAGS_RELEASE *= /O2 /Ot /Ox /GL
        #  Multithreaded compiling for Visual Studio
        QMAKE_CXXFLAGS += -MP
        # Linker flags
        QMAKE_LFLAGS_RELEASE += /LTCG
}

*-g++ {

        # COMPILER FLAGS

        #  Optimization flags
        QMAKE_CXXFLAGS_DEBUG -= -O0 -g
        QMAKE_CXXFLAGS_DEBUG *= -Og -g3
        QMAKE_CXXFLAGS_RELEASE *= -O3 -mfpmath=sse

        #  Extension flags
        QMAKE_CXXFLAGS_RELEASE += -msse2 -msse
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
