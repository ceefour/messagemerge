#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QSettings>
#include <recipientsdlg.h>
#include <QDebug>
#include <QErrorMessage>
#include <QProgressDialog>
#include <stdexcept>
#include "messagemerger.h"
#include "templateeditdialog.h"
#include "templatesdialog.h"
#include "sendwizard.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), contactManager(), contactModel(),
    ui(new Ui::MainWindow)
{
    //contactManager = 0;
    //contacts = 0;
    ui->setupUi(this);

    initTemplates();
    reloadTemplates();

    ui->templateEdit->setFocus();
    ui->templateEdit->textCursor().movePosition(QTextCursor::End);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTemplates() {
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
}

void MainWindow::reloadTemplates() {
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

void MainWindow::saveTemplates() {
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

void MainWindow::on_actionLoadTemplate_triggered()
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

void MainWindow::reloadContacts() {
    qDebug() << "reloadContacts() called";
    QProgressDialog progress(this);
    progress.setWindowTitle("Loading Contacts");
    progress.setMinimum(0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMaximum(2);
    progress.setLabelText("Loading contacts...");
    progress.show();

    if (contactModel != 0) {
        qDebug() << "Deleting contacts";
        delete contactModel;
        contactModel = 0;
    }
    // I think using Filter makes it so slow under Symbian
    //this->sortedIds = mgr->contactIds(util.contactFilter(), util.contactSortOrder());
    qDebug() << "Loading contacts from ContactManager";
    QList<QContactLocalId> sortedIds = contactManager->contactIds(util.contactSortOrder());
    int contactCount = sortedIds.count();
    contactModel = new ContactListModel(this);// new QStandardItemModel(contactCount, 1, this);
    qDebug() << "Got" << contactCount << "contacts";
    progress.setMaximum(1 + contactCount);
    QListIterator<QContactLocalId> it(sortedIds);
    int idx = 0;
    while (it.hasNext()) {
        if (idx % 25 == 0) {
            progress.setValue(idx + 1);
            progress.setLabelText("Loading contacts "+ QString::number(idx+1) + " of " + QString::number(contactCount) + "...");
        }
        if (progress.wasCanceled())
            break;

        //update();
        QContactLocalId contactId = it.next();
        //qDebug() << "Reading id=" << contactId;
        try {
            QContact contact;
            // TODO: How to catch this error? It still just terminates on Nokia E71! :(
            contact = contactManager->contact(contactId);
            //QContactNickname nick = contact.detail<QContactNickname>();
            //qDebug() << "Display=" << contact.displayLabel() << " merged=" << MessageMerger::fullName(contact.detail<QContactName>());// << " nick=" << nick.nickname();
            //contacts.append(contact);
            /*
            QStandardItem *item = new QStandardItem(MessageMerger::fullName(contact.detail<QContactName>()));
            item->setData(contactId);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            contacts->setItem(idx, item);
            */
            contactModel->addItem(MessageMerger::fullName(contact.detail<QContactName>()), contactId);
        } catch (std::exception ex) {
            QErrorMessage(this).showMessage("Cannot load contact ID " + QString::number(contactId) + ": " + ex.what());
        }

        idx++;
    }
    progress.close();
}

void MainWindow::on_actionRecipients_triggered()
{
    if (contactModel == 0)
        reloadContacts();
    RecipientsDlg dlg(this, contactModel);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_HILDON) || defined(Q_WS_MAEMO_5)
    dlg.showMaximized();
#endif
    dlg.exec();
}

void MainWindow::setContactManager(const QString &name) {
    if (this->contactManager != 0) {
        if (this->contactModel != 0) {
            delete contactModel;
            contactModel = 0;
        }
        qDebug() << "Destroying ContactManager";
        delete contactManager;
        this->contactManager = 0;
    }
    qDebug() << "Loading ContactManager:" << name;
    this->contactManager = new QContactManager(name, QMap<QString, QString>(), this);
    qDebug() << "MainWindow::setContactManager:" << contactManager->managerName()
            << contactManager->managerVersion() << contactManager->managerUri();
    qDebug() << "Loading sample contacts if needed";
    util.loadSampleIfNeeded(contactManager);
    //contactModel = new ContactListModel(this, contactManager);
}

void MainWindow::on_actionEditTemplates_triggered()
{
    TemplatesDialog templatesDlg(this, &templates);
#ifdef Q_OS_SYMBIAN
    templatesDlg.setWindowState(Qt::WindowMaximized);
#endif
    if (templatesDlg.exec() == QDialog::Accepted) {
        saveTemplates();
    }
}

void MainWindow::on_actionSaveTemplate_triggered()
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

void MainWindow::on_actionSend_triggered()
{
    if (contactModel == 0 || contactModel->checkedIds().count() <= 0) {
        QMessageBox(this).information(this, "Please Select Recipients",
                                      "Please select one or more recipients.");
//        QErrorMessage(this).showMessage("Please select one or more recipients.");
        return;
    }

    QList<QContact> checkedContacts;
    for (int i = 0; i < contactModel->checkedIds().count(); i++) {
        QContact contact = contactManager->contact(contactModel->checkedIds().at(i));
        checkedContacts.append(contact);
    }

    SendWizard w(this);
    w.setCheckedContacts(checkedContacts);
    w.setTemplateBody(ui->templateEdit->toPlainText());
#ifdef Q_OS_SYMBIAN
    w.setWindowState(Qt::WindowMaximized);
#endif
    w.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About MessageMerge",
                       QString("MessageMerge\n(C) 2010 Soluvas\n") +
                       "http://www.soluvas.com");
}

void MainWindow::on_actionHelp_triggered()
{
    QMessageBox::information(this, "Help", QString("Available template variables:\n") +
                             "[[name]]\n[[firstname]]\n[[lastname]]\n[[email]]\n[[city]]");
}
