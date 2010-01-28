#ifndef MAINWIZARD_H
#define MAINWIZARD_H

#include <qwizard.h>
#include <qlist.h>
#include <qmap.h>
#include <qcontact.h>
#include <qcontactmanager.h>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QContact)

namespace Ui {
    class MainWizard;
}

class MainWizard : public QWizard {
    Q_OBJECT
public:
    MainWizard(QWidget *parent = 0);
    ~MainWizard();
    QString templateBody() const;
    void setContactManager(QContactManager *contactManager);
    void reloadTemplates();
    void saveTemplates();
    void setOutputDir(const QString& outputDir);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWizard *ui;
    QMap<QString, QString> templates;
    QList<QContact> contacts;
    QList<QContact> selectedContacts();
    QContactManager *m_contactManager;

private slots:
    void handle_currentIdChanged(int id);
    void on_loadTemplateBtn_clicked();
    void on_saveAsTemplateBtn_clicked();
    void on_editTemplatesBtn_clicked();
    void refreshMessagePreview();
    void refreshContactList();
    void refreshContactCombo();
    void processSaveFiles();
    void processSendTextMessages();
    void processDraftTextMessages();
    void reloadContacts();
};

#endif // MAINWIZARD_H
