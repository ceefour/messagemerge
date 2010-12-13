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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), contactManager(),
    ui(new Ui::MainWindow)
{
    //contactManager = 0;
    //contacts = 0;
    ui->setupUi(this);

    reloadTemplates();
}

MainWindow::~MainWindow()
{
    delete ui;
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
        qDebug() << "Reading id=" << contactId;
        try {
            QContact contact;
            // TODO: How to catch this error? It still just terminates on Nokia E71! :(
            contact = contactManager->contact(contactId);
            //QContactNickname nick = contact.detail<QContactNickname>();
            qDebug() << "Display=" << contact.displayLabel() << " merged=" << MessageMerger::fullName(contact.detail<QContactName>());// << " nick=" << nick.nickname();
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
