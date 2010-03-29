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
