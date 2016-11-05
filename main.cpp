#include "opencodegames.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenCodeGames w;
    w.show();

    return a.exec();
}
