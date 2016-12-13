#ifndef CLIENT_H
#define CLIENT_H
#include <QString>
class GaThread;
struct Client
{
    qintptr socketDescriptor;
    GaThread *thread;
    QTcpSocket *socket;
    QString typeOfMachine;
    QString status;
    QString bestx;
    double  besty;
    bool    parallelUpdated;
    QStringList parallelGenome;
    QStringList parallelFitness;
    QString machineName;
    int generations;
};

#endif // CLIENT_H

