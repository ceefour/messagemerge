/*
Copyright 2010 Soluvas <http://www.soluvas.com>

This software is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui/QApplication>
#include "mainwizard.h"
#include <qtcontacts.h>
#include <qinputdialog.h>
#include <qdir.h>
#include <qicon.h>

QTM_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/logo.png"));

    QStringList managers = QContactManager::availableManagers();
    QInputDialog managerDlg;
    managerDlg.setWindowTitle("Contact Manager Selection");
    managerDlg.setLabelText("Select Phonebook:");
    managerDlg.setComboBoxItems(managers);
    if (managerDlg.exec() == QDialog::Accepted) {
        MainWizard w;
        w.setContactManager(new QContactManager(managerDlg.textValue(), QMap<QString, QString>(), &a));
        w.setOutputDir(QDir::homePath() + "/output");
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_HILDON) || defined(Q_WS_MAEMO_5)
        w.showMaximized();
#else
        w.show();
#endif
        return a.exec();
    } else
        return 1;
}
