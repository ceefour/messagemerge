#include "sendwizard.h"
#include "ui_sendwizard.h"

SendWizard::SendWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::SendWizard)
{
    ui->setupUi(this);
}

SendWizard::~SendWizard()
{
    delete ui;
}
