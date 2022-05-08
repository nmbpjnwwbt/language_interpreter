TEMPLATE = app
CONFIG += c++17
QT += testlib

SOURCES += \
        lexer.cpp \
        main.cpp

DISTFILES += \
    syntax.ebnf

HEADERS += \
    lexer.h




target.path = .
INSTALLS += target
