#ifndef OPENCODEGAMES_H
#define OPENCODEGAMES_H

#include <QWidget>
#include <QtNetwork>
#include <QTcpSocket>

namespace Ui {
class OpenCodeGames;
}

struct StructGames
{
    int ID;
    QString Name;
    QString Server;
    int Port;
    QString Version;
};

class OpenCodeGames : public QWidget
{
    Q_OBJECT

public:
    explicit OpenCodeGames(QWidget *parent = 0);
    ~OpenCodeGames();
    void Connection(std::string text_string);
    void CenterDisp(int disposition);

private slots:
    void startRead();
    void GamesListConnect();
    void PlayerStatsConnect();
    void on_HomeButton_clicked();
    void on_SettingsButton_clicked();
    void on_PlayButton_clicked();
    void on_QuitButton_clicked();
    void on_SSaveButton_clicked();
    void on_SCancelButton_clicked();
    void SettingsUsername();
    void SettingsPassword();
    void Games_List();

private:
    Ui::OpenCodeGames *ui;
    QHostAddress addr;
    QTcpSocket connection_info;
    QString username, token = "63253fa398dfa1e0c67d04e512f9d79f953bf074cd592dbcf9692a5e71e9ac0c";
    int LevelValue, ProgressValue, ProgressEnd, ProgressPercentual, Progress;
    int AfterID = 1;
    std::map<int, StructGames> MapGames;
};

#endif // OPENCODEGAMES_H
