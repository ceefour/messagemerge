/********* DEPRECATED ***************/
#include "messagetemplate.h"

MessageTemplate::MessageTemplate()
{
}

MessageTemplate::MessageTemplate(const QString &name, const QString &text) {
    setName(name);
    setText(text);
}

QString MessageTemplate::name() const { return _name; }

QString MessageTemplate::text() const { return _text; }

void MessageTemplate::setName(const QString &name) {
    _name = name;
}

void MessageTemplate::setText(const QString &text) {
    _text = text;
}
