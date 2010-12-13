#ifndef MESSAGEMERGEUTILS_H
#define MESSAGEMERGEUTILS_H

#include <QObject>
#include <qtcontacts.h>

QTM_USE_NAMESPACE

class MessageMergeUtils : public QObject
{
    Q_OBJECT
public:
    explicit MessageMergeUtils(QObject *parent = 0);
    void loadSampleIfNeeded(QContactManager *m_contactManager);
    QContactDetailFilter contactFilter();
    QList<QContactSortOrder> contactSortOrder();

signals:

public slots:

};

#endif // MESSAGEMERGEUTILS_H
