#include "clientprotocol.h"

ClientProtocol::ClientProtocol(Global *g,PlotArea *p,QObject *parent) : QObject(parent)
{
    globalMinimum=0;
    globalMinimumX="";
    plotarea=p;
    global=g;
    connected=false;
    numberOfGenomes=g->getParallelChromosomes();
    nextBlockSize=0;
    status=STATE_WAIT;
    thread=NULL;
    problem=NULL;
    population=NULL;
    library=NULL;

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,SIGNAL(connected()),this,SLOT(connectSlot()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
    openSession();
    if(global->getIp()!="" && global->getPort()!=0)
        connectToServer();
}

void    ClientProtocol::displayError(QAbstractSocket::SocketError socketError)
{
    emit socketErrorSignal(socketError);
}

void    ClientProtocol::setPlotArea(PlotArea *p)
{
    plotarea=p;
}

static const QDataStream::Version kDSVersion = QDataStream::Qt_5_6;
void    ClientProtocol::sendObject(QJsonObject &object)
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
    tcpSocket->flush();
}

void    ClientProtocol::recvObject(QJsonObject &object)
{
    QDataStream in(tcpSocket);
    in.setVersion(kDSVersion);
    QByteArray byteArray;
       if (nextBlockSize == 0) {
           if (tcpSocket->bytesAvailable() < sizeof(qint32))
           {

               return;
           }
           in >> nextBlockSize;
       }
       if (tcpSocket->bytesAvailable() < nextBlockSize)
       {

           return;
       }
        in >> byteArray;
         nextBlockSize=0;
       QJsonParseError error;
       QJsonDocument reader = QJsonDocument::fromJson(byteArray, &error);

       if (error.error != QJsonParseError::NoError)
       {
           return;
       }
       object=reader.object();
}


 void   ClientProtocol::connectSlot()
 {
    connected=true;
    QString message=CM_ASKSERVERFORCONNECT;
    QJsonObject object;
    object["message"]=message;
    object["machine"]=QSysInfo::productType();
    object["machinename"]=global->getMachineName();
    sendJson(object);
    status=STATE_START;
 }

void     ClientProtocol::readData()
{
   QString s=recvString();
}


void     ClientProtocol::openSession()
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));
        networkSession->open();
    }
}

void    ClientProtocol::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();
}

void    ClientProtocol::connectToServer()
{
    if(!connected)
    {
        QString s1;
        s1=global->getIp();
        QString s2;
        s2=QString::number(global->getPort());
        tcpSocket->abort();
        tcpSocket->connectToHost(s1,
                             s2.toInt());
        global->setIp(s1);
        global->setPort(s2.toInt());
    }
}

void    ClientProtocol::runProblem(QString settings)
{
    status=STATE_RUN;
    /** It appears to be some problem here
     * for repeating executions of the client
     * The best is to wait the client to terminate its execution
     * before starting another Population **/
    /*
    if(thread!=NULL)
    {
        thread->terminate();
        delete thread;
        thread=NULL;
    }
    if(thread!=NULL && thread->stopflag==1)
    {
        thread->stopflag=0;
        thread->start();
        return ;
    }
    if(thread!=NULL && thread->isRunning())
    {

        thread->stopflag=1;
        thread->wait();
        thread->terminate();
    }
    */
    if(thread!=NULL)
    {
        delete thread;
        thread=NULL;
    }

    if(library!=NULL)
    {
       library->unload();
       delete library;

    }
    QString xx=getenv("EXTERNAL_STORAGE");
    QString location=QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    xx=location;
    if(xx!="") xx=xx+"/";
    QString libraryname=xx+"experiment"+socketId+".so";
    QFile file(libraryname);
    file.open(QIODevice::WriteOnly);
    file.write(codeBytes);
    file.close();
    library=new QLibrary(libraryname);
    if(problem!=NULL) delete problem;
    problem=new MyProblem(library,settings);
    if(plotarea!=NULL) plotarea->clearPoints();
    if(population!=NULL) delete population;
    int genome_count,generations;
    double mutation_rate,selection_rate;
    global->getSettings(selection_rate,mutation_rate,genome_count,generations);
    population=new Population(genome_count,problem);
    population->setMutationRate(mutation_rate);
    population->setSelectionRate(selection_rate);
    population->setLocalSearchChromosomes(global->getLocalSearchChromosomes());
    population->setLocalSearchGenerations(global->getLocalSearchGenerations());
    thread=new MyThread(population,plotarea,generations,numberOfGenomes);
    connect(thread,SIGNAL(completed()),this,SLOT(endThread()));
    connect(thread,SIGNAL(newMinimum(QString,double,int)),this,SLOT(newMinimumSlot(QString,double,int)));
    connect(thread,SIGNAL(sendGenomes(QStringList,QStringList)),this,SLOT(sendGenomesSlot(QStringList,QStringList)));
    connect(thread,SIGNAL(requestGenomes()),this,SLOT(requestGenomesSlot()));
    thread->start();
 }
void    ClientProtocol::sendJson(QJsonObject &object)
{
    sendObject(object);
}

void    ClientProtocol::requestGenomesSlot()
{
    QString message=CM_CLIENTASKGENOMES;
    QJsonObject object;
    object["message"]=message;
    object["generations"]=population->getGeneration();
    sendJson(object);
}

void    ClientProtocol::sendStatus()
{
    QString message=CM_CLIENTSENDSTATUS;
    QJsonObject object;
    object["message"]=message;
    object["status"]=status;
    object["minimum"]=QString::number(globalMinimum);
    object["generations"]=population->getGeneration();
    object["bestx"]=globalMinimumX;
    sendJson(object);
}

void    ClientProtocol::sendGenomesSlot(QStringList x,QStringList y)
{
    //send a number of genomes to the server
    QString message=CM_CLIENTSENDGENOMES;
    QJsonObject object;
    object["message"]=message;
    object["count"]=y.size();
    QJsonArray array;
    for(int i=0;i<x.size();i++)
    {
        QJsonObject obj;
        obj["xvalue"]=x.at(i);
        obj["yvalue"]=y[i];
        array.append(obj);
    }
    object["array"]=array;
    sendJson(object);
}

void    ClientProtocol::newMinimumSlot(QString s,double v,int g)
{
    globalMinimumX=s;
    globalMinimum=v;
    generations=g;
    sendStatus();
}

void    ClientProtocol::endThread()
{
    status=STATE_FINISH;
    if(problem!=NULL) problem->callDone();
    sendStatus();
}

QString ClientProtocol::recvString()
{
   QString message;
   int dataSize;
  QJsonObject object;
   if(tcpSocket->bytesAvailable())
    recvObject(object);
   message=object["message"].toString();
   qDebug()<<"Message is "<<message<<endl;
   if(message.size()==0) return "";
//codeBytes=buffer;
   if(message==CM_SERVERSENDPROGRAM)
    {
       QString ss=object["data"].toString();
       QByteArray ba;
       ba.append(ss);
       codeBytes=QByteArray::fromBase64(ba);
       qDebug()<<"Settings at receive is "<<object["parameters"].toString()<<endl;
       runProblem(object["parameters"].toString());
    }
   else
   if(message==CM_SERVERSENDCODE)
   {
    socketId=QString::number(object["socket"].toInt());
   }
   else
   if(message==CM_SERVERASKSTATUS)
   {
     sendStatus();
   }
   else
   if(message==CM_SERVERSENDTERMINATE)
   {
       if (thread!=NULL)
           thread->terminate();
        qApp->quit();
   }
   else
   if(message==CM_SERVERASKPAUSE)
   {
        pauseExecution();
   }
   else
   if(message==CM_SERVERASKRESUME)
   {
       resumeExecution();
   }
   else
   if(message==CM_SERVERSENDMINIMUM)
   {
        //o client paralambanei ena elaxisto se morfi json
        //An o population einai energos mpainei se auton.
       //Me auton ton tropo enimeronontai oloi oi plithismoi
       //gia kapoio neo elaxisto pou exei brethei. Prosoxi
       //kathos auto to elaxisto mporei kai na agnoithei.
       if(population!=NULL   && thread!=NULL && thread->isRunning())
       {

           QString json=QString::fromLatin1(codeBytes);
           QJsonDocument doc=QJsonDocument::fromJson(json.toUtf8());
           QJsonObject obj=doc.object();
           QString besty=obj["besty"].toString();
           QString bestx=obj["bestx"].toString();
           Data xvalue;
           double yvalue=besty.toDouble();
           QStringList list=bestx.split(",");
           for(int i=0;i<list.size();i++)
               xvalue.push_back(list[i].toDouble());
           population->replaceWorst(yvalue,xvalue);

       }
   }
   else
   if(message==CM_SERVERSENDMEMORY)
   {

   }
   else
   if(message==CM_SERVERSENDGENOMES)
   {
       QString json=QString::fromLatin1(codeBytes);
       QJsonDocument doc=QJsonDocument::fromJson(json.toUtf8());
       QJsonObject obj=doc.object();
       int count=obj["count"].toInt();
       QStringList x;
       QStringList y;
       QJsonArray array=obj["array"].toArray();
       for(int i=0;i<count;i++)
       {
            QJsonObject object=array[i].toObject();
            QString xvalue=object["xvalue"].toString();
            QString yvalue=object["yvalue"].toString();
            x.append(xvalue);
            y.append(yvalue);
       }


       population->setGenomes(x,y);
       thread->enableParallelData();

   }
return "";

}

void    ClientProtocol::resumeExecution()
{
    if(thread==NULL) return;

    if(thread!=NULL && thread->stopflag==1)
    {
        thread->stopflag=0;
        thread->start();
        status=STATE_RUN;
        return;
    }

}

void    ClientProtocol::pauseExecution()
{
    if(thread==NULL) return;
    if(thread->stopflag==1) return;
    thread->stopflag=1;
    /** hangs forever sometimes **/
    thread->wait();
    status=STATE_PAUSE;
}
