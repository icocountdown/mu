TEMPLATE = lib
CONFIG += static
CONFIG -= qt
TARGET = mu_script_io

HEADERS += \
    synthesizer.h \
    script.h \
    routine.h \
    reference.h \
    lambda.h \
    lambda_single.h \
    expression.h \
    cluster.h \
    builder.h

SOURCES += \
    synthesizer.cpp \
    script.cpp \
    routine.cpp \
    reference.cpp \
    lambda.cpp \
    lambda_single.cpp \
    expression.cpp \
    cluster.cpp \
    builder.cpp

INCLUDEPATH += \
    $(BASE) \
    $(BASE)/boost

QMAKE_CXXFLAGS += \
    -std=c++0x
