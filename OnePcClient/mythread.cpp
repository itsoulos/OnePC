#include "mythread.h"
#include <QDebug>
MyThread::MyThread(Population *pop, PlotArea *plot,int g,int ngenomes)
{
    population = pop;
    plotarea = plot;
    generations = g;
    numberOfGenomes=ngenomes;
    this->setTerminationEnabled(true);
    stopflag=0;
    if(plotarea!=NULL) plotarea->clearPoints();
}

QString MyThread::vector2csv(QVector<double>&x)
{
    QString s="";
    for(int i=0;i<x.size();i++)
    {
       s=s+QString::number(x[i]);
       if(i!=x.size()-1) s=s+",";
    }
    return s;
}

void    MyThread::disableParallelData()
{
    QMutex mutex;
    mutex.lock();
    parallelDataReady=false;
    mutex.unlock();
}

void    MyThread::enableParallelData()
{
    QMutex mutex;
    mutex.lock();
    parallelDataReady=true;
    mutex.unlock();
}

bool    MyThread::isParalleDataReady()
{
    return parallelDataReady;
}

void MyThread::run()
{
    double oldMinimum=1e+100;
    Data bestGenome;
    QStringList xvalue;
    QStringList yvalue;

    if(population->getGeneration()<=1)
    {
        x1=0.0;
        x2=0.0;
        stopat=0.0;
    }
    bool firstTime=true;
    for(int i=population->getGeneration();i<=generations;i++)
    {
        qDebug()<<"Client pass1"<<endl;
        if(!firstTime)
        {
        disableParallelData();
        emit requestGenomes();
        while(!parallelDataReady)
        {
          QThread::usleep(1);

        }

        }
         qDebug()<<"Client pass2"<<endl;
        if(stopflag) break;
 qDebug()<<"Client pass3"<<endl;
        population->nextGeneration();
        firstTime=false;
         qDebug()<<"Client pass4"<<endl;
        //collect N best chromosomes and
        //send them to the server
        //of course emit the signal
        population->getGenomes(numberOfGenomes,xvalue,yvalue);
        emit sendGenomes(xvalue,yvalue);
 qDebug()<<"Client pass5"<<endl;
        double bestf=population->getBestFitness();
        double v=1.0+fabs(bestf);
        x1+=v;
        x2+=v*v;
        double variance=x2/(i+1)-(x1/(i+1))*(x1/(i+1));
        if(variance<stopat && i>10) break;
         qDebug()<<"Client pass6"<<endl;
        if(plotarea!=NULL)
            plotarea->addPoint(population->getGeneration()*1.0,-bestf);
  qDebug()<<"Client pass7"<<endl;
        if(bestf<oldMinimum)
        {
            stopat=variance/2.0;
            oldMinimum=bestf;
            bestGenome=population->getBestGenome();
            QString s=vector2csv(bestGenome);
            emit newMinimum(s,oldMinimum,i);
        }
         qDebug()<<"Client pass8"<<endl;

    }
    population->evaluateBestGenome();
    emit completed();
}
