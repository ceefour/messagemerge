#include "messagemerger.h"
#include "qtcontacts.h"

MessageMerger::MessageMerger()
{
}

QString MessageMerger::merge(QString const &templateBody, QContact contact) const {
    QContactName contactName = contact.detail<QContactName>();
    QString message = templateBody;
    message = message.replace("[[name]]",
                              fullName(contactName), Qt::CaseSensitive);
    QString firstName = contactName.first().isEmpty() ? "" : contactName.first();
    message = message.replace("[[firstname]]",
                              firstName, Qt::CaseSensitive);
    QString lastName = contactName.last().isEmpty() ? "" : contactName.last();
    message = message.replace("[[lastname]]",
                              lastName, Qt::CaseSensitive);
    QContactEmailAddress email = contact.detail<QContactEmailAddress>();
    message = message.replace("[[email]]",
                              email.isEmpty() ? "" : email.emailAddress(), Qt::CaseSensitive);
    QContactAddress address = contact.detail<QContactAddress>();
    message = message.replace("[[city]]",
                              !address.isEmpty() && !address.locality().isEmpty() ? address.locality() : "",
                              Qt::CaseSensitive);
    return message;
}

QString MessageMerger::fullName(QContactName contactName) {
    if (!contactName.first().isEmpty() && !contactName.last().isEmpty())
        return contactName.first() + " " + contactName.last();
    if (!contactName.first().isEmpty())
        return contactName.first();
    if (!contactName.last().isEmpty())
        return contactName.last();
    return "";
}
