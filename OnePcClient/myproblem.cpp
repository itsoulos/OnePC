#include "myproblem.h"
#include <QDebug>
MyProblem::MyProblem(QLibrary *L,QString s)
{
    tempx=NULL;
    tempg=NULL;
    settings=s;
    setLibrary(L);
}

double  MyProblem::funmin(Data x)
{
    int i;
    for(i=0;i<dimension;i++) tempx[i]=x[i];
    FUNMIN fmin=(FUNMIN)library->resolve("funmin");
    //FOR FORTRAN
    if(fmin==NULL)
        fmin=(FUNMIN)library->resolve("funmin_");
    return fmin(tempx);

}

void    MyProblem::granal(Data x, Data &g)
{
    int i;
    for(i=0;i<dimension;i++) tempx[i]=x[i];
    GRANAL ganal=(GRANAL)library->resolve("granal");
    //FOR FORTRAN
    if(ganal==NULL)
        ganal=(GRANAL)library->resolve("granal_");
    ganal(tempx,tempg);
    for(i=0;i<dimension;i++) g[i]=tempg[i];
}

void    MyProblem::setLibrary(QLibrary *L)
{
    if(L==NULL)
        qDebug()<<"Library is null";
    library = L;
    bool f=library->load();
    if(!f)
        qDebug()<<"error "<<library->errorString();
    /** PROBLEM INITIALIZATION**/
    INITFUNCTION init=(INITFUNCTION)library->resolve("init");
    if(init==NULL)
        init=(INITFUNCTION)library->resolve("init_");
    if(init!=NULL)
    {
        qDebug()<<"Settings String is "<<settings<<endl;
        QJsonDocument doc = QJsonDocument::fromJson(settings.toUtf8());
        QJsonObject obj;
        obj=doc.object();
        init(obj);
    }

    GETDIMENSION getdim=(GETDIMENSION)library->resolve("getdimension");
    if(getdim==NULL)
        getdim=(GETDIMENSION)library->resolve("getdimension_");

    int dim=getdim();
    this->setDimension(dim);
    MARGIN lfunction=(MARGIN)library->resolve("getleftmargin");
    MARGIN rfunction=(MARGIN)library->resolve("getrightmargin");
    if(lfunction==NULL)
    {
        lfunction=(MARGIN)library->resolve("getleftmargin_");
        rfunction=(MARGIN)library->resolve("getrightmargin_");
    }
    if(tempx!=NULL) delete[] tempx;
    if(tempg!=NULL) delete[] tempg;
    tempx=new double [dim];
    tempg=new double[dim];
    lfunction(tempx);
    int i;
    for(i=0;i<dim;i++) lmargin[i]=tempx[i];
    rfunction(tempx);
    for(i=0;i<dim;i++) rmargin[i]=tempx[i];
}
 void    MyProblem::callDone()
 {
     if(library==NULL) return;
     DONEFUNCTION done=(DONEFUNCTION)library->resolve("done");
     if(done==NULL)
         done=(DONEFUNCTION)library->resolve("done_");
     if(done!=NULL)
         done(tempx);
 }

MyProblem::~MyProblem()
{
    delete[] tempx;
    delete[] tempg;
}
