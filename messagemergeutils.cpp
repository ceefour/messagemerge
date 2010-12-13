#include "messagemergeutils.h"
#include <QDebug>

MessageMergeUtils::MessageMergeUtils(QObject *parent) :
    QObject(parent)
{
}

void MessageMergeUtils::loadSampleIfNeeded(QContactManager *m_contactManager) {
    int contactCount = m_contactManager->contactIds().length();
    if (m_contactManager->managerName() == "memory" && contactCount == 0) {
        qDebug() << "Using manager 'memory' and empty, adding sample contacts data.";
        // sample contacts
        QtMobility::QContact contact;
        QtMobility::QContactName name;
        name.setFirstName("John");
        name.setLastName("Smith");
        contact.saveDetail(&name);
        QContactPhoneNumber phone;
        phone.setNumber("+628123456789");
        phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        contact.saveDetail(&phone);
        QContactEmailAddress email;
        email.setEmailAddress("john.smith@example.com");
        contact.saveDetail(&email);
        QContactAddress address;
        address.setLocality("Medan");
        contact.saveDetail(&address);
        m_contactManager->saveContact(&contact);
        contact = QtMobility::QContact();
        name = QtMobility::QContactName();
        name.setFirstName("Mary");
        name.setLastName("Swanson");
        contact.saveDetail(&name);
        phone = QContactPhoneNumber();
        phone.setNumber("+6285678912345");
        phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        contact.saveDetail(&phone);
        email = QContactEmailAddress();
        email.setEmailAddress("mary.swanson@example.com");
        contact.saveDetail(&email);
        address = QContactAddress();
        address.setLocality("Bandung");
        contact.saveDetail(&address);
        m_contactManager->saveContact(&contact);
    }
}

QContactDetailFilter MessageMergeUtils::contactFilter() {
    // Filter only contact types (i.e. exclude groups)
    QContactDetailFilter typeFilter;
    typeFilter.setDetailDefinitionName(QContactType::DefinitionName);
    typeFilter.setValue(QLatin1String(QContactType::TypeContact));
    typeFilter.setMatchFlags(QContactFilter::MatchExactly);
    return typeFilter;
}

QList<QContactSortOrder> MessageMergeUtils::contactSortOrder() {
    // Sort by First, then Last name
    QContactSortOrder sortFirst;
    sortFirst.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    QContactSortOrder sortLast;
    sortLast.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
    QList<QContactSortOrder> sortOrder;
    sortOrder.append(sortFirst);
    sortOrder.append(sortLast);
    return sortOrder;
}
