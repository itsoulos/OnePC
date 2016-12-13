#include "gathread.h"
# include <QTcpServer>
#include <QtNetwork>
#include "gaserver.h"
#include "protocol.h"
extern GaServer server;

GaThread::GaThread(Client *c, int desc,QObject *parent)
    : QThread(parent)
{
    client = c;
    /** Now added 
    c->socket=new QTcpSocket(this);
    c->socketDescriptor=desc;
    c->socket->setSocketDescriptor(desc);
    socketId=c->socketDescriptor;
     end now added **/
    socketId=c->socketDescriptor;
    tcpSocket=client->socket;
    nextBlockSize=0;
}


static const QDataStream::Version kDSVersion = QDataStream::Qt_5_6;
void    GaThread::sendObject(QJsonObject &object)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(kDSVersion);
    QJsonDocument doc(object);
    qint32 d1=10;
    out<<d1;
    out<<doc.toJson();

    out.device()->seek(0);

    out << qint32(block.size()-sizeof(qint32));
    tcpSocket->write(block);
    //qDebug()<<"Server send json of size "<<block.size()<<endl;
    tcpSocket->flush();
}

void    GaThread::recvObject(QJsonObject &object)
{
    QDataStream in(tcpSocket);
    in.setVersion(kDSVersion);
    QByteArray byteArray;

       if (nextBlockSize == 0) {
           if (tcpSocket->bytesAvailable() < sizeof(quint32))
           {
            	tcpSocket->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
               //qDebug()<<"Error in bytes available"<<endl;
               return;
           }
           in >> nextBlockSize;
           //qDebug()<<"Next block size is "<<nextBlockSize<<endl;
       }
       if (tcpSocket->bytesAvailable() < nextBlockSize)
	{
            tcpSocket->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
           return;
	}
       in >> byteArray;
       QJsonParseError error;
       QJsonDocument reader = QJsonDocument::fromJson(byteArray, &error);

       if (error.error != QJsonParseError::NoError)
       {
          //qDebug()<<"Error in conversion of json"<<endl;
          //qDebug()<<"Error was "<<error.errorString()<<endl;
            QString str=QString::fromLatin1(byteArray);
           // qDebug()<<"Str was "<<str<<endl;
           return;
       }
       object=reader.object();
       nextBlockSize=0;
          // QDataStream out(tcpSocket);
          // out << quint16(0xFFFF);
}



void    GaThread::sendBytes(QByteArray &b)
{
    int size=b.size();
    int d1=tcpSocket->write((char*)&size,sizeof(size));
    tcpSocket->flush();
    int d2=tcpSocket->write(b);
    tcpSocket->flush();
}

void    GaThread::sendString(QString text)
{
    QByteArray b;
    b.append(text.toLatin1());

    int d1=tcpSocket->write(b);
    if(d1<=0)
    {
        //prepei na sbistei o pelatis
        tcpSocket->close();
        emit server.removeClientSignal(client);
        return;
    }
    tcpSocket->flush();
}


void    GaThread::readDataSlot()
{
    int dataSize;
    QByteArray b;
    /*
    while(tcpSocket->bytesAvailable())
    {

        b=tcpSocket->read(3);
        QString message=QString::fromLatin1(b);
        qDebug()<<"Received message was "<<message<<endl;
         if(tcpSocket->bytesAvailable()==0) tcpSocket->waitForReadyRead(-1);
        tcpSocket->read((char*)&dataSize, sizeof(int));
        qDebug()<<"Datasize is "<<dataSize<<endl;

       b = tcpSocket->read(dataSize);
        qDebug()<<"Read b of size "<<b.size()<<endl;
        while(b.size() < dataSize) // only part of the message has been received
        {
            tcpSocket->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
            b.append(tcpSocket->read(dataSize - b.size())); // append the remaining bytes of the message
            qDebug()<<"Now b size is "<<b.size()<<" Datasize is "<<dataSize<<endl;
        }


    QString str=QString::fromLatin1(b);
    qDebug()<<"Received string was "<<str<<" B size is "<<b.size()<<endl;
    QJsonDocument doc=QJsonDocument::fromJson(str.toUtf8());
    QJsonObject obj=doc.object();

     message=obj["message"].toString();
    */
    while(tcpSocket->bytesAvailable())
    {
        QJsonObject obj;
        recvObject(obj);
        QString message=obj["message"].toString();
    if(message.size()==0) continue;
      //qDebug()<<"Received message is "<<message<<endl;
    if(message==CM_ASKSERVERFORCONNECT)
    {
        QString machine=obj["machine"].toString();
        if(machine=="debian") machine="ubuntu";
        client->typeOfMachine=machine;//list[1];
        client->machineName=obj["machinename"].toString();
        emit server.changeClientMachine(client,machine);
        QJsonObject object;
        object["message"]=CM_SERVERSENDCODE;
        object["socket"]=client->socketDescriptor;
        sendObject(object);
        /*
        sendString(CM_SERVERSENDCODE);
        QByteArray bnew;
        QString tx=QString::number(client->socketDescriptor);
        bnew.append(tx.toLatin1());
        sendBytes(bnew);
        */
    }
    else
    if(message==CM_CLIENTDELETE)
    {
        server.removeClient(socketId);
        tcpSocket->close();
    }
    else
    if(message==CM_CLIENTSENDSTATUS)
    {
        QString minimum=obj["minimum"].toString();
        QString status=obj["status"].toString();
        int  generations=obj["generations"].toInt();
        client->besty=minimum.toDouble();
        client->bestx=obj["bestx"].toString();
        client->generations=generations;
        emit server.statusClientSignal(client,status,minimum);
    }
    else
    if(message==CM_CLIENTGETMEMORY)
    {
         QString pos=obj["position"].toString();
         QString count=obj["count"].toString();
         QByteArray data=server.sharedMemory.mid(pos.toInt(),count.toInt());
         sendString(CM_SERVERSENDMEMORY);
         sendBytes(data);
    }
    else
    if(message==CM_CLIENTSETMEMORY)
    {
        QString pos=obj["position"].toString();
        QString count=obj["count"].toString();
        QString data=obj["data"].toString();
    }
    else
    if(message==CM_CLIENTSENDGENOMES)
    {
        int count=obj["count"].toInt();
        QJsonArray array=obj["array"].toArray();
        QStringList x,y;
        for(int i=0;i<count;i++)
        {
            QJsonObject obj=array[i].toObject();
            QString xvalue=obj["xvalue"].toString();
            QString yvalue=obj["yvalue"].toString();
            x.append(xvalue);
            y.append(yvalue);
        }
        emit server.clientSendGenomesSignal(client,x,y);
    }
    else
    if(message==CM_CLIENTASKGENOMES)
    {
        /*
         * 27/8/2016
         * Σε αυτό το σημείο μαθαίνουμε πότε ο πελάτης ζητά από το σύστημα
         * νέα χρωμοσώματα. Από τους χρόνους που μεσολαβούν μπορούμε να καταλάβουμε
         * πόσο γρήγορος είναι ο κάθε πελάτης στο έργο του και ανάλογα το σύστημα
         * να προσαρμοστεί.
        */
        qint64 timestamp=QDateTime::currentMSecsSinceEpoch();
        //qDebug()<<"Client is "<<client->machineName<<" Time is "<<timestamp<<endl;
        client->generations=obj["generations"].toInt();
        emit server.clientRequestGenomesSignal(client);
    }

//   qDebug()<<"Message was "<<message<<endl;
}
}

void GaThread::run()
{

    //connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readDataSlot()));

    exec();
}
