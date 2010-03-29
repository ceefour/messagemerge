/*
Copyright 2010 Soluvas <http://www.soluvas.com>

This software is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwizard.h"
#include "ui_mainwizard.h"
#include "templatesdialog.h"
#include "templateeditdialog.h"
#include "messagemerger.h"

#include <QInputDialog>
#include <QErrorMessage>
#include <qtcontacts.h>
#include <QDebug>
#include <QDir>
#include <QSettings>

// FIXME: To get Qt Mobility Messaging app to compile,
// manually copy qtmessaging.h, qmessageglobal.h, qmessagedatacomparator.h
// to epoc32's include dir
#if defined(Q_OS_SYMBIAN)
// enable SMS on Symbian
#define SMS_ENABLED
#endif

#ifdef SMS_ENABLED
#include <qtmessaging.h>
#endif

MainWizard::MainWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MainWizard)
{
    ui->setupUi(this);
    setButtonText(CancelButton, "E&xit");

    connect(this, SIGNAL(currentIdChanged(int)), SLOT(handle_currentIdChanged(int)));

#ifdef SMS_ENABLED
    ui->sendTextRadio->setEnabled(true);
#endif

    QSettings settings("Soluvas", "MessageMerge", this);

    if (!settings.contains("templates/size")) {
        qDebug() << "Generating sample templates.";
        // sample templates
        settings.beginWriteArray("templates");
        settings.setArrayIndex(0);
        settings.setValue("name", "Weather news");
        settings.setValue("body", "Hello [[firstname]],\n\n"
                          "I'd like to let you know that weather in [[city]] is great!\n\n"
                          "Best regards,\n"
                          "Weather Control");
        settings.setArrayIndex(1);
        settings.setValue("name", "New e-mail notification");
        settings.setValue("body", "Hi [[firstname]],\n\n"
                          "You have a new e-mail address: [[email]]\n\n"
                          "Good luck!");
        settings.endArray();
    }

    reloadTemplates();
}

void MainWizard::reloadTemplates() {
    QSettings settings("Soluvas", "MessageMerge", this);
    templates.clear();
    int templateCount = settings.beginReadArray("templates");
    for (int i = 0; i < templateCount; i++) {
        settings.setArrayIndex(i);
        templates.insert(settings.value("name").toString(), settings.value("body").toString());
    }
    settings.endArray();
    qDebug() << "Loaded" << templateCount << "templates from settings.";
}

void MainWizard::saveTemplates() {
    QSettings settings("Soluvas", "MessageMerge", this);
    settings.beginWriteArray("templates");
    QMapIterator<QString, QString> i(templates);
    int idx = 0;
    while (i.hasNext()) {
        i.next();
        settings.setArrayIndex(idx);
        settings.setValue("name", i.key());
        settings.setValue("body", i.value());
        idx++;
    }
    settings.endArray();
    qDebug() << "Saved" << idx << "templates to settings.";
}

MainWizard::~MainWizard()
{
    delete ui;
}

void MainWizard::setContactManager(QContactManager *contactManager) {
    m_contactManager = contactManager;
    qDebug() << "Using manager:" << m_contactManager->managerName();
}

void MainWizard::reloadContacts() {
    contacts.clear();
    if (m_contactManager == NULL)
        return;

    int contactCount = m_contactManager->contacts().length();
    if (m_contactManager->managerName() == "memory" && contactCount == 0) {
        qDebug() << "Using manager 'memory' and empty, adding sample contacts data.";
        // sample contacts
        QtMobility::QContact contact;
        QtMobility::QContactName name;
        name.setFirst("John");
        name.setLast("Smith");
        contact.saveDetail(&name);
        QContactPhoneNumber phone;
        phone.setNumber("+628123456789");
        phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        contact.saveDetail(&phone);
        QContactEmailAddress email;
        email.setEmailAddress("john.smith@example.com");
        contact.saveDetail(&email);
        QContactAddress address;
        address.setLocality("Medan");
        contact.saveDetail(&address);
        m_contactManager->saveContact(&contact);
        contact = QtMobility::QContact();
        name = QtMobility::QContactName();
        name.setFirst("Mary");
        name.setLast("Swanson");
        contact.saveDetail(&name);
        phone = QContactPhoneNumber();
        phone.setNumber("+6285678912345");
        phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        contact.saveDetail(&phone);
        email = QContactEmailAddress();
        email.setEmailAddress("mary.swanson@example.com");
        contact.saveDetail(&email);
        address = QContactAddress();
        address.setLocality("Bandung");
        contact.saveDetail(&address);
        m_contactManager->saveContact(&contact);
    }

    qDebug() << "Reading contacts...";
    QListIterator<QContactLocalId> i(m_contactManager->contacts());
    int idx = 0;
    while (i.hasNext()) {
        ui->contactsPage->setSubTitle("Reading contact #" + QString(idx + 1) + " of " + QString(contactCount));
        update();
        QContactLocalId contactId = i.next();
        try {
            QContact contact;
            // TODO: How to catch this error? It still just terminates on Nokia E71! :(
#ifdef Q_OS_SYMBIAN
            QT_TRAP_THROWING(contact = m_contactManager->contact(contactId));
#else
            contact = m_contactManager->contact(contactId);
#endif
            contacts.append(contact);
        } catch (std::exception ex) {
            QErrorMessage(this).showMessage("Cannot load contact ID " + QString::number(contactId) + ": " + ex.what());
        }
        idx++;
    }
    ui->contactsPage->setSubTitle("Select contacts to receive the message.");
    qDebug() << "Contacts loaded.";

    refreshContactList();
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
#ifdef Q_OS_SYMBIAN
    templatesDlg.setWindowState(Qt::WindowMaximized);
#endif
    if (templatesDlg.exec() == QDialog::Accepted) {
        saveTemplates();
    }
}

void MainWizard::on_saveAsTemplateBtn_clicked()
{
    TemplateEditDialog templateDlg(this);
    templateDlg.setWindowTitle("Save Template As");
    templateDlg.setTemplateBody(ui->templateEdit->toPlainText());
#ifdef Q_OS_SYMBIAN
    templateDlg.setWindowState(Qt::WindowMaximized);
#endif
    if (templateDlg.exec() == QDialog::Accepted) {
        QString templateName = templateDlg.templateName();
        if (!templateName.isEmpty()) {
            templates.insert(templateName, templateDlg.templateBody());
            saveTemplates();
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
            QErrorMessage(this).showMessage("Invalid template name: \"" + selectedName + "\".");
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
        QListWidgetItem *item = new QListWidgetItem(contact.displayLabel(),
                             ui->contactList);
        item->setData(Qt::UserRole, QVariant::fromValue(contact));
        qDebug() << i + 1 << ":" << contact.localId() << ":" << item->text();
    }
}

void MainWizard::refreshContactCombo() {
    ui->previewContactCombo->clear();
    QListIterator<QContact> i(selectedContacts());
    while (i.hasNext()) {
        QContact contact = i.next();
        ui->previewContactCombo->addItem(contact.displayLabel(),
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
    if (currentPage() == ui->contactsPage) {
        reloadContacts();
    } else if (currentPage() == ui->previewPage) {
        refreshContactCombo();
        refreshMessagePreview();
    } else if (currentPage() == ui->generatePage) {

    } else if (currentPage() == ui->processingPage) {
        if (ui->saveFilesRadio->isChecked())
            processSaveFiles();
        if (ui->sendTextRadio->isChecked())
            processSendTextMessages();
        if (ui->draftTextRadio->isChecked())
            processDraftTextMessages();
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
    MessageMerger merger;
    foreach (const QContact& contact, selectedContacts()) {
        ui->processingLabel2->setText(contact.displayLabel());
        update();
        QString message = merger.merge(templateBody(), contact);
        QString fileName = targetDir + "/" + contact.displayLabel() + ".txt";
        qDebug() << fileName << "Contact:" << contact.displayLabel() << "Message:" << message;
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(message.toUtf8());
            file.close();
        } else
            QErrorMessage(this).showMessage("Error writing file " + fileName);
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        update();
    }
    ui->processingLabel2->setText("");

    next();
}

void MainWizard::processSendTextMessages() {
#ifdef SMS_ENABLED
    MessageMerger merger;
    ui->progressBar->setMaximum(selectedContacts().length());
    ui->progressBar->setValue(0);
    ui->processingLabel1->setText("Sending text messages");
    update();
    foreach (const QContact &contact, selectedContacts()) {
        ui->processingLabel2->setText(contact.displayLabel());
        update();
        QMessage message;
        message.setType(QMessage::Sms);
        QContactPhoneNumber phone = contact.detail<QContactPhoneNumber>();
        message.setTo(QMessageAddress(phone.number(), QMessageAddress::Phone));
        QString body = merger.merge(templateBody(), contact);
        message.setBody(body);
        QMessageServiceAction service(this);
        service.send(message);
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        update();
    }
    ui->processingLabel2->setText("");
    update();
    next();
#endif
}

void MainWizard::processDraftTextMessages() {
#ifdef SMS_ENABLED
    // TODO: no API for save as draft
//    QMessageStore *store = QMessageStore::instance();
//    MessageMerger merger;
//    ui->progressBar->setMaximum(selectedContacts().length());
//    ui->progressBar->setValue(0);
//    ui->processingLabel1->setText("Saving text messages as draft");
//    update();
//    foreach (const QContact &contact, selectedContacts()) {
//        ui->processingLabel2->setText(contact.displayLabel());
//        update();
//        QMessage message;
//        message.setType(QMessage::Sms);
//        QContactPhoneNumber phone = contact.detail<QContactPhoneNumber>();
//        message.setTo(QMessageAddress(phone.number(), QMessageAddress::Phone));
//        QString body = merger.merge(templateBody(), contact);
//        message.setBody(body);
//        message.setStatus();
//        store->addMessage(m);
//        ui->progressBar->setValue(ui->progressBar->value() + 1);
//        update();
//    }
//    ui->processingLabel2->setText("");
//    update();
//    next();
#endif
}

void MainWizard::setOutputDir(const QString& outputDir) {
    ui->folderEdit->setText(outputDir);
}
