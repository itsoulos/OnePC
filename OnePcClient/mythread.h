#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
# include <QMutex>
# include <QMutexLocker>
#include  "population.h"
#include "plotarea.h"

class MyThread :public QThread
{
    Q_OBJECT
private:
    int        generations;
    Population *population;
    PlotArea   *plotarea;
    int         numberOfGenomes;
    QString     vector2csv(QVector<double>&x);
    //Για το κριτήριο τερματισμού
    double x1,x2,stopat;
    bool     parallelDataReady;
 public:
    int stopflag;
    MyThread(Population *pop,PlotArea *plot,int g,int ngenomes);
    void    disableParallelData();
    void    enableParallelData();
    bool    isParalleDataReady();
    void run();
signals:
    void    newMinimum(QString x,double v,int generation);
    void    sendGenomes(QStringList x,QStringList y);
    void    requestGenomes();
    void    completed();

};

#endif // MYTHREAD_H
