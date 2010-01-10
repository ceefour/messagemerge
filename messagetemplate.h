#ifndef MESSAGETEMPLATE_H
#define MESSAGETEMPLATE_H

#include <QString>

class MessageTemplate
{
    QString _name;
    QString _text;
public:
    MessageTemplate();
    MessageTemplate(const QString &name, const QString &text);
    QString name() const;
    QString text() const;
    void setName(const QString &name);
    void setText(const QString &text);
};

#endif // MESSAGETEMPLATE_H
