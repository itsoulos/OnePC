#ifndef GASERVER_H
#define GASERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QVector>
#include "gathread.h"
#include "client.h"
# define SHAREDSIZE (1024*1024)
class GaServer  : public QTcpServer
{
    Q_OBJECT
public:
    GaServer(QObject *parent = 0);
    void    removeClient(int sockid);
    QByteArray sharedMemory;
    ~GaServer();
signals:
    void    changeClientMachine(Client *C,QString x);
    void    newClientSignal(Client *C);
    void    removeClientSignal(Client *C);
    void    statusClientSignal(Client *C,QString status,QString minimum);
    void    clientSendGenomesSignal(Client *C,QStringList x,QStringList y);
    void    clientRequestGenomesSignal(Client *C);
protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
public:
    QVector<Client*> client;
};

#endif // GASERVER_H
