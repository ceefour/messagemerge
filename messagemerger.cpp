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
    QString firstName = contactName.firstName().isEmpty() ? "" : contactName.firstName();
    message = message.replace("[[firstname]]",
                              firstName, Qt::CaseSensitive);
    QString lastName = contactName.lastName().isEmpty() ? "" : contactName.lastName();
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
    if (!contactName.firstName().isEmpty() && !contactName.lastName().isEmpty())
        return contactName.firstName() + " " + contactName.lastName();
    if (!contactName.firstName().isEmpty())
        return contactName.firstName();
    if (!contactName.lastName().isEmpty())
        return contactName.lastName();
    return "";
}
