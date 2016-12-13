#ifndef MYPROBLEM_H
#define MYPROBLEM_H
# include <problem.h>
# include <QJsonArray>
# include <QJsonDocument>
# include <QJsonObject>
# include <QLibrary>
typedef int (*GETDIMENSION)();
typedef double (*FUNMIN)(double *);
typedef void   (*GRANAL)(double *,double *);
typedef void   (*MARGIN)(double *);
typedef void   (*INITFUNCTION)(QJsonObject);
typedef void   (*DONEFUNCTION)(double *);

class MyProblem :public Problem
{
private:
    double   *tempx;
    double   *tempg;
    QLibrary *library;
    QString   settings;
public:
    MyProblem(QLibrary *L,QString s);
    virtual double funmin(Data x);
    virtual void   granal(Data x, Data &g);
    void    setLibrary(QLibrary *L);
    void    callDone();
    ~MyProblem();
};
#endif // MYPROBLEM_H
