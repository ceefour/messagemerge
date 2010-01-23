# -------------------------------------------------
# Project created by QtCreator 2010-01-10T18:43:06
# -------------------------------------------------
TARGET = message-merge
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    messagetemplate.cpp \
    templatesdialog.cpp \
    templateeditdialog.cpp \
    mainwizard.cpp \
    messagemerger.cpp
HEADERS += mainwindow.h \
    messagetemplate.h \
    templatesdialog.h \
    templateeditdialog.h \
    mainwizard.h \
    messagemerger.h
FORMS += mainwindow.ui \
    templatesdialog.ui \
    templateeditdialog.ui \
    mainwizard.ui
CONFIG += mobility
MOBILITY += contacts
MOBILITY += messages
