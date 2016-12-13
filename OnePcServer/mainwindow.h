#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QTcpSocket>
#include <QNetworkInterface>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
# include <sys/types.h>
# include <stdio.h>
# include <sys/time.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
#include <vector>
#include <string>
# include <QString>
# include <QtNetwork/QNetworkInterface>
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextStream>
#include <QDateTime>
#include <QStringList>
#include <QDir>
#include <QSettings>
#if defined(Q_OS_WIN)
#include <WinSock2.h>
#include <WinSock.h>
#include <stdint.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#else
#include <sys/socket.h>
#include <syslog.h>
# include <netinet/in.h>
# include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QDateTime>
#include <QTableView>
#include <QTableWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QListView>
#include <QScrollArea>
#include "gaserver.h"
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QApplication>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QFileSystemWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "protocol.h"
#include "database.h"
#include "osdialog.h"
#include "aboutproblemdialog.h"
#include "parameterdialog.h"
#include "clientchartdialog.h"
# include "parser.h"

/*  27/8/2016
 *  Σε αυτήν την δομή θα βάζουμε τους πελάτες και πότε
 *  ζήτησαν χρωμοσώματα από τον server. Έτσι μπορούμε
 *  να κάνουμε μια χοντρική εκτίμηση της ταχύτητας και
 *  της απόδοσης των πελατών.
*/
struct ClientTime
{
    qintptr clientid;
    int generations;
    qint64 timestamp;
};

extern GaServer server;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void    addMessage(QString text);
    QString getIpAddress();
    void    makeInfoTab();
    void    makeClientTab();

    void    makeMessageTab();
    void    makeProblemTab();
    QPixmap getOsIcon(QString name);
private:
    QTabWidget  *tab;
    QWidget *infoTab;
    QWidget *clientTab;
    QWidget *problemTab;
    QWidget *messageTab;
    QString ipAddress;
    QTextEdit  *messageArea;
    Database *db;
    Parser *parser;

    QPushButton *loadProblemButton;
    QPushButton *activeProblemButton;
    QPushButton *deleteProblemButton;

    QPushButton *totalStatus;
    QPushButton *totalRun;
    QPushButton *loadScript;
    void    setOsIcon(int pos);
    void    setStatusIcon(int pos);

    QString activeProblem;


    QTableWidget *problem_table;
    QTableWidget *client_table;
    QVector<ClientTime> clienttime;

    void    addProblem(QString name, int ipos);
    void    delProblem(QString name);
    int     findClientBySocket(int sockid);
    bool    problemIsRunning();
    void    addTimeStamp(Client *C);
    void    sendObjectToClient(int ipos,QJsonObject object);

    bool    executeCommand(QString command);
public slots:
    void    terminateServerSlot();
    void    newClientSlot(Client *C);
    void    removeClientSlot(Client *C);
    void    statusClientSlot(Client *C,QString status,QString minimum);
    void    getGenomesSlot(Client *C,QStringList x,QStringList y);
    void    clientRequestGenomesSlot(Client *C);
    void    totalRunSlot();
    void    totalStatusSlot();
    void    statusSlot();
    void    terminateSlot();
    void    clientMachineSlot(Client *C,QString x);
    void    pauseSlot();
    void    loadProblemSlot();
    void    activeProblemSlot();
    void    deleteProblemSlot();
    void    checkProblemSlot(bool status);
    void    infoProblemSLot();
    void    problemParametersSlot();
    void    loadScriptSlot();
    void    runParserCommands();
signals:
     void    allClientsTerminated();
};

#endif // MAINWINDOW_H
