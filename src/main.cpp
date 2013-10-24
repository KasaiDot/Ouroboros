#include "ouroboros.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ouroboros w;
    w.show();

    return a.exec();
}
