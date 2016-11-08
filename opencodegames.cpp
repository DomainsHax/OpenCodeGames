#include "opencodegames.h"
#include "ui_opencodegames.h"

#include <QPushButton>
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
    }
}

OpenCodeGames::~OpenCodeGames(){ delete ui; }

void OpenCodeGames::Connection(std::string text_string)
{
    connection_info.connectToHost(addr, 8000);
    connection_info.write(text_string.c_str());
    connect(&connection_info, SIGNAL(readyRead()), this, SLOT(startRead()));
}

void OpenCodeGames::PlayerStatsConnect(){ Connection(std::string("INFO_USERNAME " + token.toStdString())); }

void OpenCodeGames::startRead()
{
    connection_info.waitForReadyRead();
    const std::string status_s = QString::fromStdString(QByteArray(connection_info.read(800)).toStdString()).toStdString();
    std::regex regex_info_results("^INFO_RESULTS ([a-zA-Z0-9.-_-]+) ([a-zA-Z0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)$");
    std::regex regex_set_username("^SET_USERNAME ([a-zA-Z0-9.-_-]+)$");
    std::smatch match;

    if(std::regex_match(status_s, regex_info_results) && std::regex_search(status_s.begin(), status_s.end(), match, regex_info_results))
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
        qDebug() << "Username is changed\n";
    }
    else if(status_s == "USERNAME_ALREADY_EXISTS") qDebug() << "Username already exists\n";
    else if(status_s == "USERNAME_ALREADY_SETTED") qDebug() << "Username already setted\n";
    else if(status_s == "NO-CHANGE_USERNAME") qDebug() << "Username isn't changed (chars incorrect)\n";

    else if(status_s == "SET_PASSWORD") qDebug() << "Password is changed\n";
    else if(status_s == "PASSWORD_ALREADY_SETTED") qDebug() << "Password is already setted\n";
    else if(status_s == "SET_PASSWORD_INCORRECT") qDebug() << "Last Password isn't correct\n";
    else if(status_s == "NO-CHANGE_PASSWORD") qDebug() << "Password isn't changed (chars incorrect)\n";

    else if(status_s == "TOKEN_DOESNT_EXISTS") qDebug() << "token doesn't exists\n";
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

void OpenCodeGames::on_SSaveButton_clicked()
{
    if((username != ui->SUsernameValue->text())   &&   (!ui->SPasswordValue->text().isEmpty() && !ui->SNewPasswordValue->text().isEmpty() && !ui->SRetypeNewPasswordValue->text().isEmpty()))
    {
        SettingsUsername();
        QTimer::singleShot(500,this,SLOT(SettingsPassword()));
    }
    else if(username != ui->SUsernameValue->text())
        SettingsUsername();
    else if(!ui->SPasswordValue->text().isEmpty() && !ui->SNewPasswordValue->text().isEmpty() && !ui->SRetypeNewPasswordValue->text().isEmpty())
        SettingsPassword();
}

void OpenCodeGames::SettingsUsername()
{
    if(!ui->SUsernameValue->text().isEmpty())
    {
        if(ui->SUsernameValue->text().length() >= 3 && ui->SUsernameValue->text().length() <= 16)
            Connection(std::string("SETTINGS_USERNAME " + token.toStdString() + " " + ui->SUsernameValue->text().toStdString()));
        else if(ui->SUsernameValue->text().length() <= 3) qDebug() << "Username < 3 chars";
        else if(ui->SUsernameValue->text().length() >= 16) qDebug() << "Username > 16 chars";
    }
}

void OpenCodeGames::SettingsPassword()
{
    if(ui->SNewPasswordValue->text() == ui->SRetypeNewPasswordValue->text())
    {
        if((ui->SPasswordValue->text().length() >= 6 && ui->SPasswordValue->text().length() <= 32) && (ui->SNewPasswordValue->text().length() >= 5 && ui->SNewPasswordValue->text().length() <= 32) && (ui->SRetypeNewPasswordValue->text().length() >= 5 && ui->SRetypeNewPasswordValue->text().length() <= 32))
            Connection(std::string("SETTINGS_PASSWORD " + token.toStdString() + " " + ui->SPasswordValue->text().toStdString() + " " + ui->SNewPasswordValue->text().toStdString()));
        else if(ui->SNewPasswordValue->text().length() <= 6) qDebug() << "Password < 6 chars";
        else if(ui->SNewPasswordValue->text().length() >= 32) qDebug() << "Password > 32 chars";
    }
}

void OpenCodeGames::on_SCancelButton_clicked()
{
    ui->SUsernameValue->setText(username);
    ui->SPasswordValue->setText("");
    ui->SNewPasswordValue->setText("");
    ui->SRetypeNewPasswordValue->setText("");

    CenterDisp(1);
}
