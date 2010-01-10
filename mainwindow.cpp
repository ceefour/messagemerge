#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    templates.append(MessageTemplate("Weather news",
                                     "Hello [[firstname]],\n\n"
                                     "I'd like to let you know that weather in [[city]] is great!\n\n"
                                     "Best regards,\n"
                                     "Weather Control"));
    templates.append(MessageTemplate("New e-mail notification",
                                     "Hi [[firstname]],\n\n"
                                     "You have a new e-mail address: [[email]]\n\n"
                                     "Good luck!"));
    refreshTemplatesCombo();
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

void MainWindow::refreshTemplatesCombo() {
    ui->templatesCombo->clear();
    QListIterator<MessageTemplate> i(templates);
    while (i.hasNext()) {
        MessageTemplate tpl = i.next();
        ui->templatesCombo->addItem(tpl.name());
    }
}

void MainWindow::on_editTemplatesBtn_clicked()
{
    QString str = templates[0].text().replace("[[firstname]]", "Jenny Hong");
    str = str.replace("[[city]]", "Tokyo");
    QMessageBox msg;
    msg.setText(str);
    msg.exec();
    MessageTemplate tpl("generic name", "contents");
    templates.append(tpl);
    refreshTemplatesCombo();
}

void MainWindow::on_previewBackBtn_clicked()
{
    ui->navStack->setCurrentWidget(ui->startPage);
}

void MainWindow::on_startPreviewBtn_clicked()
{
    ui->navStack->setCurrentWidget(ui->previewPage);
}
