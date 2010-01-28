#include <QtGui/QApplication>
#include "mainwizard.h"
#include <qtcontacts.h>
#include <qinputdialog.h>

QTM_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList managers = QContactManager::availableManagers();
    QInputDialog managerDlg;
    managerDlg.setWindowTitle("Choose Contact Manager");
    managerDlg.setLabelText("Select the Contact Manager that you want to use:");
    managerDlg.setComboBoxItems(managers);
    if (managerDlg.exec() == QDialog::Accepted) {
        MainWizard w;
        w.setContactManager(new QContactManager(managerDlg.textValue(), QMap<QString, QString>(), &a));
        w.setOutputDir(a.applicationDirPath() + "/output");
#ifdef Q_OS_SYMBIAN
        w.showMaximized();
#else
        w.show();
#endif
        return a.exec();
    } else
        return 1;
}
