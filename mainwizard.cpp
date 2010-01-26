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
#include <QSettings>

MainWizard::MainWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MainWizard)
{
    ui->setupUi(this);

    connect(this, SIGNAL(currentIdChanged(int)), SLOT(handle_currentIdChanged(int)));

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
    reloadContacts();
}

void MainWizard::reloadContacts() {
    contacts.clear();
    if (m_contactManager == NULL)
        return;

    if (m_contactManager->managerName() == "memory") {
        qDebug() << "Using manager 'memory', adding sample contacts data.";
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

    QListIterator<QContactLocalId> i(m_contactManager->contacts());
    while (i.hasNext()) {
        QContactLocalId contactId = i.next();
        QContact contact = m_contactManager->contact(contactId);
        contacts.append(contact);
    }

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
    if (templatesDlg.exec() == QDialog::Accepted) {
        saveTemplates();
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
        qDebug() << i + 1 << ":" << contact.localId() << ":" << item->text();
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
