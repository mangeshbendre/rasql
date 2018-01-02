# -------------------------------------------------
# Project created by QtCreator 2010-03-05T20:15:02
# -------------------------------------------------
QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rat
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    settings.cpp \
    connect.cpp \
    highlighter.cpp \
    richraparser.cpp \
    dbconnection.cpp \
    ../system/UTFConverter.cpp \
    ../system/RAParser.cpp \
    ../system/LALR.cpp \
    ../system/ErrorReporter.cpp \
    ../system/DFA.cpp \
    ../system/CGTFile.cpp
HEADERS += ../system/RAParser.h \
    mainwindow.h \
    dbconnection.h \
    settings.h \
    connect.h \
    highlighter.h \
    richraparser.h \
    Misc.h \
    ../system/UTFConverter.h \
    ../system/SymbolBase.h \
    ../system/RAParser.h \
    ../system/Misc.h \
    ../system/LALR.h \
    ../system/Factory_RA.h \
    ../system/ErrorReporter.h \
    ../system/DFA.h \
    ../system/defines.h \
    ../system/CGTFile.h
FORMS += mainwindow.ui \
    settings.ui \
    connect.ui
RESOURCES += ra.qrc

DISTFILES +=
