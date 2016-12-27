#ifndef OPENCODEGAMES_H
#define OPENCODEGAMES_H

#include "filedownloader.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include <QtNetwork>
#include <QTcpSocket>

namespace Ui {
class OpenCodeGames;
}

struct Games
{
    QString token_install;
    int ID;
    QString Name;
    QString Server;
    int Port;
    QString Version;
    QJsonArray files;
    int id_file = 0;
    bool end_install = false;

    QWidget* GameStatus = new QWidget();
    QWidget* pic = new QWidget();
    QLabel* NameLabel = new QLabel();
    QGridLayout* GameStatusLayout = new QGridLayout();
    int status;
    QPushButton* Status = new QPushButton();
    QString dir;
};

class OpenCodeGames : public QWidget
{
    Q_OBJECT

public:
    explicit OpenCodeGames(QWidget *parent = 0);
    ~OpenCodeGames();
    void Connection(const QJsonObject jsonObjConn);
    void ConnectionGames(const QJsonObject jsonObjConnGames);
    void StartPageDisp(int disposition);
    void CenterDisp(int disposition);
    std::string json_to_string(const QJsonObject jsonObj);
    void StatusGame(int id_game, int status);
    void ResetSettings();
    void NewsInfo();
    void News();

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
    void makefile();

private:
    Ui::OpenCodeGames *ui;
    QHostAddress addr;
    QString os;
    int architecture;
    QString username, token;
    bool LoginCompleted = false, RegisterCompleted = false;
    int LevelValue, ProgressValue, ProgressEnd, Progress;
    float ProgressPercentual;

    std::map<int, Games> MapGames;
    FileDownloader* Download;
    int id_game = NULL;
    QVBoxLayout *GamesLayout = new QVBoxLayout;

    QStringList list_file = {"lol.txt", "ciao.txt"};
    bool download_process = false;
    int li = 0;

    QJsonArray ArrayNews;
    QGridLayout *NewsLayout = new QGridLayout;
};

#endif // OPENCODEGAMES_H
