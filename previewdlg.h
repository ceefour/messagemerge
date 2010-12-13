#ifndef PREVIEWDLG_H
#define PREVIEWDLG_H

#include <QDialog>

namespace Ui {
    class PreviewDlg;
}

class PreviewDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewDlg(QWidget *parent = 0);
    ~PreviewDlg();

private:
    Ui::PreviewDlg *ui;
};

#endif // PREVIEWDLG_H
