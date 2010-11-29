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

#include "templatesdialog.h"
#include "ui_templatesdialog.h"
#include <QInputDialog>
#include "templateeditdialog.h"

TemplatesDialog::TemplatesDialog(QWidget *parent, QMap<QString, QString> *templates) :
    QDialog(parent),
    ui(new Ui::TemplatesDialog),
    _templates(templates)
{
    ui->setupUi(this);
    refreshTemplateList();
    refreshTemplatePreview();
}

TemplatesDialog::~TemplatesDialog()
{
    delete ui;
}

void TemplatesDialog::changeEvent(QEvent *e)
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

QMap<QString, QString> *TemplatesDialog::templates() {
    return _templates;
}

void TemplatesDialog::on_newBtn_clicked()
{
    TemplateEditDialog templateDlg(this);
    templateDlg.setWindowTitle("New Template");
#ifdef Q_OS_SYMBIAN
    templateDlg.setWindowState(Qt::WindowMaximized);
#endif
    if (templateDlg.exec() == QDialog::Accepted) {
        templates()->insert(templateDlg.templateName(), templateDlg.templateBody());
        refreshTemplateList();
    }
}

void TemplatesDialog::refreshTemplateList() {
    ui->templateList->clear();
    QMapIterator<QString, QString> i(*templates());
    while (i.hasNext()) {
        i.next();
        ui->templateList->addItem(i.key());
    }
}

void TemplatesDialog::on_editBtn_clicked()
{
    QList<QListWidgetItem *> sel = ui->templateList->selectedItems();
    if (!sel.isEmpty()) {
        QString templateName = sel[0]->text();
        TemplateEditDialog templateDlg(this);
        templateDlg.setWindowTitle("Edit Template \"" + templateName + "\"");
        templateDlg.setTemplateName(templateName);
        templateDlg.setTemplateBody(templates()->value(templateName));
#ifdef Q_OS_SYMBIAN
        templateDlg.setWindowState(Qt::WindowMaximized);
#endif
        if (templateDlg.exec() == QDialog::Accepted) {
            if (templateDlg.templateName().isEmpty())
                throw "Template name must not be empty.";
            templates()->remove(templateName);
            templates()->insert(templateDlg.templateName(), templateDlg.templateBody());
            refreshTemplateList();
        }
    }
}

void TemplatesDialog::on_deleteBtn_clicked()
{
    QList<QListWidgetItem *> sel = ui->templateList->selectedItems();
    if (!sel.isEmpty()) {
        QString templateName = sel[0]->text();
        templates()->remove(templateName);
        refreshTemplateList();
    }
}

void TemplatesDialog::refreshTemplatePreview() {
    QList<QListWidgetItem *> sel = ui->templateList->selectedItems();
    if (!sel.isEmpty()) {
        QString templateName = sel[0]->text();
        ui->templateEdit->setPlainText(templates()->value(templateName));
    } else {
        ui->templateEdit->clear();
    }
}
