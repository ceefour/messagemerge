#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMap<QString, QString> templates;
    void reloadTemplates();
    void saveTemplates();

private slots:
    void on_actionLoadTemplate_triggered();
    void on_actionLoad_triggered();
};

#endif // MAINWINDOW_H
