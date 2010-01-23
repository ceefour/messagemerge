#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include "templatesdialog.h"
#include "templateeditdialog.h"
#include <QErrorMessage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    templates.insert("Weather news",
                                     "Hello [[firstname]],\n\n"
                                     "I'd like to let you know that weather in [[city]] is great!\n\n"
                                     "Best regards,\n"
                                     "Weather Control");
    templates.insert("New e-mail notification",
                                     "Hi [[firstname]],\n\n"
                                     "You have a new e-mail address: [[email]]\n\n"
                                     "Good luck!");
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
    TemplatesDialog templatesDlg(this, &templates);
    if (templatesDlg.exec() == QDialog::Accepted) {
    }
}

void MainWindow::on_previewBackBtn_clicked()
{
    ui->navStack->setCurrentWidget(ui->startPage);
}

void MainWindow::on_startPreviewBtn_clicked()
{
    ui->navStack->setCurrentWidget(ui->previewPage);
}

void MainWindow::on_saveAsTemplateBtn_clicked()
{
    TemplateEditDialog templateDlg(this);
    templateDlg.setWindowTitle("Save Template As");
    templateDlg.setTemplateBody(ui->templateEdit->toPlainText());
    if (templateDlg.exec() == QDialog::Accepted) {
        QString templateName = templateDlg.templateName();
        if (!templateName.isEmpty()) {
            templates.insert(templateName, templateDlg.templateBody());
        } else {
            QErrorMessage(this).showMessage("Template name must not be empty.");
        }
    }
}

void MainWindow::on_loadTemplateBtn_clicked()
{
    QInputDialog loadTemplateDlg(this, Qt::Dialog);
    loadTemplateDlg.setWindowTitle("Load Template");
    loadTemplateDlg.setInputMode(QInputDialog::TextInput);
    loadTemplateDlg.setLabelText("Template:");
    loadTemplateDlg.setComboBoxItems(templates.keys());
    if (loadTemplateDlg.exec() == QDialog::Accepted) {
        QString selectedName = loadTemplateDlg.textValue();
        if (templates.contains(selectedName)) {
            ui->templateEdit->setPlainText(templates[selectedName]);
        } else {
            QMessageBox msg(this);
            msg.setIcon(QMessageBox::Critical);
            msg.setText("Invalid template name: \"" + selectedName + "\".");
            msg.exec();
        }
    }
}
