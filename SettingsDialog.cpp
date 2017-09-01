#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include "GameManager.h"

SettingsDialog::SettingsDialog(QDialog *parent) : QDialog(parent), _ui(new Ui::SettingsDialog)
{
    _ui->setupUi(this);

    Player* black = sGameManager->GetBlackPlayer();
    Player* white = sGameManager->GetWhitePlayer();

    _ui->comboBoxBlackDifficulty->setCurrentIndex(black->GetGameDifficulty());
    _ui->comboBoxWhiteDifficulty->setCurrentIndex(white->GetGameDifficulty());

    _ui->comboBoxBlackPlayerType->setCurrentIndex(black->GetPlayerType());
    _ui->comboBoxWhitePlayerType->setCurrentIndex(white->GetPlayerType());
}

SettingsDialog::~SettingsDialog()
{
    delete _ui;
}

void SettingsDialog::on_pushButtonApply_clicked()
{
    Player* black = sGameManager->GetBlackPlayer();
    Player* white = sGameManager->GetWhitePlayer();

    black->SetGameDifficulty(GameDifficulty(_ui->comboBoxBlackDifficulty->currentIndex()));
    white->SetGameDifficulty(GameDifficulty(_ui->comboBoxWhiteDifficulty->currentIndex()));

    black->SetPlayerType(PlayerType(_ui->comboBoxBlackPlayerType->currentIndex()));
    white->SetPlayerType(PlayerType(_ui->comboBoxWhitePlayerType->currentIndex()));

    accept();
}

void SettingsDialog::on_pushButtonCancel_clicked()
{
    reject();
}
