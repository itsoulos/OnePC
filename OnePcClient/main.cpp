#include "mainwindow.h"
#include <QApplication>
# include <QFont>
# include <QStandardPaths>
#include <QCoreApplication>
#include "clientprotocol.h"
#include <locale.h>
bool hasGui=true;
QString machineName="";

void    printHelp()
{
    qDebug()<<"OnePC parameter help screen";
    qDebug()<<"========================================";
    qDebug()<<"--chromosomes    <int>, default 200";
    qDebug()<<"--generations    <int>, default 200";
    qDebug()<<"--gui            <string>, default yes";
    qDebug()<<"--localsearchchromosomes <int>, default 20";
    qDebug()<<"--localsearchgenerations <int>, default 50";
    qDebug()<<"--machinename    <string>, default ''";
    qDebug()<<"--mutationrate   <double>, default 0.05";
    qDebug()<<"--parallelchromosomes <int>, default 10";
    qDebug()<<"--seed           <int>, default ''";
    qDebug()<<"--selectionrate  <double>, default 0.90";
    qDebug()<<"--serverip       <string>,default ''";
    qDebug()<<"--serverport     <int>,default ''";
    exit(EXIT_SUCCESS);
}


void parseCommandLine(Global *g,QStringList args)
{
    for(int i=0;i<args.size();i++)
    {
        QStringList sp=args[i].split("=");
        if(sp[0]=="--generations")
        {
            g->setGenerations(sp[1].toInt());
        }
        else
        if(sp[0]=="--chromosomes")
        {
            g->setChromosomes(sp[1].toInt());
        }
        else
        if(sp[0]=="--selectionrate")
        {
            g->setSelectionRate(sp[1].toDouble());
        }
        else
        if(sp[0]=="--mutationrate")
        {
            g->setMutationRate(sp[1].toDouble());
        }
        else
        if(sp[0]=="--serverip")
        {
            g->setIp(sp[1]);
        }
        else
        if(sp[0]=="--serverport")
        {
            g->setPort(sp[1].toInt());
        }
        else
        if(sp[0]=="--gui")
        {
            if(sp[1]=="yes" || sp[1]=="true") hasGui=true;
            else
            if(sp[1]=="no"  || sp[1]=="false")
            hasGui=false;
        }
        else
        if(sp[0]=="--seed")
        {
            int seed=sp[1].toInt();
            srand(seed);
            srand48(seed);
            qDebug()<<"Seed is "<<seed<<endl;
        }
        else
        if(sp[0]=="--machinename")
        {
            machineName=sp[1];
            g->setMachineName(machineName);
        }
        else
        if(sp[0]=="--parallelchromosomes")
        {
            int c=sp[1].toInt();
            g->setParallelChromosomes(c);
        }
        else
        if(sp[0]=="--localsearchchromosomes")
        {
            int c=sp[1].toInt();
            g->setLocalSearchChromosomes(c);
        }
        else
        if(sp[0]=="--localsearchgenerations")
        {
            int c=sp[1].toInt();
            g->setLocalSearchGenerations(c);
        }
        else
        if(sp[0]=="--help")
        {
            printHelp();
        }
    }
}

int main(int argc, char *argv[])
{
   QCoreApplication *app=new QCoreApplication(argc,argv);
   QApplication *guiApp;
   MainWindow *w;
   ClientProtocol *protocol;
   setlocale(LC_ALL,"C");
   Global *global=new Global();

   QString xx=getenv("EXTERNAL_STORAGE");
   QString paramfile=xx+"/paramfile";
   QFile file(paramfile);
   if(file.exists())
    {
        file.open(QFile::ReadOnly);
        QStringList buffer;
        QTextStream fp(&file);
        QString line;
        do
        {
            line=fp.readLine();
            if(line.length()<2) break;
            buffer.append(line);
        }while(!line.isNull());
        file.close();
        parseCommandLine(global,buffer);
    }
    else
    parseCommandLine(global,app->arguments());
    protocol=new ClientProtocol(global,NULL);
    if(hasGui)
    {
        delete app;
        guiApp=new QApplication(argc,argv);
        w=new MainWindow(protocol,global);

     if(QSysInfo::productType()=="ubuntu" || QSysInfo::productType()=="debian"  || QSysInfo::productType()=="windows" || QSysInfo::productType()=="osx")
     {

            QFont font("arial",14,QFont::Bold);
            guiApp->setFont(font);
            w->show();

     }
     else
     {
            QFont font("arial",18,QFont::Bold);
            guiApp->setFont(font);
            w->showFullScreen();
     }
            return guiApp->exec();
    }
    else
    {

     return app->exec();
    }
}
