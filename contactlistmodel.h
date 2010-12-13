#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include "qtcontacts.h"
#include <QList>

QTM_USE_NAMESPACE

class ContactListModel : public QAbstractListModel
{
    Q_OBJECT
    //QContactManager *mgr;
    QStringList names;
    QList<QContactLocalId> contactIds;
    QList<QContactLocalId> checkedIds;
public:
    explicit ContactListModel(QObject *parent);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void addItem(const QString &text, QContactLocalId data);

signals:

public slots:

};

#endif // CONTACTLISTMODEL_H
