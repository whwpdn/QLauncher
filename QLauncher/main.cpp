#include "qlauncher.h"
//#include "QLauncherApp.h"
#include <QtGui/QApplication>
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLauncher w;
    w.show();

    return a.exec();
}
