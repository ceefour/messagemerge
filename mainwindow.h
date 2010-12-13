#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "qtcontacts.h"
#include "contactlistmodel.h"
#include <QStandardItemModel>
#include "messagemergeutils.h"

QTM_USE_NAMESPACE

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setContactManager(const QString &name);

private:
    QContactManager *contactManager;
    QMap<QString, QString> templates;
    ContactListModel *contactModel;
    //QStandardItemModel *contacts;
    MessageMergeUtils util;

    Ui::MainWindow *ui;

    void initTemplates();
    void reloadTemplates();
    void saveTemplates();
    void reloadContacts();

private slots:
    void on_actionHelp_triggered();
    void on_actionAbout_triggered();
    void on_actionSend_triggered();
    void on_actionSaveTemplate_triggered();
    void on_actionEditTemplates_triggered();
    void on_actionRecipients_triggered();
    void on_actionLoadTemplate_triggered();
};

#endif // MAINWINDOW_H
