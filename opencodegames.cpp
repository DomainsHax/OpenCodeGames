#include "opencodegames.h"
#include "ui_opencodegames.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QGridLayout>

#include <iostream>
#include <string>
#include <regex>

OpenCodeGames::OpenCodeGames(QWidget *parent) : QWidget(parent), ui(new Ui::OpenCodeGames)
{
    ui->setupUi(this);
    CenterDisp(1);

    QHostInfo ip = QHostInfo::fromName("localhost");
    if(!ip.addresses().isEmpty())
    {
        addr = ip.addresses().first();
        PlayerStatsConnect();
        QTimer::singleShot(500,this,SLOT(GamesListConnect()));
    }
}

OpenCodeGames::~OpenCodeGames(){ delete ui; }

void OpenCodeGames::Connection(std::string text_string)
{
    connection_info.connectToHost(addr, 8000);
    connection_info.write(text_string.c_str());
    connect(&connection_info, SIGNAL(readyRead()), this, SLOT(startRead()));
}

void OpenCodeGames::GamesListConnect() { Connection(std::string("GAMES " + std::to_string(AfterID))); }
void OpenCodeGames::PlayerStatsConnect(){ Connection(std::string("INFO_USERNAME " + token.toStdString())); }

void OpenCodeGames::startRead()
{
    connection_info.waitForReadyRead();
    const std::string status_s = QString::fromStdString(QByteArray(connection_info.read(800)).toStdString()).toStdString();
    std::regex regex_games("^GAMES ([0-9]+) ([a-zA-Z0-9]+) ([a-zA-Z0-9.]+) ([0-9]+) ([a-zA-Z0-9.]+) ([0-9]+)$");
    std::regex regex_info_results("^INFO_RESULTS ([a-zA-Z0-9.-_-]+) ([a-zA-Z0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)$");
    std::regex regex_set_username("^SET_USERNAME ([a-zA-Z0-9.-_-]+)$");
    std::smatch match;

    if (regex_match(status_s, regex_games) && regex_search(status_s.begin(), status_s.end(), match, regex_games))
    {
        int ID = atoi(std::string(match[1]).c_str());
        bool After;

        if(std::string(match[6]) == "0") After = false;
        else After = true;

        AfterID = ID + 1;
        MapGames[ID].ID = ID;
        MapGames[ID].Name = QString::fromStdString(std::string(match[2]));
        MapGames[ID].Server = QString::fromStdString(match[3]);
        MapGames[ID].Port = atoi(std::string(match[4]).c_str());
        MapGames[ID].Version = QString::fromStdString(match[5]);

        if(After == true) QTimer::singleShot(200,this,SLOT(GamesListConnect()));
        else Games_List();
    }
    else if(std::regex_match(status_s, regex_info_results) && std::regex_search(status_s.begin(), status_s.end(), match, regex_info_results))
    {
        username = QString::fromStdString(match[1]);
        token = QString::fromStdString(match[2]);
        LevelValue = atoi(std::string(match[3]).c_str());
        ProgressValue = atoi(std::string(match[4]).c_str());
        ProgressEnd = atoi(std::string(match[5]).c_str());
        ProgressPercentual = (ProgressValue * 100) / ProgressEnd;
        Progress = (ProgressPercentual * 162) / 100;

        ui->UserNameValue->setText(username);
        ui->SUsernameValue->setText(username);

        ui->ProgressLevel->setText(("<font color='white'>" + std::to_string(LevelValue) + "</font>").c_str());
        ui->ProgressBar->resize(Progress, 12);
        ui->ProgressValue->setText(("<font color='white'>" + std::to_string(ProgressValue) + "/" + std::to_string(ProgressEnd) + "</font>").c_str());
    }
    else if(std::regex_match(status_s, regex_set_username) && std::regex_search(status_s.begin(), status_s.end(), match, regex_set_username))
    {
        username = QString::fromStdString(std::string(match[1]));
        ui->UserNameValue->setText(username);
        ui->SUsernameValue->setText(username);
        qDebug() << "Username is changed";
    }
    else if(status_s == "USERNAME_ALREADY_EXISTS") qDebug() << "Username already exists";
    else if(status_s == "USERNAME_ALREADY_SETTED") qDebug() << "Username already setted";
    else if(status_s == "NO-CHANGE_USERNAME") qDebug() << "Username isn't changed (chars incorrect)";
    else if(status_s == "SET_PASSWORD") qDebug() << "Password is changed";
    else if(status_s == "PASSWORD_ALREADY_SETTED") qDebug() << "Password is already setted";
    else if(status_s == "SET_PASSWORD_INCORRECT") qDebug() << "Last Password isn't correct";
    else if(status_s == "NO-CHANGE_PASSWORD") qDebug() << "Password isn't changed (chars incorrect)";
    else if(status_s == "TOKEN_DOESNT_EXISTS") qDebug() << "token doesn't exists";
}

void OpenCodeGames::CenterDisp(int disposition)
{
    ui->HomeCenterWidget->setVisible(false);
    ui->SettingsCenterWidget->setVisible(false);
    ui->PlayCenterWidget->setVisible(false);

    if(disposition == 1) ui->HomeCenterWidget->setVisible(true);
    else if(disposition == 2) ui->SettingsCenterWidget->setVisible(true);
    else if(disposition == 3) ui->PlayCenterWidget->setVisible(true);
}

void OpenCodeGames::on_HomeButton_clicked(){ CenterDisp(1); }
void OpenCodeGames::on_SettingsButton_clicked(){ CenterDisp(2); }
void OpenCodeGames::on_PlayButton_clicked(){ CenterDisp(3); }
void OpenCodeGames::on_QuitButton_clicked(){ close(); }

void OpenCodeGames::Games_List()
{
    QVBoxLayout *GamesScrollLayout = new QVBoxLayout;
    int max = MapGames.size();

    for(int i = 1; i < max + 1; i++)
    {
        QWidget *GameStatus = new QWidget;
        QWidget *pic = new QWidget;
        QLabel *Name = new QLabel(MapGames[i].Name);
        QGridLayout *GameStatusLayout = new QGridLayout;
        QPushButton *Status = new QPushButton;

        QString Version = MapGames[i].Version;
        QString Vers = NULL;
        QString dir = "/root/OpenCodeGames/Games/" + MapGames[i].Name;
        QDir* DIRECTORY = new QDir(dir);
        QFile* VERSION = new QFile(dir + "/VERSION.txt");
        QFile* EXEC = new QFile(dir + "/" + MapGames[i].Name);

        if(DIRECTORY->exists())
        {
            if(VERSION->exists())
            {
                VERSION->open(QIODevice::ReadOnly);
                Vers = VERSION->readAll();
            }
        }

        GameStatus->setStyleSheet("background-color:grey;");
        if(max <= 4) GameStatus->setFixedSize(306, 83);
        else GameStatus->setFixedSize(292, 84);

        pic->setFixedSize(65, 65);
        pic->setStyleSheet("background-color:blue;");

        Name->setFixedSize(150, 15);
        Name->setAlignment(Qt::AlignCenter);

        Status->setFixedSize(45, 30);

        GameStatusLayout->addWidget(pic, 0, 0, 0, 0, Qt::AlignLeft);
        GameStatusLayout->addWidget(Name, 0, 2, 0, 4, Qt::AlignTop);

        int flag = 0;

        if(Vers == Version && DIRECTORY->exists() && VERSION->exists() && EXEC->exists())
            Status->setText("Play");
        else if(!DIRECTORY->exists() && !EXEC->exists() && !VERSION->exists())
        {
            Status->setText("Install");
            flag = 1;
        }
        else if((Vers != Version or !EXEC->exists() or !VERSION->exists()) && flag == 0)
            Status->setText("Update");

        GameStatusLayout->addWidget(Status, 1, 5, 1, 1, Qt::AlignBottom);
        GamesScrollLayout->addWidget(GameStatus);
        GameStatus->setLayout(GameStatusLayout);
    }

    ui->PlayScrollAreaWidget->setLayout(GamesScrollLayout);
}

void OpenCodeGames::on_SSaveButton_clicked()
{
    int settings = 0;
    if(username != ui->SUsernameValue->text() && !ui->SUsernameValue->text().isEmpty())
        settings += 1;
    if(ui->SNewPasswordValue->text() == ui->SRetypeNewPasswordValue->text() && !ui->SPasswordValue->text().isEmpty() && !ui->SNewPasswordValue->text().isEmpty() && !ui->SRetypeNewPasswordValue->text().isEmpty())
        settings += 2;

    if(settings == 1) SettingsUsername();
    else if(settings == 2) SettingsPassword();
    else if(settings == 3)
    {
        SettingsUsername();
        QTimer::singleShot(500,this,SLOT(SettingsPassword()));
    }
}

void OpenCodeGames::SettingsUsername()
{
    if(ui->SUsernameValue->text().length() >= 3 && ui->SUsernameValue->text().length() <= 16)
        Connection(std::string("SETTINGS_USERNAME " + token.toStdString() + " " + ui->SUsernameValue->text().toStdString()));
    else if(ui->SUsernameValue->text().length() <= 3) qDebug() << "Username < 3 chars";
}

void OpenCodeGames::SettingsPassword()
{
    if((ui->SPasswordValue->text().length() >= 6 && ui->SPasswordValue->text().length() <= 32) && (ui->SNewPasswordValue->text().length() >= 6 && ui->SNewPasswordValue->text().length() <= 32) && (ui->SRetypeNewPasswordValue->text().length() >= 6 && ui->SRetypeNewPasswordValue->text().length() <= 32))
        Connection(std::string("SETTINGS_PASSWORD " + token.toStdString() + " " + ui->SPasswordValue->text().toStdString() + " " + ui->SNewPasswordValue->text().toStdString()));
    else if(ui->SNewPasswordValue->text().length() <= 6) qDebug() << "Password < 6 chars";
}

void OpenCodeGames::on_SCancelButton_clicked()
{
    ui->SUsernameValue->setText(username);
    ui->SPasswordValue->setText("");
    ui->SNewPasswordValue->setText("");
    ui->SRetypeNewPasswordValue->setText("");

    CenterDisp(1);
}
