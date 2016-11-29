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
    void Connection(const QJsonObject jsonObjConn);
    void ConnectionGames(const QJsonObject jsonObj);
    void StartPageDisp(int disposition);
    void CenterDisp(int disposition);
    std::string json_to_string(const QJsonObject jsonObj);

private slots:
    void GamesListConnect();
    void PlayerStatsConnect();
    void on_button_Register_clicked();
    void on_button_Login_clicked();
    void on_L_RegisterButton_clicked();
    void on_R_LoginButton_clicked();
    void on_HomeButton_clicked();
    void on_SettingsButton_clicked();
    void on_PlayButton_clicked();
    void on_QuitButton_clicked();
    void on_SSaveButton_clicked();
    void on_SCancelButton_clicked();
    void SettingsUsername();
    void SettingsPassword();
    void Games_List();
    void InstallButton(int);

private:
    Ui::OpenCodeGames *ui;
    QHostAddress addr;
    QString username, token;
    bool LoginCompleted = false, RegisterCompleted = false;
    int LevelValue, ProgressValue, ProgressEnd, Progress;
    float ProgressPercentual;
    int AfterID = 1;
    std::map<int, StructGames> MapGames;
};

#endif // OPENCODEGAMES_H
