#include "mainwindow.h"
#include <QApplication>
#include <locale.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
     printf ("Locale is: %s\n", setlocale(LC_ALL,"C") );
    /*
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
    */
    MainWindow w;
    w.show();

    return a.exec();
}
