#ifndef MESSAGEMERGER_H
#define MESSAGEMERGER_H

#include <QString>
#include <QContact>

QTM_USE_NAMESPACE

class MessageMerger
{
public:
    MessageMerger();
    QString merge(QString const &templateBody, QContact contact) const;
};

#endif // MESSAGEMERGER_H
