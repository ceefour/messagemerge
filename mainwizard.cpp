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

// enable SMS on Symbian
#if defined(Q_OS_SYMBIAN)
#define SMS_ENABLED
#endif

#ifdef SMS_ENABLED
#include <qmessage.h>
#include <qmessageaddress.h>
#include <qmessageservice.h>
#endif

MainWizard::MainWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MainWizard)
{
    ui->setupUi(this);
    setButtonText(CancelButton, "E&xit");

    connect(this, SIGNAL(currentIdChanged(int)), SLOT(handle_currentIdChanged(int)));

    // FIXME: When Qt Mobility fixes this, enable send SMS and change save as draft
    ui->sendTextRadio->setVisible(false);
#ifdef SMS_ENABLED
    ui->draftTextRadio->setEnabled(true);
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

    int contactCount = m_contactManager->contactIds().length();
    if (m_contactManager->managerName() == "memory" && contactCount == 0) {
        qDebug() << "Using manager 'memory' and empty, adding sample contacts data.";
        // sample contacts
        QtMobility::QContact contact;
        QtMobility::QContactName name;
        name.setFirstName("John");
        name.setLastName("Smith");
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
        name.setFirstName("Mary");
        name.setLastName("Swanson");
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

    qDebug() << "Reading contacts (filtered, sorted)...";

    // Filter only contact types (i.e. exclude groups)
    QContactDetailFilter typeFilter;
    typeFilter.setDetailDefinitionName(QContactType::DefinitionName);
    typeFilter.setValue(QLatin1String(QContactType::TypeContact));
    typeFilter.setMatchFlags(QContactFilter::MatchExactly);

    // Sort by First, then Last name
    QContactSortOrder sortFirst;
    sortFirst.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    QContactSortOrder sortLast;
    sortLast.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
    QList<QContactSortOrder> sortOrder;
    sortOrder.append(sortFirst);
    sortOrder.append(sortLast);

    QList<QContactLocalId> contactIds = m_contactManager->contactIds(sortOrder);
    QListIterator<QContactLocalId> i(contactIds);
    int idx = 0;
    while (i.hasNext()) {
        ui->contactsPage->setSubTitle("Reading contact #" + QString(idx + 1) + " of " + QString(contactCount));
        idx++;
        update();
        QContactLocalId contactId = i.next();
        qDebug() << "Reading id=" << contactId;
        try {
            QContact contact;
            // TODO: How to catch this error? It still just terminates on Nokia E71! :(
            contact = m_contactManager->contact(contactId);
            QContactNickname nick = contact.detail<QContactNickname>();
            qDebug() << "Display=" << contact.displayLabel() << " merged=" << MessageMerger::fullName(contact.detail<QContactName>()) << " nick=" << nick.nickname();
            contacts.append(contact);
        } catch (std::exception ex) {
            QErrorMessage(this).showMessage("Cannot load contact ID " + QString::number(contactId) + ": " + ex.what());
        }
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
        QListWidgetItem *item = new QListWidgetItem(contact.displayLabel(), ui->contactList);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, QVariant::fromValue(contact));
        qDebug() << i + 1 << ":" << contact.localId() << ":" << item->text();
    }
}

void MainWizard::refreshContactCombo() {
    ui->previewContactCombo->clear();
    QListIterator<QContact> i(checkedContacts());
    while (i.hasNext()) {
        QContact contact = i.next();
        ui->previewContactCombo->addItem(contact.displayLabel(),
                                         QVariant::fromValue(contact));
    }
}

QList<QContact> MainWizard::checkedContacts() {
    QList<QContact> checkedContacts;
    QListIterator<QListWidgetItem *> i(ui->contactList->findItems("", Qt::MatchStartsWith));
    while (i.hasNext()) {
        QListWidgetItem *item = i.next();
        if (item->checkState() == Qt::Checked) {
            QContact contact = item->data(Qt::UserRole).value<QContact>();
            checkedContacts.append(contact);
        }
    }
    return checkedContacts;
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

    ui->progressBar->setMaximum(checkedContacts().length());
    ui->progressBar->setValue(0);
    ui->processingLabel1->setText("Processing contacts");
    update();
    MessageMerger merger;
    foreach (const QContact& contact, checkedContacts()) {
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

/**
  * As of Qt 4.6.2 and Qt Mobility 1.0.0-beta1
  * the send() code only save to draft.
  * Confirmed with Qt Mobility's own writemessage example.
  * Tested with Nokia E71 v400.
  */
void MainWizard::processSendTextMessages() {
#ifdef SMS_ENABLED
    MessageMerger merger;
    ui->progressBar->setMaximum(checkedContacts().length());
    ui->progressBar->setValue(0);
    ui->processingLabel1->setText("Sending text messages");
    update();
    foreach (const QContact &contact, checkedContacts()) {
        ui->processingLabel2->setText(contact.displayLabel());
        update();
        QMessage message;
        message.setType(QMessage::Sms);
        QContactPhoneNumber phone = contact.detail<QContactPhoneNumber>();
        message.setTo(QMessageAddress(QMessageAddress::Phone, phone.number()));
        QString body = merger.merge(templateBody(), contact);
        message.setBody(body);
        QMessageService service(this);
        service.send(message);
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        update();
    }
    ui->processingLabel2->setText("");
    update();
    next();
#endif
}

/**
  * As of Qt 4.6.2 and Qt Mobility 1.0.0-beta1
  * the send() code only save to draft.
  * Confirmed with Qt Mobility's own writemessage example.
  * Tested with Nokia E71 v400.
  */
void MainWizard::processDraftTextMessages() {
#ifdef SMS_ENABLED
    MessageMerger merger;
    ui->progressBar->setMaximum(checkedContacts().length());
    ui->progressBar->setValue(0);
    ui->processingLabel1->setText("Saving text messages as draft");
    update();
    foreach (const QContact &contact, checkedContacts()) {
        ui->processingLabel2->setText(contact.displayLabel());
        update();
        QMessage message;
        message.setType(QMessage::Sms);
        QContactPhoneNumber phone = contact.detail<QContactPhoneNumber>();
        message.setTo(QMessageAddress(QMessageAddress::Phone, phone.number()));
        QString body = merger.merge(templateBody(), contact);
        message.setBody(body);
        QMessageService service(this);
        service.send(message);
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        update();
    }
    ui->processingLabel2->setText("");
    update();
    next();
#endif
}

void MainWizard::setOutputDir(const QString& outputDir) {
    ui->folderEdit->setText(outputDir);
}
