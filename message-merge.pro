# -------------------------------------------------
# Project created by QtCreator 2010-01-10T18:43:06
# -------------------------------------------------
TARGET = message-merge
TEMPLATE = app
SOURCES += main.cpp \
    templatesdialog.cpp \
    templateeditdialog.cpp \
    mainwizard.cpp \
    messagemerger.cpp
HEADERS += templatesdialog.h \
    templateeditdialog.h \
    mainwizard.h \
    messagemerger.h
FORMS += templatesdialog.ui \
    templateeditdialog.ui \
    mainwizard.ui
CONFIG += mobility
MOBILITY += contacts

linux {
    #On Linux QMF is required to be installed and the QMF_INCLUDEDIR and QMF_LIBDIR environment variables
    #set to the messagingframework/src/libraries/qtopiamail and build/messagingframework/image/lib directories respectively.
    QMF_INCLUDEDIR = "/opt/qt-messagingframework/src/libraries/qtopiamail"
    QMF_LIBDIR = "/opt/qt-messagingframework/image/lib"
    qmf_enabled = yes
}

symbian {
    MOBILITY += messaging
} win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
#LIBS += -L /opt/qt-messagingframework/image/lib -lqtopiamail
}
