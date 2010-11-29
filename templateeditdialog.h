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
