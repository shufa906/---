QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TextEditor
TEMPLATE = app

# 中文源码编码（Qt 5.12 / MinGW）
QMAKE_CXXFLAGS += -finput-charset=UTF-8

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    aboutdialog.cpp \
    finddialog.cpp \
    replacedialog.cpp

HEADERS += \
    mainwindow.h \
    codeeditor.h \
    aboutdialog.h \
    finddialog.h \
    replacedialog.h

FORMS += \
    mainwindow.ui
