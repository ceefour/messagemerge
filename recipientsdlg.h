#ifndef RECIPIENTSDLG_H
#define RECIPIENTSDLG_H

#include <QDialog>
#include <QStringList>
#include <QAbstractListModel>

namespace Ui {
    class RecipientsDlg;
}

class RecipientsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RecipientsDlg(QWidget *parent, QAbstractItemModel *model);
    ~RecipientsDlg();

private:
    Ui::RecipientsDlg *ui;
    QAbstractListModel *model;
};

#endif // RECIPIENTSDLG_H
