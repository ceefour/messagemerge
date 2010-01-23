#include "templateeditdialog.h"
#include "ui_templateeditdialog.h"

TemplateEditDialog::TemplateEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemplateEditDialog)
{
    ui->setupUi(this);
}

TemplateEditDialog::~TemplateEditDialog()
{
    delete ui;
}

void TemplateEditDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString TemplateEditDialog::templateName() const {
    return ui->nameEdit->text();
}

QString TemplateEditDialog::templateBody() const {
    return ui->bodyEdit->toPlainText();
}

void TemplateEditDialog::setTemplateName(const QString &templateName) {
    ui->nameEdit->setText(templateName);
}

void TemplateEditDialog::setTemplateBody(const QString &templateBody) {
    ui->bodyEdit->setPlainText(templateBody);
}
