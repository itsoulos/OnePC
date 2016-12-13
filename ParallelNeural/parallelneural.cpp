# include <math.h>
# include <stdio.h>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <QUrl>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QBuffer>
#include <vector>
using namespace std;

//NNTEST EXAMPLE
extern "C"
{
double	**xdata;
double	*ydata;
int dcount;
int dimension=0;

//parameters
//1. weights the number of weights
//2. leftmargin the left marging for the weights
//3. rightmargin the right marging for the weights
//4. trainfile the input file with the train data
//5. testfile  the input file with the test data
//6. outputfile the output file of the training
//7. tolminiters the number of iterations for tolmin
int node_count=3;
double leftMargin=-1e+1;
double rightMargin=1e+1;
int    tolminiters=2001;
QString trainfile="wine.train";
QString testfile="wine.test";
QString outputfile="neural.txt";

void    init(QJsonObject obj)
{
    if(obj.contains("trainfile")) trainfile=obj["trainfile"].toString();
    if(obj.contains("testfile"))  testfile=obj["testfile"].toString();
    if(obj.contains("leftmargin")) leftMargin=obj["leftmargin"].toString().toDouble();
    if(obj.contains("rightmargin")) rightMargin=obj["rightmargin"].toString().toDouble();
    if(obj.contains("weights"))     node_count=obj["weights"].toString().toInt();
    if(obj.contains("outputfile"))  outputfile=obj["outputfile"].toString();
    if(obj.contains("tolminiters")) tolminiters=obj["tolminiters"].toString().toInt();

    QFile data(trainfile);
    data.open(QFile::ReadOnly);
    QTextStream in(&data);
    in>>dimension;
    in>>dcount;
    xdata=new double*[dcount];
    ydata=new double[dcount];
    for(int i=0;i<dcount;i++)
    {
     xdata[i]=new double[dimension];
     for(int j=0;j<dimension;j++) in>>xdata[i][j];
     in>>ydata[i];
     }
    data.close();
}


double	getValue(double *node,double *x);

void 	done(double *node)
{
extern double tolmin(double *,int);
    double fmin=tolmin(node,tolminiters);
    QFile data(testfile);
    data.open(QFile::ReadOnly);
    QTextStream in(&data);
       int icount;
    in>>dimension;
    in>>icount;
    double **testx;
    double *testy;

    testx=new double*[icount];
    testy=new double[icount];
    for(int i=0;i<icount;i++)
    {
     testx[i]=new double[dimension];
     for(int j=0;j<dimension;j++) in>>testx[i][j];
     in>>testy[i];
     }
    data.close();

    double per=0.0;
    const double eps=1e-7;
    QVector<double> myclass;


    for(int i=0;i<icount;i++)
    {
        bool found=false;
        for(int j=0;j<myclass.size();j++)
            if(fabs(myclass[j]-testy[i])<eps)
            {
                found=true;
                break;
            }
        if(!found)
            myclass.append(testy[i]);
    }


    double sumTestError=0.0;
    double sumClassError=0.0;

    for(int i=0;i<icount;i++)
    {
            per=getValue(node,testx[i]);
            sumTestError+=(per-testy[i]) * (per-testy[i]);
                int imin=-1;
                double dmin=1e+10;
                for(int j=0;j<myclass.size();j++)
                    if(fabs(myclass[j]-per)<dmin)
                    {
                        imin=j;
                        dmin=fabs(myclass[j]-per);
                    }
                if(fabs(myclass[imin]-testy[i])>eps) sumClassError=sumClassError+1.0;
   }

        sumClassError=sumClassError*100.0/icount;
        QFile dataOutput(outputfile);
        dataOutput.open(QFile::Append);
        QTextStream out(&dataOutput);
        out<<testfile<<"\t"<<node_count<<"\t"<<fmin<<"\t"<<sumTestError<<"\t"<<sumClassError<<endl;
        dataOutput.close();

    delete []testy;
    for(int i=0;i<icount;i++) delete[] testx[i];
    delete[] testx;

    delete[] ydata;
    for(int i=0;i<dcount;i++) delete[] xdata[i];
    delete[] xdata;

}

double	sig(double x)
{
    return 1.0/(1.0+exp(-x));
}

double	getValue(double *node,double *x)
{
    double arg=0.0;
    double per=0.0;
        int nodes=node_count;
    for(int i=1;i<=nodes;i++)
    {
        arg=0.0;
                for(int j=1;j<=dimension;j++)
        {
                        int pos=(dimension+2)*i-(dimension+1)+j-1;
            arg+=node[pos]*x[j-1];
        }
                arg+=node[(dimension+2)*i-1];
                per+=node[(dimension+2)*i-(dimension+1)-1]*sig(arg);
    }
    return per;
}

void	getDeriv(double *node,double *x,double *G)
{
    double arg;
        int nodes=node_count;
        double f,f2;
        for(int i=1;i<=nodes;i++)
        {
                arg = 0.0;
                for(int j=1;j<=dimension;j++)
                {
                        arg+=node[(dimension+2)*i-(dimension+1)+j-1]*x[j-1];
                }
                arg+=node[(dimension+2)*i-1];
                f=sig(arg);
                f2=f*(1.0-f);
                G[(dimension+2)*i-1]=node[(dimension+2)*i-(dimension+1)-1]*f2;
                G[(dimension+2)*i-(dimension+1)-1]=f;
                for(int k=1;k<=dimension;k++)
                {
                        G[(dimension+2)*i-(dimension+1)+k-1]=
                                x[k-1]*f2*node[(dimension+2)*i-(dimension+1)-1];
                }
    }
}

double	getTrainError(double *node,int count,double **xdata,double *ydata)
{
    double sum=0.0;
    double per=0.0;
    for(int i=0;i<count;i++)
    {
        per=getValue(node,xdata[i])-ydata[i];
        sum+=per * per;
    }
    return sum;
}

void	getGradient(double *node,int count,
        double **xdata,double *ydata,double *g)
{
        for(int i=0;i<(dimension+2)*node_count;i++) g[i]=0.0;
        double gtemp[(dimension+2)*node_count];
    for(int i=0;i<count;i++)
    {
        getDeriv(node,xdata[i],gtemp);
        double per=getValue(node,xdata[i])-ydata[i];
                for(int j=0;j<(dimension+2)*node_count;j++)
            g[j]+=gtemp[j]*per;
    }
        for(int j=0;j<(dimension+2)*node_count;j++) g[j]*=2.0;
}

int	getdimension()
{
        return (dimension+2) * node_count;
}

int	hasmargins()
{
    return 1;
}

void	getleftmargin(double *x)
{
        for(int i=0;i<(dimension+2)*node_count;i++)
                x[i]=leftMargin;
}

void	getrightmargin(double *x)
{
        for(int i=0;i<(dimension+2)*node_count;i++)
                x[i]=rightMargin;
}

double	funmin(double *x)
{
    double v;
     v=getTrainError(x,dcount,xdata,ydata);

     return v;
}

void    granal(double *x,double *g)
{
    getGradient(x,dcount,xdata,ydata,g);
}

}
