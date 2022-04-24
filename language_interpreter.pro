TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        lexer.cpp \
        main.cpp

DISTFILES += \
    syntax.ebnf

HEADERS += \
    lexer.h
