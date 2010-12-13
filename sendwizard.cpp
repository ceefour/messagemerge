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

#include "sendwizard.h"
#include "ui_sendwizard.h"
#include "templatesdialog.h"
#include "templateeditdialog.h"
#include "messagemerger.h"
#include <stdexcept>

#include <QInputDialog>
#include <QErrorMessage>
#include <qtcontacts.h>
#include <QDebug>
#include <QDir>
#include <QSettings>

// enable SMS on Symbian
#if defined(Q_OS_SYMBIAN)
#define SMS_ENABLED
#else
#endif

#ifdef SMS_ENABLED
#include <QMessage>
#include <QMessageAddress>
#include <QMessageService>
#include <QMessageAccount>
#endif

SendWizard::SendWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::SendWizard)
{
    ui->setupUi(this);

    connect(this, SIGNAL(currentIdChanged(int)), SLOT(handle_currentIdChanged(int)));

    ui->folderEdit->setText(QDir::toNativeSeparators(QDir::homePath() + "/messagemerge-output"));

    ui->draftTextRadio->setVisible(false);
    ui->draftTextRadio->setEnabled(false);

#if !defined(SMS_ENABLED)
    ui->sendTextRadio->setEnabled(false);
#endif
}

SendWizard::~SendWizard()
{
    delete ui;
}

//void SendWizard::setContactManager(QContactManager *contactManager) {
//    m_contactManager = contactManager;
//    qDebug() << "SendWizard::setContactManager:" << m_contactManager->managerName()
//            << m_contactManager->managerVersion() << m_contactManager->managerUri();
//}

void SendWizard::setCheckedContacts(const QList<QContact> &checkedContacts) {
    this->checkedContacts = checkedContacts;
//    refreshContactList();
}

void SendWizard::changeEvent(QEvent *e)
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

void SendWizard::setTemplateBody(const QString& templateBody) {
    this->templateBody = templateBody;
}

void SendWizard::preDisplay() {
    refreshContactCombo();
    refreshMessagePreview();
}

void SendWizard::refreshMessagePreview() {
    int index = ui->previewContactCombo->currentIndex();
    if (index >= 0 && index < checkedContacts.length()) {
        QContact contact = ui->previewContactCombo->itemData(index, Qt::UserRole).value<QContact>();
        MessageMerger merger;
        QString message = merger.merge(templateBody, contact);
        ui->messagePreviewEdit->setPlainText(message);
    } else {
        ui->messagePreviewEdit->setPlainText("(no contact selected)");
    }
}

void SendWizard::refreshContactCombo() {
    ui->previewContactCombo->clear();
    QListIterator<QContact> i(checkedContacts);
    while (i.hasNext()) {
        QContact contact = i.next();
        ui->previewContactCombo->addItem(contact.displayLabel(),
                                         QVariant::fromValue(contact));
    }
}

void SendWizard::handle_currentIdChanged(int id)
{
    qDebug() << "wizard ID changed" << id;
    if (currentPage() == ui->previewPage) {
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

void SendWizard::processSaveFiles() {
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

    ui->progressBar->setMaximum(checkedContacts.length());
    ui->progressBar->setValue(0);
    ui->processingLabel1->setText("Processing contacts");
    update();
    MessageMerger merger;
    foreach (const QContact& contact, checkedContacts) {
        ui->processingLabel2->setText(contact.displayLabel());
        update();
        QString message = merger.merge(templateBody, contact);
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
  * As of Qt 4.6.2 and Qt Mobility 1.0.0
  * the send() code only save to draft, *if* using Sms format.
  * Confirmed with Qt Mobility's own writemessage example.
  * Tested with Nokia E71 v400.
  * A workaround is setting parent account id as MMS.
  */
void SendWizard::processSendTextMessages() {
#ifdef SMS_ENABLED
    MessageMerger merger;
    ui->progressBar->setMaximum(checkedContacts.length());
    ui->progressBar->setValue(0);
    ui->processingLabel1->setText("Sending text messages");
    update();
    foreach (const QContact &contact, checkedContacts) {
        ui->processingLabel2->setText(contact.displayLabel());
        update();
        QMessage message;
        message.setType(QMessage::Sms);
        QContactPhoneNumber phone = contact.detail<QContactPhoneNumber>();
        message.setTo(QMessageAddress(QMessageAddress::Phone, phone.number()));
        QString body = merger.merge(templateBody, contact);
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
  * As of Qt 4.6.2 and Qt Mobility 1.0.0
  * the send() code only save to draft.
  * Confirmed with Qt Mobility's own writemessage example.
  * Tested with Nokia E71 v400.
  */
void SendWizard::processDraftTextMessages() {
#ifdef SMS_ENABLED
    MessageMerger merger;
    ui->progressBar->setMaximum(checkedContacts.length());
    ui->progressBar->setValue(0);
    ui->processingLabel1->setText("Saving text messages as draft");
    update();
    foreach (const QContact &contact, checkedContacts) {
        ui->processingLabel2->setText(contact.displayLabel());
        update();
        QMessage message;
        message.setType(QMessage::Sms);
        QContactPhoneNumber phone = contact.detail<QContactPhoneNumber>();
        message.setTo(QMessageAddress(QMessageAddress::Phone, phone.number()));
        QString body = merger.merge(templateBody, contact);
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

void SendWizard::setOutputDir(const QString& outputDir) {
    ui->folderEdit->setText(outputDir);
}

void SendWizard::on_previewContactCombo_currentIndexChanged(int index)
{
    refreshMessagePreview();
}
