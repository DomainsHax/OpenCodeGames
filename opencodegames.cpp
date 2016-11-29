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
    StartPageDisp(1);

    QHostInfo ip = QHostInfo::fromName("localhost");
    if(!ip.addresses().isEmpty()) addr = ip.addresses().first();
}

OpenCodeGames::~OpenCodeGames(){ delete ui; }

void OpenCodeGames::Connection(const QJsonObject jsonObjConn)
{
    //WRITE
    QTcpSocket connection_info;
    connection_info.connectToHost(addr, 8000);
    connection_info.write(QString(QJsonDocument(jsonObjConn).toJson(QJsonDocument::Compact)).toStdString().c_str());

    //READ
    connection_info.waitForReadyRead();
    QByteArray response = connection_info.read(800);
    QJsonObject jsonObj = QJsonDocument::fromJson(response).object();

    if(jsonObj.value("method") == "login_completed")
    {
        QString username_login = jsonObj.value("username").toString();
        token = jsonObj.value("token").toString();
        LoginCompleted = true;

        PlayerStatsConnect();
        GamesListConnect();

        ui->Login->setVisible(false);
        ui->Menu->setVisible(true);

        qDebug() << "LOGIN_COMPLETED " << username_login << " " << token;
    }
    else if(jsonObj.value("method") == "account_no_verified"){ qDebug() << "The Account isn't verified"; ui->status_Login->setText("The Account isn't verified"); }
    else if(jsonObj.value("method") == "account_password_invalid"){ qDebug() << "Password invalid"; ui->status_Login->setText("Password invalid"); }
    else if(jsonObj.value("method") == "account_inexistent"){ qDebug() << "The Account is inexistent"; ui->status_Login->setText("The Account is inexistent"); }
    else if(jsonObj.value("method") == "register_completed")
    {
        QString username_register = jsonObj.value("username").toString();
        QString email_register = jsonObj.value("email").toString();

        RegisterCompleted = true;
        ui->status_Register->setText("REGISTER_COMPLETED");
        qDebug() << "REGISTER_COMPLETED " << username_register << " " << email_register;
    }
    else if(jsonObj.value("method") == "no-register")
    {
        QString error_str;
        int error = jsonObj.value("errors").toInt();
        if(error == 1) error_str = "Username aldreay exist";
        else if(error == 2) error_str = "Email aldreay exist";
        else if(error == 3) error_str = "Username and Email aldreay exist";

        ui->status_Register->setText(error_str);
        qDebug() << "NO-REGISTER " << error_str;
    }
    else if(jsonObj.value("method") == "no-register_lenght") ui->status_Register->setText("NO-REGISTER_LENGHT");
    else if(jsonObj.value("method") == "info_results")
    {
        username = jsonObj.value("username").toString();
        token = jsonObj.value("token").toString();
        LevelValue = jsonObj.value("level_value").toInt();
        ProgressValue = jsonObj.value("progress_value").toInt();
        ProgressEnd = jsonObj.value("progress_end").toInt();
        ProgressPercentual = ((float)ProgressValue * 100) / (float)ProgressEnd;

        float ProgressFloat = (ProgressPercentual * 162) / 100;
        if(ProgressFloat >= (float)((int)ProgressFloat)+.5) Progress = ProgressFloat + .5;
        else Progress = (int)ProgressFloat;

        ui->UserNameValue->setText(username);
        ui->SUsernameValue->setText(username);

        ui->ProgressLevel->setText(("<font color='white'>" + std::to_string(LevelValue) + "</font>").c_str());
        ui->ProgressBar->resize(Progress, 10);

        if(ProgressValue == 100800 && ProgressEnd < 100800) ui->ProgressValue->setText(("<font color='white'>" + std::to_string(ProgressValue) + "/" + std::to_string(ProgressEnd) + "</font>").c_str());
        else ui->ProgressValue->setText("<font color='white'>MAX</font>");

        qDebug() << "INFO_RESULTS " << Progress << "   " << ProgressFloat << "   " << ProgressPercentual << "%     " << ProgressValue << '/' << ProgressEnd;
    }
    else if(jsonObj.value("method") == "games")
    {
        QJsonArray jsonArray = jsonObj.value("games_list").toArray();
        int max = jsonArray.size();

        for(int i = 0; i < max; i++)
        {
            QJsonObject jsonObjGame = jsonArray[i].toObject();
            MapGames[i + 1].ID = i + 1;
            MapGames[i + 1].Name = jsonObjGame.value("name").toString();
            MapGames[i + 1].Server = jsonObjGame.value("server").toString();
            MapGames[i + 1].Port = jsonObjGame.value("port").toInt();
            MapGames[i + 1].Version = jsonObjGame.value("version").toString();
        }

        Games_List();
        qDebug() << "GAMES";
    }
    else if(jsonObj.value("method") == "set_username")
    {
        username = jsonObj.value("username").toString();
        ui->UserNameValue->setText(username);
        ui->SUsernameValue->setText(username);
        qDebug() << "Username is changed " << username;
    }
    else if(jsonObj.value("method") == "username_already_setted") qDebug() << "Username already setted";
    else if(jsonObj.value("method") == "username_already_exists") qDebug() << "Username already exists";
    else if(jsonObj.value("method") == "no-change_username") qDebug() << "Username isn't changed (chars incorrect)";
    else if(jsonObj.value("method") == "set_password") qDebug() << "Password is changed";
    else if(jsonObj.value("method") == "password_already_setted") qDebug() << "Password is already setted";
    else if(jsonObj.value("method") == "set_password_incorrect") qDebug() << "Last Password isn't correct";
    else if(jsonObj.value("method") == "no-change_password") qDebug() << "Password isn't changed (chars incorrect)";
    else if(jsonObj.value("method") == "token_doesnt_exists") qDebug() << "Token doesn't exists";
    else if(jsonObj.value("method") == "invalid-request") qDebug() << "Invalid Request";
}

void OpenCodeGames::GamesListConnect(){ QJsonObject jsonObj; jsonObj.insert("method", "games"); jsonObj.insert("token", token); Connection(jsonObj); }
void OpenCodeGames::PlayerStatsConnect(){ QJsonObject jsonObj; jsonObj.insert("method", "info_username"); jsonObj.insert("token", token); Connection(jsonObj); }

std::string OpenCodeGames::json_to_string(const QJsonObject jsonObj){ return QString(QJsonDocument(jsonObj).toJson(QJsonDocument::Compact)).toStdString(); }

void OpenCodeGames::StartPageDisp(int disposition)
{
    ui->Menu->setVisible(false);
    ui->Login->setVisible(false);
    ui->Register->setVisible(false);

    if(disposition == 1) ui->Login->setVisible(true);
    else ui->Register->setVisible(true);
}

void OpenCodeGames::on_L_RegisterButton_clicked(){ StartPageDisp(2); }
void OpenCodeGames::on_R_LoginButton_clicked(){ StartPageDisp(1); }

void OpenCodeGames::on_button_Login_clicked()
{
    if(!LoginCompleted)
    {
        QString Username = ui->L_Username->text();
        QString Password = ui->L_Password->text();

        if((!Username.isEmpty() && Username.size() >= 3) && (!Password.isEmpty() && Password.size() >= 6))
        { QJsonObject jsonObj; jsonObj.insert("method", "login"); jsonObj.insert("username", Username); jsonObj.insert("password", Password); Connection(jsonObj); }

        if(Username.isEmpty()) ui->L_statusUsername->setText("<font color='red'>Username is empty</font>");
        else if(Username.size() < 3) ui->L_statusUsername->setText("<font color='red'>Username shorter than 3 chars</font>");
        else ui->L_statusUsername->setText("");

        if(Password.isEmpty()) ui->L_statusPassword->setText("<font color='red'>Password is empty</font>");
        else if(Password.size() < 6) ui->L_statusPassword->setText("<font color='red'>Password shorter than 6 chars</font>");
        else ui->L_statusPassword->setText("");
    }
}

void OpenCodeGames::on_button_Register_clicked()
{
    if(!RegisterCompleted)
    {
        QString Username = ui->R_Username->text();
        QString Email = ui->R_Email->text();
        QString Password = ui->R_Password->text();
        QString RetypePassword = ui->R_RetypePassword->text();

        if(Password == RetypePassword && (!Username.isEmpty() && Username.size() >= 3) && (!Email.isEmpty()) && (!Password.isEmpty() && Password.size() >= 6) && (!RetypePassword.isEmpty() && RetypePassword.size() >= 6))
        { QJsonObject jsonObj; jsonObj.insert("method", "register"); jsonObj.insert("username", Username); jsonObj.insert("email", Email); jsonObj.insert("password", Password); Connection(jsonObj); }

        if(Username.isEmpty()) ui->R_statusUsername->setText("<font color='red'>Username is empty</font>");
        else if(Username.size() < 3) ui->R_statusUsername->setText("<font color='red'>Username shorter than 3 chars</font>");
        else ui->R_statusUsername->setText("");

        if(Email.isEmpty()) ui->R_statusEmail->setText("<font color='red'>Email is empty</font>");
        else ui->R_statusEmail->setText("");

        if(Password.isEmpty()) ui->R_statusPassword->setText("<font color='red'>Password is empty</font>");
        else if(Password.size() < 6) ui->R_statusPassword->setText("<font color='red'>Password shorter than 6 chars</font>");
        else ui->R_statusPassword->setText("");

        if(RetypePassword.isEmpty()) ui->R_statusRetypePassword->setText("<font color='red'>Retype Password is empty</font>");
        else if(RetypePassword.size() < 6) ui->R_statusRetypePassword->setText("<font color='red'>Retype Password shorter than 6 chars</font>");
        else ui->R_statusRetypePassword->setText("");

        if(Password != RetypePassword) ui->status_Register->setText("<font color='red'>Password != Retype Password</font>");
        else ui->status_Register->setText("");
    }
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
        QWidget *GameStatus = new QWidget, *pic = new QWidget;
        QLabel *Name = new QLabel(MapGames[i].Name);
        QGridLayout *GameStatusLayout = new QGridLayout;
        QPushButton *Status = new QPushButton;

        QString Version = MapGames[i].Version, Vers = NULL, dir = "/home/domainshax/OpenCodeGames/OpenCodeGames/Games/" + MapGames[i].Name;
        QDir *DIRECTORY = new QDir(dir);
        QFile *VERSION = new QFile(dir + "/VERSION.txt"), *EXEC = new QFile(dir + "/" + MapGames[i].Name);

        if(DIRECTORY->exists())
            if(VERSION->exists()){ VERSION->open(QIODevice::ReadOnly); Vers = VERSION->readAll(); }

        GameStatus->setStyleSheet("background-color:grey;");
        if(max <= 4) GameStatus->setFixedSize(306, 83);
        else GameStatus->setFixedSize(292, 84);

        pic->setFixedSize(65, 65);
        pic->setStyleSheet("background-color:blue;");

        Name->setFixedSize(150, 15);
        Name->setAlignment(Qt::AlignCenter);
        Name->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

        Status->setFixedSize(45, 30);

        GameStatusLayout->addWidget(pic, 0, 0, 0, 0, Qt::AlignLeft);
        GameStatusLayout->addWidget(Name, 0, 2, 0, 4, Qt::AlignTop);

        if(Vers == Version && DIRECTORY->exists() && VERSION->exists() && EXEC->exists())
        {
            Status->setText("Play");
        }
        else if(!DIRECTORY->exists() && !EXEC->exists() && !VERSION->exists())
        {
            Status->setText("Install");

            QSignalMapper* signalMapper = new QSignalMapper (this) ;
            connect (Status, SIGNAL(clicked()), signalMapper, SLOT(map())) ;
            signalMapper -> setMapping (Status, i) ;
            connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(InstallButton(int))) ;
        }
        else if((Vers != Version or !EXEC->exists() or !VERSION->exists()))
        {
            Status->setText("Update");
        }

        GameStatusLayout->addWidget(Status, 1, 5, 1, 1, Qt::AlignBottom);
        GamesScrollLayout->addWidget(GameStatus);
        GameStatus->setLayout(GameStatusLayout);
    }

    ui->PlayScrollAreaWidget->setLayout(GamesScrollLayout);
}

void OpenCodeGames::on_SSaveButton_clicked()
{
    int settings = 0;
    if(username != ui->SUsernameValue->text() && !ui->SUsernameValue->text().isEmpty()) settings += 1;
    if(ui->SNewPasswordValue->text() == ui->SRetypeNewPasswordValue->text() && !ui->SPasswordValue->text().isEmpty() && !ui->SNewPasswordValue->text().isEmpty() && !ui->SRetypeNewPasswordValue->text().isEmpty()) settings += 2;

    if(settings == 1) SettingsUsername();
    else if(settings == 2) SettingsPassword();
    else if(settings == 3){ SettingsUsername(); QTimer::singleShot(500,this,SLOT(SettingsPassword())); }
}

void OpenCodeGames::SettingsUsername()
{
    if(ui->SUsernameValue->text().length() >= 3 && ui->SUsernameValue->text().length() <= 16)
    { QJsonObject jsonObj; jsonObj.insert("method", "settings_username"); jsonObj.insert("token", token); jsonObj.insert("username", ui->SUsernameValue->text()); Connection(jsonObj); }
    else qDebug() << "Username < 3 chars";
}

void OpenCodeGames::SettingsPassword()
{
    if((ui->SPasswordValue->text().length() >= 6 && ui->SPasswordValue->text().length() <= 32) && (ui->SNewPasswordValue->text().length() >= 6 && ui->SNewPasswordValue->text().length() <= 32) && (ui->SRetypeNewPasswordValue->text().length() >= 6 && ui->SRetypeNewPasswordValue->text().length() <= 32))
    { QJsonObject jsonObj; jsonObj.insert("method", "settings_password"); jsonObj.insert("token", token); jsonObj.insert("password", ui->SPasswordValue->text()); jsonObj.insert("new_password", ui->SNewPasswordValue->text()); Connection(jsonObj); }
    else qDebug() << "Password < 6 chars";
}

void OpenCodeGames::on_SCancelButton_clicked()
{
    ui->SUsernameValue->setText(username);
    ui->SPasswordValue->setText("");
    ui->SNewPasswordValue->setText("");
    ui->SRetypeNewPasswordValue->setText("");
    CenterDisp(1);
}

void OpenCodeGames::ConnectionGames(const QJsonObject jsonObjConnGames)
{
    //WRITE
    QTcpSocket connection_games;
    connection_games.connectToHost(addr, 8000);
    connection_games.write(QString(QJsonDocument(jsonObjConnGames).toJson(QJsonDocument::Compact)).toStdString().c_str());

    //READ
    connection_games.waitForReadyRead();
    QByteArray response = connection_games.read(1000000);

    QFile* exec = new QFile("/home/domainshax/OpenCodeGames/OpenCodeGames/Games/LOL");
    exec->open(QIODevice::WriteOnly);
    exec->write(response);
    exec->close();

    qDebug() << response;
}

void OpenCodeGames::InstallButton(int i){ QJsonObject jsonObj; jsonObj.insert("method", "install"); jsonObj.insert("id", i); ConnectionGames(jsonObj); }
