#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_editTemplatesBtn_clicked()
{
    MessageTemplate tpl("generic name", "contents");
    templates.append(tpl);
    ui->templatesCombo->clear();
    QListIterator<MessageTemplate> i(templates);
    while (i.hasNext()) {
        MessageTemplate tpl = i.next();
        ui->templatesCombo->addItem(tpl.name());
    }
}
