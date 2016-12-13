#include "gaserver.h"
GaServer::GaServer(QObject *parent)
    :QTcpServer(parent)
{
    client.resize(0);
    sharedMemory.resize(SHAREDSIZE);
    sharedMemory.fill(0);
}


void GaServer::incomingConnection(qintptr socketDescriptor)
{
    for(int i=0;i<client.size();i++)
        if(client[i]->socketDescriptor==socketDescriptor)
            return;

    Client *C=new Client;
    
  C->socketDescriptor=socketDescriptor;
    C->socket=new QTcpSocket(this);
    C->socket->setSocketDescriptor(socketDescriptor);
    
    GaThread *thread = new GaThread(C, socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    C->thread=thread;
    C->parallelUpdated=false;
    connect(C->socket,SIGNAL(readyRead()),thread,SLOT(readDataSlot()));
    client.append(C);
    emit newClientSignal(C);
    thread->start();

   // C->socket->moveToThread(thread);
   /**/


}

void    GaServer::removeClient(int sockid)
{
    qDebug()<<"Calling Ga Server remove client with sockid="<<sockid<<endl;
    int pos=-1;
    for(int i=0;i<client.size();i++)
    {
        if(client[i]->socketDescriptor==sockid)
        {
            pos=i;
            break;
        }
    }
    if(pos!=-1)
    {

        client[pos]->thread->terminate();

        emit removeClientSignal(client[pos]);
        client.remove(pos);
    }
}

GaServer::~GaServer()
{
    for(int i=0;i<client.size();i++)
        delete client[i];
}

