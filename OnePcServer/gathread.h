#ifndef GATHREAD_H
#define GATHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "client.h"
class GaThread : public QThread
{

    Q_OBJECT

public:
    GaThread(Client *c, int desc,QObject *parent);
    void run() Q_DECL_OVERRIDE;
    void     sendString(QString s);
    void     sendBytes(QByteArray &b);
    void    sendObject(QJsonObject &object);
    void    recvObject(QJsonObject &object);
    qint32 nextBlockSize;
signals:
    void error(QTcpSocket::SocketError socketError);

private:
    Client *client;
    QTcpSocket *tcpSocket;
    int socketId;
public slots:
    void    readDataSlot();
};

#endif // GATHREAD_H
