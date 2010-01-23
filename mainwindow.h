#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QContact>
#include "messagetemplate.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QContact)

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString templateBody() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QMap<QString, QString> templates;
    QList<QtMobility::QContact> contacts;
    QList<QtMobility::QContact> selectedContacts();

private slots:
    void on_MainWindow_iconSizeChanged(QSize iconSize);
    void on_navStack_currentChanged(int );
    void on_loadTemplateBtn_clicked();
    void on_saveAsTemplateBtn_clicked();
    void on_startPreviewBtn_clicked();
    void on_previewBackBtn_clicked();
    void on_editTemplatesBtn_clicked();
    void refreshMessagePreview();
    void refreshContactList();
    void refreshContactCombo();
};

#endif // MAINWINDOW_H
