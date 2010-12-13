#include "contactlistmodel.h"
#include <QVariant>
#include <messagemerger.h>
#include <QDebug>
#include <messagemergeutils.h>

ContactListModel::ContactListModel(QObject *parent) :
    QAbstractListModel(parent), contactIds(), checkedIds()
{
    MessageMergeUtils util;
    //this->mgr = mgr;
    // I think using Filter makes it so slow under Symbian
    //this->sortedIds = mgr->contactIds(util.contactFilter(), util.contactSortOrder());
    //this->sortedIds = mgr->contactIds(util.contactSortOrder());
}

int ContactListModel::rowCount(const QModelIndex &parent) const {
    return contactIds.length();
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (index.row() >= contactIds.length()) return QVariant();
    QContactLocalId contactId = contactIds.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        {
            return names.at(index.row());
        }
    case Qt::CheckStateRole:
        return checkedIds.contains(contactId) ? Qt::Checked : Qt::Unchecked;
    default:
        return QVariant();
    }
}

QVariant ContactListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

Qt::ItemFlags ContactListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    if (index.row() >= contactIds.length()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

bool ContactListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    qDebug() << "Setting" << index.row() << role << "to" << value;
    if (!index.isValid()) return false;
    if (index.row() >= contactIds.length()) return false;
    if (role == Qt::CheckStateRole) {
        QContactLocalId contactId = contactIds.at(index.row());
        checkedIds.removeAll(contactId);
        if (value == Qt::Checked)
            checkedIds.append(contactId);
        dataChanged(index, index);
        return true;
    }
    return false;
}

void ContactListModel::addItem(const QString &text, QContactLocalId data) {
    names.append(text);
    contactIds.append(data);
}

