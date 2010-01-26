#ifndef TEMPLATEEDITDIALOG_H
#define TEMPLATEEDITDIALOG_H

#include <QDialog>

namespace Ui {
    class TemplateEditDialog;
}

class TemplateEditDialog : public QDialog {
    Q_OBJECT
public:
    TemplateEditDialog(QWidget *parent = 0);
    ~TemplateEditDialog();
    QString templateName() const;
    QString templateBody() const;
    void setTemplateName(const QString &templateName);
    void setTemplateBody(const QString &templateBody);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TemplateEditDialog *ui;
};

#endif // TEMPLATEEDITDIALOG_H
