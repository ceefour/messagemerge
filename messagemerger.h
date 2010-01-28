#ifndef MESSAGEMERGER_H
#define MESSAGEMERGER_H

#include <QString>
#include <qcontact.h>

QTM_USE_NAMESPACE

class MessageMerger
{
public:
    MessageMerger();
    QString merge(QString const &templateBody, QContact contact) const;
    static QString fullName(QContactName contactName);
};

#endif // MESSAGEMERGER_H
