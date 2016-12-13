#include "mainwindow.h"
#include <QStandardPaths>
#include <QHostAddress>
GaServer server;
# define WINDOW_WIDTH   800
# define WINDOW_HEIGHT  600
# define ICON_WIDTH     (WINDOW_WIDTH/10)
# define ICON_HEIGHT    (WINDOW_HEIGHT/15)
# define SERVERPORT 51000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    db=new Database();
    parser=NULL;
    activeProblem="";
    /*
     * Prepare the server
    */
    QHostAddress serverAddr(getIpAddress());
    bool retValue=false;
    retValue=server.listen(serverAddr,SERVERPORT);
    if(!retValue) retValue=server.listen(serverAddr);
    if (!retValue)
    {
        QMessageBox::critical(this, tr("Genetics Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server.errorString()));
        close();
        return;
    }
    connect(&server,SIGNAL(newClientSignal(Client*)),this,SLOT(newClientSlot(Client*)));
    connect(&server,SIGNAL(removeClientSignal(Client*)),this,SLOT(removeClientSlot(Client*))
            );
    connect(&server,SIGNAL(statusClientSignal(Client*,QString,QString)),
            this,SLOT(statusClientSlot(Client*,QString,QString)));
    connect(&server,SIGNAL(changeClientMachine(Client*,QString)),this,SLOT(clientMachineSlot(Client*,QString)));
    connect(&server,SIGNAL(clientSendGenomesSignal(Client*,QStringList,QStringList)),
            this,SLOT(getGenomesSlot(Client*,QStringList,QStringList)));
    connect(&server,SIGNAL(clientRequestGenomesSignal(Client*)),this,
            SLOT(clientRequestGenomesSlot(Client*)));
    connect(this,SIGNAL(allClientsTerminated()),this,SLOT(runParserCommands()));
    ipAddress=this->getIpAddress();

    this->setFixedSize(WINDOW_WIDTH,WINDOW_HEIGHT);

    QWidget *mainWidget=new QWidget(this);
    mainWidget->setGeometry(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
    QHBoxLayout *l2=new QHBoxLayout();
    tab=new QTabWidget;
    mainWidget->setLayout(l2);
    l2->addWidget(tab);
    /*
     *  Prepare the tab
    */
    infoTab=new QWidget;
    clientTab=new QWidget;
    problemTab=new QWidget;
    messageTab=new QWidget;
    tab->addTab(infoTab,"Info");
    tab->addTab(clientTab,"Clients");
    tab->addTab(problemTab,"Problems");
    tab->addTab(messageTab,"Messages");
    makeInfoTab();
    makeClientTab();
    makeProblemTab();
    makeMessageTab();
    addMessage("Server Started");
}


void    MainWindow::makeInfoTab()
{
    QVBoxLayout *l=new QVBoxLayout();
    infoTab->setLayout(l);
    QLabel *lab1=new QLabel("<h2 align=center>Server Information</h2><table border=1 align=center><tr><td>Server IP:</td><td>"+ipAddress+"</td></tr>"+
                    "<tr><td>Server Port:</td><td>"+QString::number(server.serverPort())+"</td></tr>"+
                     "<tr><td>Type of machine:</td><td> "+QSysInfo::productType()+"</td></tr>"+
                     "<tr><td>Cpu Architecture:</td><td> "+QSysInfo::currentCpuArchitecture()+"</td></tr>"+
                     "<tr><td>Build abi: </td><td>"+QSysInfo::buildAbi()+"</td></tr></table>");

    lab1->setGeometry(0,0,400,400);
    l->addWidget(lab1);
    lab1->setAlignment(Qt::AlignTop);

    QHBoxLayout *buttonLayout=new QHBoxLayout();
    l->addLayout(buttonLayout);
    loadScript=new QPushButton;
    loadScript->setText(tr("LOAD SCRIPT"));
    buttonLayout->addWidget(loadScript);
    connect(loadScript,SIGNAL(clicked(bool)),this,SLOT(loadScriptSlot()));
}

bool    MainWindow::executeCommand(QString command)
{
    QStringList list=command.split(" ");
    int x = QString::compare(list[0], "SET", Qt::CaseInsensitive);
    if(x==0)
    {
       if(list.size()==1)
       {
           qDebug()<<"Incorrect set command"<<endl;
           return true;
       }
       if(QString::compare(list[1],"PROBLEM",Qt::CaseInsensitive)==0)
       {
            if(list.size()!=3)
            {
                qDebug()<<"Incorrect set command"<<endl;
                return true;
            }
            QString problemName=list[2];
            //check the relevant check box
            //set the active problem string
            bool foundProblem=false;
            for(int i=0;i<problem_table->rowCount();i++)
            {
                QWidget *cell0=problem_table->cellWidget(i,0);
                QHBoxLayout *playout=(QHBoxLayout *)cell0->layout();
                QCheckBox *cp=(QCheckBox*)playout->itemAt(0)->widget();
                QString pname=cp->property("problem").toString();
                if(pname==problemName)
                {
                    cp->setChecked(true);
                    foundProblem=true;
                    activeProblem=problemName;
                    break;
                }
            }
            if(!foundProblem)
            {
                qDebug()<<"Problem "<<problemName<<" not found"<<endl;
                return true;
            }
       }
       else
       if(QString::compare(list[1],"PARAMETER",Qt::CaseInsensitive)==0)
       {
           if(list.size()!=4)
           {
               qDebug()<<"Incorrect set command"<<endl;
               return true;
           }
           if(activeProblem=="")
           {
               qDebug()<<"Problem not set"<<endl;
               return true;
           }
           QString paramName=list[2];
           QString paramValue=list[3];
           QString s=db->getProblemSettings(activeProblem);
           QJsonDocument doc = QJsonDocument::fromJson(s.toUtf8());
           QJsonObject obj=doc.object();
           obj[paramName]=paramValue;
           QJsonDocument doc2(obj);
           QString strJson(doc2.toJson(QJsonDocument::Compact));
           db->setProblemSettings(activeProblem,strJson);
       }
       else
       if(QString::compare(list[1],"LOGFILE",Qt::CaseInsensitive)==0)
       {
            if(list.size()!=3)
            {
                qDebug()<<"Incorrect set command"<<endl;
                return true;
            }
       }
    }
    else
    {
        int y = QString::compare(list[0], "RUN", Qt::CaseInsensitive);
        if(y==0)
        {
            if(problemIsRunning()) return false;
            totalRunSlot();
        }
    }
    return true;
}

void    MainWindow::runParserCommands()
{
    QString command="";
    if(parser==NULL) return;
    while(true)
    {
        command=parser->nextCommand();
        if(command.size()==0) break;
        bool retValue=executeCommand(command);
        if(!retValue)
        {
            parser->undoCommand();
            break;
        }
    }
}

void    MainWindow::loadScriptSlot()
{
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to open",
                            ".",
                            "OnePC script (*.os *.txt )");
    if(files.size()!=0)
    {
        if(parser!=NULL) delete parser;
        parser=new Parser(files[0]);
        if(parser->totalCommands()==0)
        {
            QMessageBox::information(this, tr("OnePC Server"),
                       tr("The following error occurred: ")+
                       QString("Command file "+files[0]+" is probably empty"));
        }
        else
        {
            runParserCommands();
        }
    }
}

void    MainWindow::makeClientTab()
{
    /*
     *  edo prepei na mpei mia lista me ta hosts
     *  pou exoyn syndethei kathos kai pliktra
     *  me ta opoia na mporoume na elexoume auta ta hosts
    */
    QVBoxLayout *l=new QVBoxLayout();
    clientTab->setLayout(l);
    /*

    */
    client_table = new QTableWidget();
    l->addWidget(client_table);
    QStringList m_TableHeader;

    m_TableHeader<<tr("Operating System")<<tr("Client Id")<<tr("Status")<<tr("Minimum")<<""<<""<<"";
    client_table->setColumnCount(7);
    client_table->setHorizontalHeaderLabels(m_TableHeader);
    client_table->verticalHeader()->setVisible(false);
    client_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    client_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    client_table->setSelectionMode(QAbstractItemView::SingleSelection);
    client_table->setShowGrid(true);

    client_table->resizeColumnsToContents();
    client_table->resizeRowsToContents();
  //  client_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QHBoxLayout *buttonLayout=new QHBoxLayout();
    l->addLayout(buttonLayout);
    totalStatus=new QPushButton("All Status");
    buttonLayout->addWidget(totalStatus);
    totalRun=new QPushButton("Run Experiment");
    buttonLayout->addWidget(totalRun);
    connect(totalRun,SIGNAL(clicked(bool)),this,SLOT(totalRunSlot()));
    connect(totalStatus,SIGNAL(clicked()),this,SLOT(totalStatusSlot()));
}


void    MainWindow::infoProblemSLot()
{
    QObject *s=QObject::sender();
    QVariant p=s->property("problem");
    AboutProblemDialog *d=new AboutProblemDialog(p.toString(),db);
    d->exec();
}

void    MainWindow::delProblem(QString name)
{
     int ipos=-1;
    for(int i=0;i<problem_table->rowCount();i++)
    {
        QPushButton *about_button=(QPushButton *)problem_table->cellWidget(i,2);
        if(about_button->property("problem").toString()==name)
        {
            ipos=i;
            break;
        }
    }
    if(ipos!=-1)
    {
        problem_table->removeRow(ipos);
    }

}

void    MainWindow::checkProblemSlot(bool status)
{
    if(status==false) return;
    QObject *s=QObject::sender();
    QCheckBox *b=(QCheckBox *)s;
    QString pname=b->property("problem").toString();
    for(int i=0;i<problem_table->rowCount();i++)
    {
        QWidget *cell0=problem_table->cellWidget(i,0);
        QHBoxLayout *playout=(QHBoxLayout *)cell0->layout();
        QCheckBox *cp=(QCheckBox*)playout->itemAt(0)->widget();
        if(cp->property("problem").toString()!=pname && cp->isChecked())
        {
            cp->setChecked(false);
        }
    }
}

void    MainWindow::loadProblemSlot()
{
//edo prepei na emfanizei dialogo gia eisagogi tis bibliothikis.
//O xristis prepei na dialexei kai to leitourgiko systima afou epilexei
//to onoma tou arxeiou
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to open",
                            ".",
                            "Shared Libraries (*.so *.dll *.dylib *.a)");
    if(files.size()!=0)
    {
        OsDialog *d=new OsDialog();
        int dialogCode = d->exec();
        if(dialogCode == QDialog::Accepted)
        {
            QString selectedOs=d->getSelected();
            QString selectedName=d->getProblemName();
            QString settings=d->getSettings();
            if(selectedOs=="" || selectedName=="")
            {
                QMessageBox::information(this, tr("OnePC Server"),
                           tr("The following error occurred: ")+
                           QString("You should specify problem name and operating system."));
            }
            else
            {
                db->insertProblem(selectedName,selectedOs,files[0]);
                if(settings.size()!=0)
                    db->setProblemSettings(selectedName,settings);
                bool exists=false;
                int ipos=-1;
                for(int i=0;i<problem_table->rowCount();i++)
                {
                    QPushButton *about_button=(QPushButton*)problem_table->cellWidget(i,2);
                    if(about_button->property("problem").toString()==selectedName)
                    {
                        exists=true;
                        ipos=i;
                        break;
                    }
                }
                if(!exists)
                {
                    int currentRows=problem_table->rowCount();
                    problem_table->setRowCount(currentRows+1);
                    addProblem(selectedName,currentRows);

                }
                else
                {
                   QWidget *cell3=problem_table->cellWidget(ipos,3);
                   QHBoxLayout *playout=(QHBoxLayout*)cell3->layout();
                   QLabel *lab=new QLabel("");
                   lab->setPixmap(getOsIcon(selectedOs));
                   lab->setFixedSize(WINDOW_WIDTH/10,WINDOW_HEIGHT/12);
                   playout->addWidget(lab,0,Qt::AlignLeft);
                }
                problem_table->resizeColumnsToContents();
                problem_table->resizeRowsToContents();
            }
        }
    }
}

void    MainWindow::activeProblemSlot()
{
    int ipos=-1;
    QCheckBox *cp=NULL;
    for(int i=0;i<problem_table->rowCount();i++)
    {
        QWidget *cell0=problem_table->cellWidget(i,0);
        QHBoxLayout *playout=(QHBoxLayout *)cell0->layout();
        cp=(QCheckBox*)playout->itemAt(0)->widget();
        if(cp->isChecked())
        {
            ipos=i;
            break;
        }
    }
    if(ipos==-1)
    {
        QMessageBox::information(this, tr("Ga Server"),
                   tr("The following error occurred: ")+
                   QString("You should select a problem from the list"));
        return;
    }

    activeProblem=cp->property("problem").toString();

    QMessageBox::information(this, tr("Ga Server"),
               tr("Information: ")+
               QString("Current active problem set to "+activeProblem));
}

void    MainWindow::deleteProblemSlot()
{
    int ipos=-1;
    QCheckBox *cp=NULL;
    QString problem_name="";
    for(int i=0;i<problem_table->rowCount();i++)
    {
        QWidget *cell0=problem_table->cellWidget(i,0);
        QHBoxLayout *playout=(QHBoxLayout *)cell0->layout();
        cp=(QCheckBox*)playout->itemAt(0)->widget();
        if(cp->isChecked())
        {
            ipos=i;
            problem_name=cp->property("problem").toString();
            break;
        }
    }
    if(ipos==-1)
    {
        QMessageBox::information(this, tr("Ga Server"),
                   tr("The following error occurred: ")+
                   QString("You should select a problem from the list"));
        return;
    }

    QMessageBox::StandardButton resBtn = QMessageBox::question( this,
               "Ga Server",
              "Delete problem. Are you sure?",
              QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
              QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
            delProblem(problem_name);
            QStringList oslist=db->getOsList(problem_name);
            for(int i=0;i<oslist.size();i++)
                db->deleteProblem(problem_name,oslist[i]);
     }
}

QPixmap MainWindow::getOsIcon(QString name)
{
    if(name=="ubuntu"||name=="debian")
        return QPixmap(":/images/linux_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT);
    else
    if(name=="android")
        return QPixmap(":/images/android_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT);
    else
    if(name=="ios")
        return QPixmap(":/images/ios_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT);
    else
    if(name=="osx")
        return QPixmap(":/images/apple_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT);
    else
    if(name=="windows")
        return QPixmap(":/images/windows_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT);
    return QPixmap(":/images/linux_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT);
}

void    MainWindow::problemParametersSlot()
{
    QObject *s=QObject::sender();
    QVariant p=s->property("problem");
    ParameterDialog *d=new ParameterDialog(p.toString(),db);
    d->exec();
}

void    MainWindow::addProblem(QString name,int ipos)
{
    QWidget *cell0=new QWidget(this);
    QHBoxLayout *lcell0=new QHBoxLayout();
    cell0->setLayout(lcell0);;
    QCheckBox *cp=new QCheckBox(this);
    cp->setProperty("problem",name);
    lcell0->addWidget(cp,0,Qt::AlignCenter);
    problem_table->setCellWidget(ipos,0,cell0);
    connect(cp,SIGNAL(toggled(bool)),this,SLOT(checkProblemSlot(bool)));
    problem_table->setItem(ipos,1,new QTableWidgetItem(name));
    QPushButton *about_button=new QPushButton(this);
    about_button->setText(tr("About"));
    about_button->setProperty("problem",name);
    connect(about_button,SIGNAL(clicked()),this,SLOT(infoProblemSLot()));
    problem_table->setCellWidget(ipos,2,about_button);


    QPushButton *parameterButton=new QPushButton(this);
    parameterButton->setText(tr("Parameters"));
    parameterButton->setProperty("problem",name);
    problem_table->setCellWidget(ipos,3,parameterButton);
    connect(parameterButton,SIGNAL(clicked(bool)),this,SLOT(problemParametersSlot()));

    QWidget *cell3=new QWidget();
    QHBoxLayout *playout=new QHBoxLayout();
    cell3->setLayout(playout);
    QStringList osList=db->getOsList(name);
    for(int j=0;j<osList.size();j++)
    {
        QLabel *lab=new QLabel("");
        lab->setPixmap(getOsIcon(osList[j]));
        lab->setFixedSize(ICON_WIDTH,ICON_HEIGHT);
        playout->addWidget(lab,0,Qt::AlignLeft);
    }
      problem_table->setCellWidget(ipos,4,cell3);
}

void    MainWindow::makeProblemTab()
{
    QVBoxLayout *l=new QVBoxLayout();
    problemTab->setLayout(l);
    problem_table = new QTableWidget();
    l->addWidget(problem_table);
    QStringList m_TableHeader;

    m_TableHeader<<tr("Active")<<tr("Problem name")<<tr("About")<<tr("Parameters")<<tr("Operating systems");
    problem_table->setColumnCount(5);
    problem_table->setHorizontalHeaderLabels(m_TableHeader);
    problem_table->verticalHeader()->setVisible(false);
    problem_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    problem_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    problem_table->setSelectionMode(QAbstractItemView::SingleSelection);
    problem_table->setShowGrid(true);
    QStringList problems=db->getProblemList();
    problem_table->setRowCount(problems.size());
    for(int i=0;i<problems.size();i++)
    {
        addProblem(problems[i],i);
    }
    problem_table->resizeColumnsToContents();
    problem_table->resizeRowsToContents();
    problem_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QHBoxLayout *buttonLayout=new QHBoxLayout();
    l->addLayout(buttonLayout);
    loadProblemButton=new QPushButton("Load Problem");
    buttonLayout->addWidget(loadProblemButton);
    activeProblemButton=new QPushButton("Set Active Problem");
    buttonLayout->addWidget(activeProblemButton);
    deleteProblemButton=new QPushButton("Delete Problem");
    buttonLayout->addWidget(deleteProblemButton);
    connect(loadProblemButton,SIGNAL(clicked()),this,SLOT(loadProblemSlot()));
    connect(deleteProblemButton,SIGNAL(clicked()),this,SLOT(deleteProblemSlot()));
    connect(activeProblemButton,SIGNAL(clicked()),this,SLOT(activeProblemSlot()));

}

bool    MainWindow::problemIsRunning()
{
    for(int i=0;i<server.client.size();i++)
        if(server.client[i]->status==STATE_RUN) return true;
    return false;
}

void    MainWindow::clientRequestGenomesSlot(Client *C)
{
    int ipos=findClientBySocket(C->socketDescriptor);
    QJsonArray array;
    addTimeStamp(C);
    for(int i=0;i<server.client.size();i++)
    {
        server.client[i]->parallelUpdated=false;
        if(i==ipos) continue;
        if(server.client[i]->status!=STATE_RUN) continue;
         for(int j=0;j<server.client[i]->parallelFitness.size();j++)
        {
            QJsonObject obj;
            obj["xvalue"]=server.client[i]->parallelGenome[j];
            obj["yvalue"]=server.client[i]->parallelFitness[j];
            array.append(obj);
        }
    }


    QJsonObject object;
    object["array"]=array;
    object["count"]=array.size();
    object["message"]=CM_SERVERSENDGENOMES;

    sendObjectToClient(ipos,object);

}

 void    MainWindow::sendObjectToClient(int ipos,QJsonObject object)
 {
     try{
     ((GaThread *)server.client[ipos]->thread)->sendObject(object);
    }catch(int e)
     {
        qDebug()<<"Exception raised number: "<<e<<endl;
     }
 }

 int     MainWindow::findClientBySocket(int sockid)
 {
     int ipos=-1;
     for(int i=0;i<server.client.size();i++)
         if(server.client[i]->socketDescriptor==sockid)
         {
             ipos=i;
             break;
         }
     return ipos;
 }

void    MainWindow::getGenomesSlot(Client *C,QStringList x,QStringList y)
{
   int ipos=findClientBySocket(C->socketDescriptor);
   if(ipos!=-1)
   {
       server.client[ipos]->parallelGenome=x;
       server.client[ipos]->parallelFitness=y;
       server.client[ipos]->parallelUpdated=true;
   }
}

void    MainWindow::addTimeStamp(Client *C)
{
    ClientTime T;
    T.clientid=C->socketDescriptor;
    T.timestamp=QDateTime::currentMSecsSinceEpoch();
    T.generations=C->generations;
    clienttime.append(T);


}

int totalStarted=0;
void    MainWindow::statusClientSlot(Client *C, QString status,QString minimum)
{
   int ipos=findClientBySocket(C->socketDescriptor);
   if(ipos!=-1)
   {
       server.client[ipos]->status=status;
       QLabel *minimum_label=(QLabel *)client_table->cellWidget(ipos,3);
       minimum_label->setText(minimum);
       setStatusIcon(ipos);

	qDebug()<<"Total started  = "<<totalStarted<<endl;
	 if(status==STATE_FINISH) totalStarted--;
	if(totalStarted==0) emit allClientsTerminated();
       if(status==STATE_FINISH)
       {
            int countFinished=0;
           for(int i=0;i<server.client.size();i++)
               if(server.client[i]->status==STATE_FINISH) countFinished++;
	  qDebug()<<"Count finished is "<<countFinished<<endl;
           if(countFinished==server.client.size())
               emit allClientsTerminated();
       }
   }
}

void    MainWindow::addMessage(QString text)
{
    QDateTime t=QDateTime::currentDateTime();
    QString ss=t.toString();
    messageArea->setText(messageArea->document()->toPlainText()+"\n"+ss+":"+text);
}


void    MainWindow::removeClientSlot(Client *C)
{
    addMessage("Remove client :"+QString::number(C->socketDescriptor));
    int ipos=-1;
    for(int i=0;i<client_table->rowCount();i++)
    {
        QPushButton *b=(QPushButton*)client_table->cellWidget(i,6);
        int sockid=b->property("client").toString().toInt();
        if(sockid==C->socketDescriptor) {ipos=i;break;}
    }
    if(ipos!=-1)
    client_table->removeRow(ipos);
   }

void    MainWindow::totalStatusSlot()
{
    if(server.client.size()==0)
    QMessageBox::information(this, tr("Ga Server"),
                             tr("The following error occurred: ")+
                             QString("No Clients have been added"));
    QJsonObject object;
    object["message"]=CM_SERVERASKSTATUS;
    for(int i=0;i<server.client.size();i++)
    {
        ((GaThread *)server.client[i]->thread)->sendObject(object);
    }
}

void    MainWindow::statusSlot()
{
    QObject *b=QObject::sender();
    QVariant property=b->property("client");
    int sockid=property.toString().toInt();
    int ipos=findClientBySocket(sockid);
    QJsonObject object;
    object["message"]=CM_SERVERASKSTATUS;
     ((GaThread *)server.client[ipos]->thread)->sendObject(object);
  // ((GaThread *)server.client[ipos]->thread)->sendString(CM_SERVERASKSTATUS);
    QVector<qint64> value;
    QVector<int> gen;
    for(int i=0;i<clienttime.size();i++)
    {
        if(clienttime[i].clientid==server.client[ipos]->socketDescriptor)
        {
            gen.append(clienttime[i].generations);
            value.append(clienttime[i].timestamp);
        }
    }

    QVector<int> clientPos;
    QVector<qint64> clientGeneration;
    for(int i=0;i<server.client.size();i++)
    {
        if(server.client[i]->status==STATE_FINISH) continue;
        clientPos.append(i);
        clientGeneration.append(server.client[i]->generations);
    }
    for(int i=0;i<clientPos.size();i++)
    {
        for(int j=0;j<clientPos.size()-1;j++)
        {
            if(clientGeneration[j]<clientGeneration[j+1])
            {
                int t=clientGeneration[j];
                clientGeneration[j]=clientGeneration[j+1];
                clientGeneration[j+1]=t;
                t=clientPos[j];
                clientPos[j]=clientPos[j+1];
                clientPos[j+1]=t;
            }
        }
    }
    QStringList names;
    for(int i=0;i<clientPos.size();i++)
        names.append(server.client[clientPos[i]]->machineName);
    ClientChartDialog *dialog=new ClientChartDialog("Client: "+
                       server.client[ipos]->machineName,names,clientGeneration);
    dialog->exec();
}

void    MainWindow::pauseSlot()
{
    QObject *b=QObject::sender();
    QVariant property=b->property("client");
    int sockid=property.toString().toInt();
    int ipos=findClientBySocket(sockid);
    QPushButton *pause_button=(QPushButton *)b;
    QJsonObject object;
    if(pause_button->text()=="PAUSE")
    {
        object["message"]=CM_SERVERASKPAUSE;
         ((GaThread *)server.client[ipos]->thread)->sendObject(object);
       // ((GaThread *)server.client[ipos]->thread)->sendString(CM_SERVERASKPAUSE);
        pause_button->setText("RESUME");
    }
    else
    {
        object["message"]=CM_SERVERASKRESUME;
         ((GaThread *)server.client[ipos]->thread)->sendObject(object);
        //((GaThread *)server.client[ipos]->thread)->sendString(CM_SERVERASKRESUME);
        pause_button->setText("PAUSE");
    }
}

void    MainWindow::terminateSlot()
{
    QObject *b=QObject::sender();

    QVariant property=b->property("client");

    int sockid=property.toString().toInt();

    int ipos=findClientBySocket(sockid);
    QMessageBox::StandardButton resBtn = QMessageBox::question( this,
               "OnePc Server",
               "Terminate Client: "+QString::number(server.client[ipos]->socketDescriptor)+" ?",
              QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
              QMessageBox::Yes);

    if (resBtn == QMessageBox::Yes)
    {
        QJsonObject object;
        object["message"]=CM_SERVERSENDTERMINATE;
         ((GaThread *)server.client[ipos]->thread)->sendObject(object);
       // ((GaThread *)server.client[ipos]->thread)->sendString(CM_SERVERSENDTERMINATE);
        server.removeClient(server.client[ipos]->socketDescriptor);

    }
}

void    MainWindow::setStatusIcon(int pos)
{
    QString message=server.client[pos]->status;
    QLabel *status_label=(QLabel *)client_table->cellWidget(pos,2);
    if(message==STATE_START)
        status_label->setPixmap(QPixmap(":/images/state_wait.png").
                                          scaled(WINDOW_WIDTH/10,WINDOW_HEIGHT/12,Qt::KeepAspectRatio));
    else
    if(message==STATE_FINISH)
    {
        status_label->setPixmap(QPixmap(":/images/state_finish.png").
                                          scaled(WINDOW_WIDTH/10,WINDOW_HEIGHT/12,Qt::KeepAspectRatio));
        ProblemResult t;
        t.osname=server.client[pos]->typeOfMachine;
        t.bestx=server.client[pos]->bestx;
        t.besty=server.client[pos]->besty;
        t.generations=server.client[pos]->generations;
        db->insertResult(activeProblem,t);
    }
    else
    if(message==STATE_RUN)
        status_label->setPixmap(QPixmap(":/images/state_run.png").
                                          scaled(WINDOW_WIDTH/10,WINDOW_HEIGHT/12,Qt::KeepAspectRatio));
    else
    if(message==STATE_PAUSE)
        status_label->setPixmap(QPixmap(":/images/state_pause.png").
                                          scaled(WINDOW_WIDTH/10,WINDOW_HEIGHT/12,Qt::KeepAspectRatio));

}

void    MainWindow::setOsIcon(int pos)
{
    Client *C=server.client[pos];
    QLabel *machineLabel=(QLabel*)client_table->cellWidget(pos,1);
    machineLabel->setText(C->machineName);
    QLabel *osLabel=(QLabel *)client_table->cellWidget(pos,0);
    osLabel->setPixmap(getOsIcon(C->typeOfMachine));
}

void    MainWindow::newClientSlot(Client *C)
{
    int nrows=client_table->rowCount();
    client_table->setRowCount(nrows+1);

    addMessage("New client :"+QString::number(C->socketDescriptor));
    QLabel *lab1=new QLabel("");
    lab1->setFixedSize(QSize(ICON_WIDTH,5*ICON_HEIGHT/4));
    lab1->setPixmap(getOsIcon(C->typeOfMachine));
    client_table->setCellWidget(nrows,0,lab1);

    client_table->setCellWidget(nrows,1,new QLabel(C->machineName));
    QLabel *lab2=new QLabel("");
    lab2->setPixmap(QPixmap(":/images/state_wait.png").
                                          scaled(ICON_WIDTH,ICON_HEIGHT,Qt::KeepAspectRatio));
    client_table->setCellWidget(nrows,2,lab2);
    QLabel *lab3=new QLabel("0");
    lab3->setStyleSheet("color: rgb(100,100,0) ; border: 1px solid  #ff0000 ;");
    lab3->setAlignment(Qt::AlignRight|Qt::AlignBottom);
    lab3->setFixedHeight(5*ICON_HEIGHT/4);
    client_table->setCellWidget(nrows,3,lab3);
    QPushButton *status_button=new QPushButton(this);
    status_button->setText(tr("STATUS"));
    client_table->setCellWidget(nrows,4,status_button);
    QPushButton *pause_button=new QPushButton(this);
    pause_button->setText(tr("PAUSE"));
    client_table->setCellWidget(nrows,5,pause_button);
    QPushButton *terminate_button=new QPushButton(this);
    terminate_button->setText(tr("TERMINATE"));
    client_table->setCellWidget(nrows,6,terminate_button);
    client_table->resizeColumnsToContents();
    client_table->resizeRowsToContents();
    status_button->setProperty("client",QString::number(C->socketDescriptor));
    pause_button->setProperty("client",QString::number(C->socketDescriptor));
    terminate_button->setProperty("client",QString::number(C->socketDescriptor));

    connect(status_button,SIGNAL(clicked()),this,SLOT(statusSlot()));
    connect(terminate_button,SIGNAL(clicked()),this,SLOT(terminateSlot()));
    connect(pause_button,SIGNAL(clicked()),this,SLOT(pauseSlot()));

}



void    MainWindow::clientMachineSlot(Client *C, QString x)
{
    for(int i=0;i<client_table->rowCount();i++)
    {
        QPushButton *b=(QPushButton *)client_table->cellWidget(i,6);
        int sockid=b->property("client").toString().toInt();
        if(sockid==C->socketDescriptor)
        {
            setOsIcon(i);
            break;
        }
    }
}

void    MainWindow::totalRunSlot()
{
    clienttime.clear();
    if(server.client.size()==0)
    QMessageBox::information(this, tr("OnePc Server"),
                             tr("The following error occurred: ")+
                             QString("No Clients have been added"));
    else
    if(activeProblem=="")
    {
        QMessageBox::information(this, tr("OnePc Server"),
                                 tr("The following error occurred: ")+
                                 QString("You should select a problem from the problem list."));
    }
    else
        if(problemIsRunning())
        {
            QMessageBox::information(this, tr("OnePc Server"),
                                     tr("The following error occurred: ")+
              QString("Another instance is running already."));

        }
    else
    {
	totalStarted=0;
        for(int i=0;i<server.client.size();i++)
        {
            QString machine=server.client[i]->typeOfMachine;

            QByteArray data;
            if(!db->existsProblem(activeProblem,machine)) continue;
            db->getProblem(activeProblem,machine,data);
            QJsonObject object;
            object["message"]=CM_SERVERSENDPROGRAM;
            QString ss=data.toBase64();
            object["data"]=ss;
            object["parameters"]=db->getProblemSettings(activeProblem);
             ((GaThread *)server.client[i]->thread)->sendObject(object);
            server.client[i]->status=STATE_RUN;
		++totalStarted;
           // ((GaThread *)server.client[i]->thread)->sendString(CM_SERVERSENDPROGRAM);
            //((GaThread *)server.client[i]->thread)->sendBytes(data);
        }
    }
}


void    MainWindow::terminateServerSlot()
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this,
               "OnePc Server",
               tr("Quit. Are you sure ?\n"),
              QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
              QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            ;
        } else {
            for(int ipos=0;ipos<server.client.size();ipos++)
            {
                QJsonObject object;
                object["message"]=CM_SERVERSENDTERMINATE;
                 ((GaThread *)server.client[ipos]->thread)->sendObject(object);
                //((GaThread *)server.client[ipos]->thread)->sendString(CM_SERVERSENDTERMINATE);
                server.removeClient(server.client[ipos]->socketDescriptor);
            }
            qApp->exit();
        }
}

void    MainWindow::makeMessageTab()
{
    /*
     *  edo prepei na mpei kati san log area, opou tha emfanizontai minimata
     *  apo ta hosts
    */
    QHBoxLayout *l=new QHBoxLayout();
    messageTab->setLayout(l);
    messageArea=new QTextEdit("Messages:\n");
    messageArea->setReadOnly(true);
    l->addWidget(messageArea,Qt::AlignTop);
    messageArea->setAlignment(Qt::AlignTop);
}

QString MainWindow::getIpAddress()
{
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddress;
}

MainWindow::~MainWindow()
{
    delete db;
    if(parser!=NULL) delete parser;
}
