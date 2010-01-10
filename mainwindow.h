#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "messagetemplate.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QList<MessageTemplate> templates;
    void refreshTemplatesCombo();

private slots:
    void on_startPreviewBtn_clicked();
    void on_previewBackBtn_clicked();
    void on_editTemplatesBtn_clicked();
};

#endif // MAINWINDOW_H
