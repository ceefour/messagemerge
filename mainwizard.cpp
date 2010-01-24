#include "mainwizard.h"
#include "ui_mainwizard.h"
#include <QMessageBox>
#include <QInputDialog>
#include "templatesdialog.h"
#include "templateeditdialog.h"
#include <QErrorMessage>
#include "qtcontacts.h"
#include <QDebug>
#include <QDir>
#include "messagemerger.h"

MainWizard::MainWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MainWizard)
{
    ui->setupUi(this);

    connect(this, SIGNAL(currentIdChanged(int)), SLOT(handle_currentIdChanged(int)));

    // sample templates
    templates.insert("Weather news",
                     "Hello [[firstname]],\n\n"
                     "I'd like to let you know that weather in [[city]] is great!\n\n"
                     "Best regards,\n"
                     "Weather Control");
    templates.insert("New e-mail notification",
                     "Hi [[firstname]],\n\n"
                     "You have a new e-mail address: [[email]]\n\n"
                     "Good luck!");

    // sample contacts
    QtMobility::QContact contact;
    QtMobility::QContactName name;
    name.setFirst("John");
    name.setLast("Smith");
    contact.saveDetail(&name);
    QContactEmailAddress email;
    email.setEmailAddress("john.smith@example.com");
    contact.saveDetail(&email);
    QContactAddress address;
    address.setLocality("Medan");
    contact.saveDetail(&address);
    contacts.append(contact);
    contact = QtMobility::QContact();
    name = QtMobility::QContactName();
    name.setFirst("Mary");
    name.setLast("Swanson");
    contact.saveDetail(&name);
    email = QContactEmailAddress();
    email.setEmailAddress("mary.swanson@example.com");
    contact.saveDetail(&email);
    address = QContactAddress();
    address.setLocality("Bandung");
    contact.saveDetail(&address);
    contacts.append(contact);

    refreshContactList();
}

MainWizard::~MainWizard()
{
    delete ui;
}

void MainWizard::changeEvent(QEvent *e)
{
    QWizard::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWizard::on_editTemplatesBtn_clicked()
{
    TemplatesDialog templatesDlg(this, &templates);
    if (templatesDlg.exec() == QDialog::Accepted) {
    }
}

void MainWizard::on_saveAsTemplateBtn_clicked()
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

void MainWizard::on_loadTemplateBtn_clicked()
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

QString MainWizard::templateBody() const {
    return ui->templateEdit->toPlainText();
}

void MainWizard::refreshMessagePreview() {
    int index = ui->previewContactCombo->currentIndex();
    if (index >= 0 && index < contacts.length()) {
        QContact contact = ui->previewContactCombo->itemData(index, Qt::UserRole).value<QContact>();
        MessageMerger merger;
        QString message = merger.merge(templateBody(), contact);
        ui->messagePreviewEdit->setPlainText(message);
    } else {
        ui->messagePreviewEdit->setPlainText("(no contact selected)");
    }
}

void MainWizard::refreshContactList() {
    ui->contactList->clear();
    for (int i = 0; i < contacts.length(); i++) {
        QContact contact = contacts[i];
        QContactName contactName = contact.detail<QContactName>();
        QListWidgetItem *item = new QListWidgetItem(contactName.first() + " " + contactName.last(),
                             ui->contactList);
        item->setData(Qt::UserRole, QVariant::fromValue(contact));
        qDebug() << i + 1 << ":" << item->text();
    }
}

void MainWizard::refreshContactCombo() {
    ui->previewContactCombo->clear();
    QListIterator<QContact> i(selectedContacts());
    while (i.hasNext()) {
        QContact contact = i.next();
        QContactName contactName = contact.detail<QContactName>();
        ui->previewContactCombo->addItem(contactName.first() + " " + contactName.last(),
                                         QVariant::fromValue(contact));
    }
}

QList<QContact> MainWizard::selectedContacts() {
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

void MainWizard::handle_currentIdChanged(int id)
{
    qDebug() << "wizard ID changed" << id;
    if (currentPage() == ui->previewPage) {
        refreshContactCombo();
        refreshMessagePreview();
    } else if (currentPage() == ui->generatePage) {

    } else if (currentPage() == ui->processingPage) {
        if (ui->saveFilesRadio->isChecked())
            processSaveFiles();
    }
}

void MainWizard::processSaveFiles() {
    QString targetDir = ui->folderEdit->text();
    ui->processingLabel1->setText("Creating directory " + targetDir);
    ui->processingLabel2->setText("");
    update();
    QDir dir(targetDir);
    if (!dir.exists()) {
        if (!dir.mkpath(targetDir)) {
            QErrorMessage(this).showMessage("Error creating directory " + targetDir);
            return;
        }
    }

    ui->progressBar->setMaximum(selectedContacts().length());
    ui->progressBar->setValue(0);
    ui->processingLabel1->setText("Processing contacts");
    update();
    QListIterator<QContact> i(selectedContacts());
    MessageMerger merger;
    while (i.hasNext()) {
        QContact contact = i.next();
        QContactName contactName = contact.detail<QContactName>();
        ui->processingLabel2->setText(contactName.first() + " " + contactName.last());
        update();
        QString message = merger.merge(templateBody(), contact);
        QString fileName = targetDir + "/" + contactName.first() + "_" + contactName.last() + ".txt";
        qDebug() << fileName << "Contact:" << contactName.first() + " " + contactName.last() << "Message:" << message;
        QFile file(fileName);
        if (file.open(QFile::WriteOnly)) {
            file.write(message.toUtf8());
            file.close();
        } else
            QErrorMessage(this).showMessage("Error writing file " + fileName);
        ui->progressBar->setValue(ui->progressBar->value() + 1);
    }
    ui->processingLabel2->setText("");
    update();
}
