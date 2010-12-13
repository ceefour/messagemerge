#include "previewdlg.h"
#include "ui_previewdlg.h"

PreviewDlg::PreviewDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewDlg)
{
    ui->setupUi(this);
}

PreviewDlg::~PreviewDlg()
{
    delete ui;
}
