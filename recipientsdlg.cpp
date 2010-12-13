#include "recipientsdlg.h"
#include "ui_recipientsdlg.h"
#include <QStringListModel>

RecipientsDlg::RecipientsDlg(QWidget *parent, QAbstractItemModel *model) :
    QDialog(parent),
    ui(new Ui::RecipientsDlg)
{
    ui->setupUi(this);
    ui->contactList->setModel(model);
}

RecipientsDlg::~RecipientsDlg()
{
    delete ui;
}
