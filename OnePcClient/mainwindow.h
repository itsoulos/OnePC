#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QLineEdit>
#include <QSysInfo>
//#include <sys/sysinfo.h>
#include <QDebug>
#include <QTcpSocket>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QSettings>
#include <QDateTime>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QCloseEvent>
#include <QLibrary>
#include <QTextEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDataStream>
#include <QHostInfo>
#include "plotarea.h"
#include "myproblem.h"
#include "mythread.h"
#include "population.h"
#include "global.h"
#include "clientprotocol.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ClientProtocol *p,Global *g,QWidget *parent = 0);
    ~MainWindow();
    void closeEvent (QCloseEvent *event);

    void     makeConnect();
    void     makeRun();
    void     makeMessage();
    void     addMessage(QString s);
    QString getMacAdress();
private:

    ClientProtocol *protocol;
    QLineEdit *edit_genome;
    QLineEdit *edit_generations;
    QLineEdit *edit_mrate;
    QLineEdit *edit_srate;
    QPushButton *button_settings;
    QPushButton *button_clear;

    PlotArea *plotarea;

    Global  *global;


    int screenHeight,screenWidth;
    QTabWidget  *tab;
    QLineEdit   *host;
    QLineEdit   *port;
    QLabel      *sysinfo;
    QLineEdit   *machineNameEdit;
    QPushButton *connectButton;
    QPushButton *quitButton;
    QWidget     *connectTab;
    QWidget     *runTab;
    QWidget     *messageTab;

    QTextEdit      *messageArea;
    bool connected;
public slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void    connectToServer();
    void    quitSlot();
    void    clearSettingsSlot();
    void    acceptSettingsSlot();
    void    requestGenomesSlot();
signals:
    void    sharedMemoryGet(QByteArray data);
};

extern MainWindow *defaultClient;

#endif // MAINWINDOW_H
