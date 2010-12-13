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
//#include "mainwizard.h"
#include "mainwindow.h"
#include <qtcontacts.h>
#include <qinputdialog.h>
#include <qdir.h>
#include <qicon.h>
#include <QErrorMessage>
#include <QMessageBox>
#include <QDebug>

QTM_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/logo.png"));

    QString chosenManager = "";
    QStringList managers = QContactManager::availableManagers();
    foreach(QString manager, managers) {
        qDebug() << "Found manager:" << manager;
        if (manager == "invalid" || manager == "memory")
            continue;
        chosenManager = manager;
        qDebug() << "Chosen manager:" << chosenManager;
        break;
    }
    if (chosenManager.isEmpty()) {
        qWarning() << "Cannot find valid Contact Manager that is not 'invalid' or 'memory'.";
//        QErrorMessage errMsg;
//        errMsg.showMessage("Cannot find valid Contact Manager that is not 'invalid' or 'memory'.");
//        errMsg.exec();
//        return 1;
        chosenManager = "memory";
    }

//    QInputDialog managerDlg;
//    managerDlg.setWindowTitle("Contact Manager Selection");
//    managerDlg.setLabelText("Select Phonebook:");
//    managerDlg.setComboBoxItems(managers);
//    if (managerDlg.exec() == QDialog::Accepted) {

    MainWindow w;
    w.setContactManager(chosenManager);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_HILDON) || defined(Q_WS_MAEMO_5)
    w.showMaximized();
#else
    w.show();
#endif

//        MainWizard w;
//        w.setContactManager(new QContactManager(chosenManager, QMap<QString, QString>(), &a));
//        w.setOutputDir(QDir::homePath() + "/output");
//#if defined(Q_OS_SYMBIAN) || defined(Q_WS_HILDON) || defined(Q_WS_MAEMO_5)
//        w.showMaximized();
//#else
//        w.show();
//#endif

    return a.exec();
//    } else
//        return 1;
}
