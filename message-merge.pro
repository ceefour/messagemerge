#
# Copyright 2010 Soluvas <http://www.soluvas.com>
#
# This software is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library.  If not, see <http://www.gnu.org/licenses/>.
#

# -------------------------------------------------
# Project created by QtCreator 2010-01-10T18:43:06
# -------------------------------------------------

TARGET = messagemerge
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

# See: http://developer.symbian.org/forum/showthread.php?t=3768
default_deployment.pkg_prerules = \
        "; SIS header: name, uid, version" \
        $$LITERAL_HASH"{\"MessageMerge\"},(0xEa138c70),1,0,0" \    # test
#        $$LITERAL_HASH"{\"MessageMerge\"},(0x2002DA48),1,0,0" \    # real
        " " \
        "; Localised Vendor name" \
        "%{\"Soluvas\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Soluvas\"" \
        " " \
        "; Default HW/platform dependencies" \
        "[0x101F7961],0,0,0,{\"S60ProductID\"}" \
        "[0x102032BE],0,0,0,{\"S60ProductID\"}" \
        "[0x102752AE],0,0,0,{\"S60ProductID\"}" \
        "[0x1028315F],0,0,0,{\"S60ProductID\"}" \
        " "

symbian: {
    MOBILITY += messaging
    TARGET.CAPABILITY = ReadUserData \  # contacts
                        WriteUserData \ # contacts
                        LocalServices \
                        NetworkServices \ # SMS/MMS
                        UserEnvironment
#                        ReadDeviceData \
#                        WriteDeviceData \
#                        AllFiles \
#                        SwEvent
    TARGET.UID3 = 0xEa138c70    # test
#    TARGET.UID3 = 0x2002DA48   # real
}

# not available yet on Maemo
#maemo5 | contains(QT_CONFIG, hildon): {
#    MOBILITY += messaging
#}

linux-*: {
    #On Linux QMF is required to be installed and the QMF_INCLUDEDIR and QMF_LIBDIR environment variables
    #set to the messagingframework/src/libraries/qtopiamail and build/messagingframework/image/lib directories respectively.
    #QMF_INCLUDEDIR = "/opt/qt-messagingframework/src/libraries/qtopiamail"
    #QMF_LIBDIR = "/opt/qt-messagingframework/image/lib"
    #qmf_enabled = yes
    # Temporarily link against local qtopiamail lib (should be part of the platform)
    #LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
    #LIBS += -L /opt/qt-messagingframework/image/lib -lqtopiamail
}

win32: {
}
