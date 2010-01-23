#ifndef TEMPLATESDIALOG_H
#define TEMPLATESDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
    class TemplatesDialog;
}

class TemplatesDialog : public QDialog {
    Q_OBJECT
public:
    TemplatesDialog(QWidget *parent, QMap<QString, QString> *templates);
    ~TemplatesDialog();
    QMap<QString, QString> *templates();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TemplatesDialog *ui;
    QMap<QString, QString> *_templates;

private slots:
    void on_deleteBtn_clicked();
    void on_editBtn_clicked();
    void on_newBtn_clicked();

public slots:
    void refreshTemplateList();
    void refreshTemplatePreview();
};

#endif // TEMPLATESDIALOG_H
