#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include "templatesdialog.h"
#include "templateeditdialog.h"
#include <QErrorMessage>
#include "qtcontacts.h"
#include <QPointer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->navStack->setCurrentIndex(0);
    // sample data
    templates.insert("Weather news",
                     "Hello [[firstname]],\n\n"
                     "I'd like to let you know that weather in [[city]] is great!\n\n"
                     "Best regards,\n"
                     "Weather Control");
    templates.insert("New e-mail notification",
                     "Hi [[firstname]],\n\n"
                     "You have a new e-mail address: [[email]]\n\n"
                     "Good luck!");
    QtMobility::QContact contact;
    QtMobility::QContactName name;
    name.setFirst("John");
    name.setLast("Smith");
    contact.saveDetail(&name);
    contacts.append(contact);
    contact = QtMobility::QContact();
    name = QtMobility::QContactName();
    name.setFirst("Mary");
    name.setLast("Swanson");
    contact.saveDetail(&name);
    contacts.append(contact);

    refreshContactList();
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

QString MainWindow::templateBody() const {
    return ui->templateEdit->toPlainText();
}

void MainWindow::refreshMessagePreview() {
    int index = ui->previewContactCombo->currentIndex();
    if (index >= 0 && index < contacts.length()) {
        QContact contact = ui->previewContactCombo->itemData(index, Qt::UserRole).value<QContact>();
        QContactName contactName = contact.detail<QContactName>();
        QString message = templateBody();
        message = message.replace("[[name]]", contactName.first() + " " + contactName.last(), Qt::CaseSensitive);
        message = message.replace("[[firstname]]", contactName.first(), Qt::CaseSensitive);
        message = message.replace("[[lastname]]", contactName.last(), Qt::CaseSensitive);
        ui->messagePreviewEdit->setPlainText(message);
    } else {
        ui->messagePreviewEdit->setPlainText("(no contact selected)");
    }
}

void MainWindow::on_navStack_currentChanged(int )
{
    if (ui->navStack->currentWidget() == ui->previewPage) {
        refreshContactCombo();
        refreshMessagePreview();
    }
}

void MainWindow::refreshContactList() {
    ui->contactList->clear();
    for (int i = 0; i < contacts.length(); i++) {
        QContact contact = contacts[i];
        QContactName contactName = contact.detail<QContactName>();
        QListWidgetItem *item = new QListWidgetItem(contactName.first() + " " + contactName.last(),
                             ui->contactList);
        item->setData(Qt::UserRole, QVariant::fromValue(contact));
        qDebug() << i + 1 << "Contact:" << contact.id().localId() << "Item:" << item->text();
    }
}

void MainWindow::refreshContactCombo() {
    ui->previewContactCombo->clear();
    QListIterator<QContact> i(selectedContacts());
    while (i.hasNext()) {
        QContact contact = i.next();
        QContactName contactName = contact.detail<QContactName>();
        ui->previewContactCombo->addItem(contactName.first() + " " + contactName.last(),
                                         QVariant::fromValue(contact));
    }
}

QList<QContact> MainWindow::selectedContacts() {
    QList<QContact> selectedContacts;
    QList<QListWidgetItem *> sel = ui->contactList->selectedItems();
    QListIterator<QListWidgetItem *> i(sel);
    while (i.hasNext()) {
        QListWidgetItem *item = i.next();
        QContact contact = item->data(Qt::UserRole).value<QContact>();
        selectedContacts.append(contact);
    }
    return selectedContacts;
}
