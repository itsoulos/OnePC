# include <math.h>
# include <stdio.h>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <QUrl>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
//NNTEST EXAMPLE
extern "C"
{
# define urlpath "http://www.opensofthellas.gr/aclass/"

double	**xdata;
double	*ydata;
int count;
int dimension=0;
int node_count=3;
double leftMargin=-1e+1;
double rightMargin=1e+1;
QString trainfile="wine.train";
QString testfile="wine.test";
QString type="regression";

QByteArray readNetworkFile(QString file)
{
    QEventLoop loop;
    QUrl serviceUrl = QUrl(file);
    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
    QNetworkRequest networkRequest(serviceUrl);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"text/plain");
    QNetworkReply *rep=networkManager->get(networkRequest);
    loop.exec();
    QByteArray bts = rep->readAll();
    return bts;
}

void    parseJson()
{
    //load nntest.json file if exists
    QByteArray bts=readNetworkFile(urlpath+QString("nntest.json"));
    if(bts.size()==0) return;
    QString contents(bts);
    QJsonDocument doc=QJsonDocument::fromJson(contents.toUtf8());
    QJsonObject obj=doc.object();
    trainfile=obj["trainfile"].toString();
    testfile=obj["testfile"].toString();

    node_count=obj["weights"].toString().toInt();
    type=obj["type"].toString();
}


void	init()
{  
    parseJson();
    QByteArray bts = readNetworkFile(urlpath+trainfile);
    QTextStream in(&bts);
    in>>dimension;
    in>>count;
    xdata=new double*[count];
    ydata=new double[count];
    for(int i=0;i<count;i++)
    {
     xdata[i]=new double[dimension];
     for(int j=0;j<dimension;j++) in>>xdata[i][j];
     in>>ydata[i];
     }
}

double	getValue(double *node,double *x);

void 	done(double *node)
{
    QByteArray bts = readNetworkFile(urlpath+testfile);
    QTextStream in(&bts);
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
    double sum=0.0;
    double per=0.0;
    const double eps=1e-7;
    QVector<double> myclass;

    if(type=="classification")
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

    for(int i=0;i<icount;i++)
    {
            per=getValue(node,testx[i]);
            if(type=="regression") sum+=(per-testy[i]) * (per-testy[i]);
            else
            {
                int imin=-1;
                double dmin=1e+10;
                for(int j=0;j<myclass.size();j++)
                    if(fabs(myclass[j]-per)<dmin)
                    {
                        imin=j;
                        dmin=fabs(myclass[j]-per);
                    }
                if(fabs(myclass[imin]-testy[i])>eps) sum=sum+1;

            }
    }
    if(type=="classification")
        sum=sum*100.0/icount;
    //upload statistics

    QEventLoop loop;
    QUrl serviceUrl = QUrl(urlpath+QString("upload_result.php"));
    QByteArray postData;

    QString name=QString(trainfile).split(".").at(0);
    postData.append(QString("name=")+name+QString("&"));
    postData.append(QString("type=")+type+QString("&"));
    postData.append(QString("weights=")+QString::number(node_count)+QString("&"));
    postData.append(QString("result=")+QString::number(sum));

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));

    QNetworkRequest networkRequest(serviceUrl);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    QNetworkReply *rep=networkManager->post(networkRequest,postData);
    loop.exec();
    bts = rep->readAll();
    QString str(bts);
    delete []testy;
    for(int i=0;i<icount;i++) delete[] testx[i];
    delete[] testx;

    delete[] ydata;
    for(int i=0;i<count;i++) delete[] xdata[i];
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
	 v=getTrainError(x,count,xdata,ydata);
	return v;
}

void    granal(double *x,double *g)
{
	getGradient(x,count,xdata,ydata,g);
}

}
