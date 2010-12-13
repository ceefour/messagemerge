#ifndef SENDWIZARD_H
#define SENDWIZARD_H

#include <QWizard>

namespace Ui {
    class SendWizard;
}

class SendWizard : public QWizard
{
    Q_OBJECT

public:
    explicit SendWizard(QWidget *parent = 0);
    ~SendWizard();

private:
    Ui::SendWizard *ui;
};

#endif // SENDWIZARD_H
