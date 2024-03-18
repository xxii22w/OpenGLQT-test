#include "TestDialog.h"
#include <QtWidgets/QApplication>
#include <ctime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    srand(time(nullptr));
    TestDialog w;
    w.show();
    return a.exec();
}
