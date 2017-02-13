
TARGET = StrideParser
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++11

SOURCES += ast.cpp \
           platformnode.cpp \
           streamnode.cpp \
    valuenode.cpp \
    bundlenode.cpp \
    propertynode.cpp \
    namenode.cpp \
    functionnode.cpp \
    expressionnode.cpp \
    listnode.cpp \
    importnode.cpp \
    fornode.cpp \
    rangenode.cpp \
    langerror.cpp \
    keywordnode.cpp \
    declarationnode.cpp

HEADERS += ast.h \
           platformnode.h \
           streamnode.h \
    valuenode.h \
    bundlenode.h \
    propertynode.h \
    namenode.h \
    functionnode.h \
    expressionnode.h \
    listnode.h \
    importnode.h \
    fornode.h \
    rangenode.h \
    langerror.h \
    strideparser.h \
    keywordnode.hpp \
    declarationnode.h

BISONSOURCES = lang_stride.y
FLEXSOURCES = lang_stride.l

include(../config.pri)
include(parser.pri)

win32 {
    LIBS += -L${FLEX_LIB_PATH}
    LIBS += -lfl
}

unix {
    !macx {
        LIBS += -lfl
    }
    macx {
#        LIBS += -L${FLEX_LIB_PATH}
#        LIBS += -ll
    }
}
