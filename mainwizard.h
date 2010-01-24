#ifndef MAINWIZARD_H
#define MAINWIZARD_H

#include <QWizard>
#include <QList>
#include <QMap>
#include <QContact>

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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWizard *ui;
    QMap<QString, QString> templates;
    QList<QContact> contacts;
    QList<QContact> selectedContacts();

private slots:
    void handle_currentIdChanged(int id);
    void on_loadTemplateBtn_clicked();
    void on_saveAsTemplateBtn_clicked();
    void on_editTemplatesBtn_clicked();
    void refreshMessagePreview();
    void refreshContactList();
    void refreshContactCombo();
    void processSaveFiles();
};

#endif // MAINWIZARD_H
