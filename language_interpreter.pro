TEMPLATE = app
CONFIG += c++17
QT += testlib

SOURCES += \
        lexer.cpp \
        main.cpp \
        parser.cpp

DISTFILES += \
    syntax.ebnf

HEADERS += \
    lexer.h \
    parser.h




target.path = .
INSTALLS += target
