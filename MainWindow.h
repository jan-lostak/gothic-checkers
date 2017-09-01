#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QListView>
#include <QCloseEvent>

#include <BoardGraphicScene.h>

#define SAVE_FILE_HEADER "Gticka dama save file"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        void SpawnPieces(std::vector<ChessPiece*> /*pieces*/);
        void UpdateTurnsHistoryView();
        void UpdateGameSettingStatuBar();
        void UpdateWhosTurn();
        void UpdateContextMenuControls();
        void SetGameFeedbackText(const QString& str);
        void SaveToFile(QString& path);

    private slots:
        void on_actionNovaHra_triggered();
        void on_actionOtevritHru_triggered();
        void on_actionUlozit_triggered();
        void on_actionUlozitHruJako_triggered();
        void on_actionVratitTah_triggered();
        void on_actionNastaven_triggered();
        void on_actionZobrazitNapovedu_triggered();
        void on_actionOProgramu_triggered();
        void on_actionUkoncit_triggered();
        void on_actionNapovedaNejlepsihoTahu_triggered();
        void on_actionZnovu_triggered();

    private:
        void closeEvent(QCloseEvent *event);
        void OpenLoadGameFromFile();
        void OpenSaveGameToFile();

        Ui::MainWindow* _ui;
        BoardGraphicScene* _scene;

        QLabel* _gameSettingsInfo;
        QLabel* _gameDiffucultyInfo;
        QLabel* _gameCurrentPlayer;
        QLabel* _gameFeedback;

        QString _saveFile;
};

#endif // MAINWINDOW_H
