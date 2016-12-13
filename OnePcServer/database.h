#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlError>
struct ProblemResult
{
    QString osname;
    QString bestx;
    double  besty;
    int generations;
};

class Database
{
private:
    QSqlDatabase db;
public:
    Database();
    bool    insertProblem(QString name,QString operatingsystem,QString filename);    
    bool    existsProblem(QString name,QString operatingsystem);
    bool    deleteProblem(QString name,QString operatingsystem);
    bool    getProblem(QString name,QString operatingsystem,QByteArray &data);
    bool    insertResult(QString name,ProblemResult result);
    void    deleteResult(QString name,ProblemResult result);
    QVector<ProblemResult>  getResult(QString name);
    ProblemResult           getBestResult(QString name);
    QStringList             getProblemList();
    QStringList             getOsList(QString name);
    QJsonObject result2json(ProblemResult result);
    void    setProblemSettings(QString name,QString settings);
    QString getProblemSettings(QString name);
    ~Database();
};

#endif // DATABASE_H
