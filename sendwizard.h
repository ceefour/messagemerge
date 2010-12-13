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

#ifndef MAINWIZARD_H
#define MAINWIZARD_H

#include <qwizard.h>
#include <qlist.h>
#include <qmap.h>
#include <qtcontacts.h>
#include "messagemergeutils.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QContact)

namespace Ui {
    class SendWizard;
}

class SendWizard : public QWizard {
    Q_OBJECT
public:
    SendWizard(QWidget *parent = 0);
    ~SendWizard();
    void setCheckedContacts(const QList<QContact> &checkedContacts);
//    void setContactManager(QContactManager *contactManager);
    void reloadTemplates();
    void saveTemplates();
    void setOutputDir(const QString& outputDir);
    void setTemplateBody(const QString& templateBody);
    void preDisplay();

protected:
    void changeEvent(QEvent *e);

private:
    QString templateBody;
    QList<QContact> checkedContacts;
//    QContactManager *contactManager;
//    MessageMergeUtils util;

    Ui::SendWizard *ui;

private slots:
    void on_previewContactCombo_currentIndexChanged(int index);
    void handle_currentIdChanged(int id);
    void refreshMessagePreview();
    void refreshContactCombo();
    void processSaveFiles();
    void processSendTextMessages();
    void processDraftTextMessages();
};

#endif // MAINWIZARD_H
