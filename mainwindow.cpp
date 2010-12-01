#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    reloadTemplates();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reloadTemplates() {
    QSettings settings("Soluvas", "MessageMerge", this);
    templates.clear();
    int templateCount = settings.beginReadArray("templates");
    for (int i = 0; i < templateCount; i++) {
        settings.setArrayIndex(i);
        templates.insert(settings.value("name").toString(), settings.value("body").toString());
    }
    settings.endArray();
    qDebug() << "Loaded" << templateCount << "templates from settings.";
}

void MainWindow::saveTemplates() {
    QSettings settings("Soluvas", "MessageMerge", this);
    settings.beginWriteArray("templates");
    QMapIterator<QString, QString> i(templates);
    int idx = 0;
    while (i.hasNext()) {
        i.next();
        settings.setArrayIndex(idx);
        settings.setValue("name", i.key());
        settings.setValue("body", i.value());
        idx++;
    }
    settings.endArray();
    qDebug() << "Saved" << idx << "templates to settings.";
}

void MainWindow::on_actionLoadTemplate_triggered()
{
    QInputDialog loadTemplateDlg(this, Qt::Dialog);
    loadTemplateDlg.setWindowTitle("Load Template");
    loadTemplateDlg.setInputMode(QInputDialog::TextInput);
    loadTemplateDlg.setLabelText("Template:");
    loadTemplateDlg.setComboBoxItems(templates.keys());
    if (loadTemplateDlg.exec() == QDialog::Accepted) {
        QString selectedName = loadTemplateDlg.textValue();
        if (templates.contains(selectedName)) {
            ui->templateEdit->setPlainText(templates[selectedName]);
        } else {
            QErrorMessage(this).showMessage("Invalid template name: \"" + selectedName + "\".");
        }
    }
}
