#include "messagemerger.h"
#include "qtcontacts.h"

MessageMerger::MessageMerger()
{
}

QString MessageMerger::merge(QString const &templateBody, QContact contact) const {
    QContactName contactName = contact.detail<QContactName>();
    QString message = templateBody;
    message = message.replace("[[name]]",
                              contactName.first() + " " + contactName.last(), Qt::CaseSensitive);
    message = message.replace("[[firstname]]",
                              contactName.first(), Qt::CaseSensitive);
    message = message.replace("[[lastname]]",
                              contactName.last(), Qt::CaseSensitive);
    QContactEmailAddress email = contact.detail<QContactEmailAddress>();
    if (!email.isEmpty())
        message = message.replace("[[email]]",
                                  email.emailAddress(), Qt::CaseSensitive);
    QContactAddress address = contact.detail<QContactAddress>();
    if (!address.isEmpty() && !address.locality().isEmpty())
        message = message.replace("[[city]]",
                                  address.locality(), Qt::CaseSensitive);
    return message;
}
