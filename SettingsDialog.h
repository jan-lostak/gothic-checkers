#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui
{
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit SettingsDialog(QDialog *parent = nullptr);
        ~SettingsDialog();

    private:
        Ui::SettingsDialog* _ui;

    private slots:
        void on_pushButtonApply_clicked();
        void on_pushButtonCancel_clicked();
};

#endif // SETTINGSDIALOG_H
