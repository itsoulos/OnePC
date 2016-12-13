#ifndef CLIENTPROTOCOL_H
#define CLIENTPROTOCOL_H
#include <QSysInfo>
#include <QDebug>
#include <QTcpSocket>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QSettings>
#include <QDateTime>
#include <QAbstractSocket>
#include <QLibrary>
#include <QJsonObject>
#include <QJsonArray>
# include <QJsonDocument>
# include <QFile>
# include <QStandardPaths>
# include <QApplication>
#include <QDataStream>
#include <QHostInfo>
#include "plotarea.h"
#include "myproblem.h"
#include "mythread.h"
#include "population.h"
#include "global.h"
#include "protocol.h"
#include <QObject>

class ClientProtocol : public QObject
{
    Q_OBJECT
public:
    explicit ClientProtocol(Global *g,PlotArea *p,QObject *parent = 0);
    void     openSession();
    QString  recvString();
    void     pauseExecution();
    void     resumeExecution();
    void     setPlotArea(PlotArea *p);
    void    sendObject(QJsonObject &object);
    void    recvObject(QJsonObject &object);
private:
    void    runProblem(QString settings);
    Global  *global;
    PlotArea *plotarea;
    QLibrary *library;
    MyThread *thread;
    MyProblem *problem;
    Population *population;
    QTcpSocket *tcpSocket;
    QNetworkSession *networkSession;

    int blockSize;
    bool connected;
    QByteArray codeBytes;
    QString socketId;
    QString status;
    double  globalMinimum;
    QString globalMinimumX;
    int generations;
    void    sendStatus();
    void    sendJson(QJsonObject &object);
    int     numberOfGenomes;//arithmos xromosomaton pou tha stelnei i efarmogi ston server

    qint32 nextBlockSize;
public slots:
    void    connectToServer();
    void    sessionOpened();
    void    readData();
    void    connectSlot();
    void    endThread();
    void    newMinimumSlot(QString s,double v,int g);
    void    sendGenomesSlot(QStringList x,QStringList y);
    void    requestGenomesSlot();
    void    displayError(QAbstractSocket::SocketError socketError);
signals:
    void socketErrorSignal(QAbstractSocket::SocketError socketError);
};

#endif // CLIENTPROTOCOL_H
