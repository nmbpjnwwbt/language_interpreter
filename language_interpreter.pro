TEMPLATE = app
CONFIG += c++17
QT += testlib

SOURCES += \
        lexer.cpp \
        main.cpp \
        old_parser.cpp \
        parser.cpp

DISTFILES += \
    syntax.ebnf

HEADERS += \
    lexer.h \
    old_parser.h \
    parser.h




target.path = .
INSTALLS += target
