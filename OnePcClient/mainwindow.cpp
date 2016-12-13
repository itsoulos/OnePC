#include "mainwindow.h"
#include "protocol.h"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <QStandardPaths>
MainWindow *defaultClient;

MainWindow::MainWindow(ClientProtocol *p,Global *g,QWidget *parent) :
    QMainWindow(parent)
{
    protocol=p;
    connect(protocol,SIGNAL(socketErrorSignal(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));
    QWidget *mainWidget;
    global=g;
    connected=false;
    plotarea=NULL;


    QDesktopWidget desktop;
    if(QSysInfo::productType()=="ubuntu" || QSysInfo::productType()=="debian"
                || QSysInfo::productType()=="windows" || QSysInfo::productType()=="osx")
    {
    screenHeight=desktop.geometry().height()/2;
    screenWidth=desktop.geometry().width()/2;
    }
    else
    {
    screenHeight=desktop.geometry().height();
    screenWidth=desktop.geometry().width();
    }

    this->setWindowTitle("OnePc Client");
    mainWidget=new QWidget(this);
    mainWidget->setGeometry(0,0,screenWidth,screenHeight);
    this->setFixedSize(screenWidth,screenHeight);



    QHBoxLayout *l2=new QHBoxLayout();
    tab=new QTabWidget;
    mainWidget->setLayout(l2);
    l2->addWidget(tab);

    connectTab=new QWidget;
    runTab=new QWidget;
    messageTab=new QWidget;
    tab->addTab(connectTab,"Connection");
    tab->addTab(runTab,"Run");
    tab->addTab(messageTab,"Messages");
    makeConnect();
    makeRun();
    makeMessage();

    addMessage("Application Started");

    defaultClient=this;
}


QString MainWindow::getMacAdress()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if ( netInterface.hardwareAddress().size()!=0 &&
              !(netInterface.flags() & QNetworkInterface::IsLoopBack))
            return netInterface.hardwareAddress();
    }
    return QString();
}


void    MainWindow::quitSlot()
{
    if(connected)
    {
            QMessageBox::information(this,"OnePC Client","You should disconnect first");
     }
     else
        qApp->quit();

}

void    MainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this,
               "OnePC Client",
               tr("Quit. Are you sure ?\n"),
              QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
              QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            if(connected)
            {
                QString message=CM_CLIENTDELETE;
                QJsonObject object;
                object["message"]=message;
                object["machine"]=QSysInfo::productType();
                protocol->sendObject(object);
            }
            event->accept();
        }
}

void     MainWindow::makeConnect()
{
    QVBoxLayout *l=new QVBoxLayout();
    connectTab->setLayout(l);

    QHBoxLayout *l0=new QHBoxLayout();
    l->addLayout(l0);
    sysinfo=new QLabel();
    l0->addWidget(sysinfo,Qt::AlignCenter);
    sysinfo->setWordWrap(true);    QString location=QStandardPaths::writableLocation(QStandardPaths::HomeLocation);


    sysinfo->setText("<table border=1><tr><td>Type of machine:</td><td> "+QSysInfo::productType()+"</td></tr>"+
                     "<tr><td>Cpu Architecture:</td><td> "+QSysInfo::currentCpuArchitecture()+"</td></tr>"+
                     "<tr><td>MAC Address: </td><td>"+getMacAdress()+"</td></tr></table>");


    QHBoxLayout *ll0=new QHBoxLayout();
    l->addLayout(ll0);
    QLabel *lab0=new QLabel(tr("Machine name: "));
    ll0->addWidget(lab0);
    machineNameEdit=new QLineEdit();
    machineNameEdit->setText(QHostInfo::localHostName());
    if(global->getMachineName().size()!=0)
        machineNameEdit->setText(global->getMachineName());
    ll0->addWidget(machineNameEdit);

    QHBoxLayout *l1=new QHBoxLayout();
    l->addLayout(l1);
    QLabel *lab1=new QLabel("Host: ");
    l1->addWidget(lab1);
    host=new QLineEdit();
    l1->addWidget(host);
    if(global->getIp()!="")
        host->setText(global->getIp());

    QHBoxLayout *l2=new QHBoxLayout();
    l->addLayout(l2);
    QLabel *lab2=new QLabel("Port: ");
    l2->addWidget(lab2);
    port=new QLineEdit();
    l2->addWidget(port);
    if(global->getPort()!=0)
        port->setText(QString::number(global->getPort()));

    QHBoxLayout *l3=new QHBoxLayout();
    l->addLayout(l3);
    QLabel *lab3=new QLabel("Press to connect ");
    l3->addWidget(lab3);
    connectButton=new QPushButton("Connect");
    l3->addWidget(connectButton);
    connect(connectButton,SIGNAL(clicked()),this,SLOT(connectToServer()));


    QHBoxLayout *l4=new QHBoxLayout();
    l->addLayout(l4);
    QLabel *lab4=new QLabel("Press to Quit ");
    l4->addWidget(lab4);
    quitButton=new QPushButton("Quit");
    l4->addWidget(quitButton);
    connect(quitButton,SIGNAL(clicked()),this,SLOT(quitSlot()));
}

/*
 *  Αυτή η μέθοδος καλείται προκειμένου να δημιουργήσει την καρτέλα RUN
*/
void     MainWindow::makeRun()
{
    int genome_count,generations;
    double mutation_rate,selection_rate;
    global->getSettings(selection_rate,mutation_rate,genome_count,generations);

    QVBoxLayout *l=new QVBoxLayout();
    runTab->setLayout(l);

    QHBoxLayout *l1=new QHBoxLayout();
    l->addLayout(l1);
    QLabel *t1=new QLabel("Chromosome count:");
    t1->setFixedWidth(2*screenWidth/5);
    l1->addWidget(t1);
    edit_genome=new QLineEdit(QString::number(genome_count));
    l1->addWidget(edit_genome);

    QHBoxLayout *l2=new QHBoxLayout();
    l->addLayout(l2);
    QLabel *t2=new QLabel("Generations:");
    t2->setFixedWidth(2*screenWidth/5);
    l2->addWidget(t2);
    edit_generations=new QLineEdit(QString::number(generations));
    l2->addWidget(edit_generations);


    QHBoxLayout *l3=new QHBoxLayout();
    l->addLayout(l3);
    QLabel *t3=new QLabel("Selection Rate:");
    t3->setFixedWidth(2*screenWidth/5);
    l3->addWidget(t3);
    edit_srate=new QLineEdit(QString::number(selection_rate));
    l3->addWidget(edit_srate);


    QHBoxLayout *l4=new QHBoxLayout();
    l->addLayout(l4);
    QLabel *t4=new QLabel("Mutation Rate:");
    t4->setFixedWidth(2*screenWidth/5);
    l4->addWidget(t4);
    edit_mrate=new QLineEdit(QString::number(mutation_rate));
    l4->addWidget(edit_mrate);

    QHBoxLayout *l5=new QHBoxLayout();
    l->addLayout(l5);
    button_clear=new QPushButton();
    button_clear->setText("Clear Settings");
    l5->addWidget(button_clear);
    button_settings=new QPushButton();
    button_settings->setText("Set Settings");
    l5->addWidget(button_settings);

    QHBoxLayout *l6=new QHBoxLayout();
    l->addLayout(l6);
    plotarea=new PlotArea(screenWidth,screenHeight/5);
    protocol->setPlotArea(plotarea);
    l6->addWidget(plotarea);

    connect(button_clear,SIGNAL(clicked()),this,SLOT(clearSettingsSlot()));
    connect(button_settings,SIGNAL(clicked()),this,SLOT(acceptSettingsSlot()));
}
void    MainWindow::clearSettingsSlot()
{
    int genome_count,generations;
    double mutation_rate,selection_rate;
    global->getSettings(selection_rate,mutation_rate,genome_count,generations);
    edit_genome->setText(QString::number(genome_count));
    edit_generations->setText(QString::number(generations));
    edit_srate->setText(QString::number(selection_rate));
    edit_mrate->setText(QString::number(mutation_rate));
}

void    MainWindow::acceptSettingsSlot()
{
    global->setSettings(edit_srate->text().toDouble(),
                        edit_mrate->text().toDouble(),
                        edit_genome->text().toInt(),
                        edit_generations->text().toInt()
                        );
}

/*
 * Αυτή η μέθοδος καλείται προκειμένου να προσθέσει ακόμα
 * ένα μήνυμα στην καρτέλα των μηνυμάτων.
*/
void     MainWindow::addMessage(QString s)
{
  QDateTime t=QDateTime::currentDateTime();
  QString ss=t.toString();
  messageArea->setText(messageArea->document()->toPlainText()+"\n"+ss+":"+s);
}

void     MainWindow::makeMessage()
{
    QHBoxLayout *l=new QHBoxLayout();
    messageTab->setLayout(l);
    messageArea=new QTextEdit("Messages:\n");
    messageArea->setReadOnly(true);
    l->addWidget(messageArea,Qt::AlignTop);
    messageArea->setAlignment(Qt::AlignTop);
}


void    MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    /** In case the server does not exists or returns error we set the connect button
     * again to Connect **/
    connected=false;
    connectButton->setText("Connect");

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("OnePc Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("OnePc Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("OnePc Client"),
                                 tr("The following error occurred: %1.")
                                 .arg("Some other error"));
                                 //.arg(tcpSocket->errorString()));
    }
}

void    MainWindow::connectToServer()
{
    if(!connected)
    {
        global->setIp(host->text());
        global->setPort(port->text().toInt());
        global->setMachineName(machineNameEdit->text());
        protocol->connectToServer();
        connected=true;
        connectButton->setText("Disconnect");
    }
    else
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this,
                   "OnePc Client",
                   tr("Disconnect from server?\n"),
                  QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                  QMessageBox::Yes);
            if (resBtn != QMessageBox::Yes) {
                ;
            } else {
                if(connected)
                {
                    QString message=CM_CLIENTDELETE;
                    QJsonObject object;
                    object["message"]=message;
                    object["machine"]=QSysInfo::productType();
                    protocol->sendObject(object);
                    connected=false;
                    connectButton->setText("Connect");
                }
            }
    }
}

void    MainWindow::requestGenomesSlot()
{
    QString message=CM_CLIENTASKGENOMES;
    QJsonObject object;
    object["message"]=message;

    protocol->sendObject(object);
}

MainWindow::~MainWindow()
{
    delete global;
    delete protocol;
    delete plotarea;
}
