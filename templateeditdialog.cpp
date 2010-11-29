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
