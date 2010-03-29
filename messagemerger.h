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
