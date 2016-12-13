#include "database.h"
#include "protocol.h"
Database::Database()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("gaserver.db3");
    db.open();
    if(db.isOpen())
        qDebug()<<"Database opened"<<endl;
    else
        qDebug()<<"Database was not opened"<<endl;
    QSqlQuery q(db);

    /*
     * Bestx: είναι σε CSV format
    */
    bool res=q.exec(QString("create table if not exists problem(")+
                    QString("id integer primary key autoincrement,name varchar(1024),operatingsystem varchar(1024),")+
                    QString("data blob)"));

    if(!res)
        qDebug()<<"Create was failed"<<endl;
    else
        qDebug()<<"Create was sucessfull"<<endl;

    res=q.exec(QString("create table if not exists result("+
                       QString("name varchar(1024),operatingsystem varchar(1024),bestx text,besty double,generations integer")+
                       QString(")")));
    res=q.exec(QString("create table if not exists problemsettings(")+
               QString("name varchar(1024),settings text)"));
}

bool    Database::insertResult(QString name,ProblemResult result)
{
 QSqlQuery query(db);
 query.prepare("INSERT INTO result (name,operatingsystem,bestx,besty,generations) VALUES (?,?,?,?,?)");
 query.addBindValue(name);
 query.addBindValue(result.osname);
 query.addBindValue(result.bestx);
 query.addBindValue(result.besty);
 query.addBindValue(result.generations);
 query.exec();
 return true;
}

void    Database::setProblemSettings(QString name,QString settings)
{
 QSqlQuery query(db);
 QString s="select count(*) as n from problemsettings where name='"+name+"'";
 query.exec(s);

 query.next();
 bool ok =false;
 int n=query.value(0).toInt(&ok);
 qDebug()<<"Ok is "<<ok<<" N from select is "<<n<<" S is "<<s<<endl;
 if(n==0)
 {
     QSqlQuery query2(db);
   query2.prepare("insert into problemsettings(name,settings) values(?,?)");
   query2.addBindValue(name);
   query2.addBindValue(settings);
   bool f1=query2.exec();
   qDebug()<<"F1 is "<<f1<<endl;
   if(f1==false)
       qDebug()<<"Last error "<<query2.lastError()<<endl;
 }
 else
 {
     bool f2=query.exec("update problemsettings set settings='"+settings+"' where name='"+name+"'");
    qDebug()<<"F2 is "<<f2<<endl;
 }
}

QString Database::getProblemSettings(QString name)
{
    QSqlQuery query(db);
    query.exec("select settings from problemsettings where name='"+name+"'");
    bool f=query.next();
    if(!f) qDebug()<<"Failed query "<<query.lastError()<<endl;
    return query.value(0).toString();
}

bool    Database::insertProblem(QString name,QString operatingsystem,QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"File "<<filename<<" not ready for open"<<endl;
        return false;
    }
    QByteArray byteArray = file.readAll();
    QSqlQuery query(db);

    if(existsProblem(name,operatingsystem))
    {

        query.prepare("update problem set data=? where name='"+name+"' and operatingsystem='"+
                      operatingsystem+"'");
        query.addBindValue(byteArray);
        query.exec();
        return true;
    }

    query.prepare("INSERT INTO problem (name,operatingsystem,data) VALUES (?,?,?)");
    query.addBindValue(name);
    query.addBindValue(operatingsystem);
    query.addBindValue(byteArray);
    query.exec();
    return true;
}

bool    Database::getProblem(QString name,QString operatingsystem,QByteArray &data)
{
    QSqlQuery query(db);
    bool res=query.exec("select data from problem where name='"+name+"' and operatingsystem='"+operatingsystem+"'");
    if(!res)
    {
        qDebug()<<"Wrong query"<<endl;
        return false;
    }

    query.next();
    data=query.value(0).toByteArray();
    return true;
}

bool    Database::existsProblem(QString name,QString operatingsystem)
{
    QSqlQuery query(db);
    bool res=query.exec("select count(*) as n from problem where name='"+name+"' and operatingsystem='"+operatingsystem+"'");
    if(!res)
    {
        qDebug()<<"Wrong query"<<endl;
        return false;
    }
    query.next();
    int v=query.value(0).toInt();
    if(v==0) return false;
    return true;
}



bool    Database::deleteProblem(QString name,QString operatingsystem)
{
    if(!existsProblem(name,operatingsystem)) return false;
    QSqlQuery query(db);
    bool res=query.exec("delete from problem where name='"+name+"' and operatingsystem='"+operatingsystem+"'"
            );
    if(!res)
    {
        qDebug()<<"Wrong delete query "<<endl;
        return false;
    }
    return true;
}

QStringList Database::getProblemList()
{
 QSqlQuery query(db);
 QStringList list;
 bool res=query.exec("select name from problem group by name order by name");
 if(!res)
 {
     qDebug()<<"Wrong sql query"<<endl;
     return list;
 }
 while(query.next())
 {
     list.append(query.value(0).toString());
 }
 return list;
}

QStringList Database::getOsList(QString name)
{
    QSqlQuery query(db);
    QStringList list;
    bool res=query.exec("select operatingsystem from problem where name='"+name+
                        "' group by operatingsystem order by operatingsystem");
    if(!res)
    {
        qDebug()<<"Wrong sql query"<<endl;
        return list;
    }
    while(query.next())
    {
        list.append(query.value(0).toString());
    }
    return list;
}


QVector<ProblemResult> Database::getResult(QString name)
{
    QVector<ProblemResult> ret;
    QSqlQuery query(db);
    QString s="select generations,bestx,besty,operatingsystem from result where name='"+name+"' order by besty desc";
    if(!query.exec(s))
    {
        qDebug()<<"Wrong query "<<s<<endl;
        return ret;
    }
    while(query.next())
    {
        ProblemResult t;
        t.generations=query.value(0).toInt();
        t.bestx=query.value(1).toString();
        t.besty=query.value(2).toDouble();
        t.osname=query.value(3).toString();
        ret.append(t);
    }
    return ret;
}

ProblemResult Database::getBestResult(QString name)
{
    ProblemResult t;
      QSqlQuery query(db);
    QString s="select generations,bestx,besty,operatingsystem from result where name='"+name+"' order by besty asc";
    if(!query.exec(s))
    {
        qDebug()<<"Wrong query "<<s<<endl;
        return t;
    }
    while(query.next())
    {
        t.generations=query.value(0).toInt();
        t.bestx=query.value(1).toString();
        t.besty=query.value(2).toDouble();
        t.osname=query.value(3).toString();
        return t;
    }
    return t;
}

QJsonObject Database::result2json(ProblemResult result)
{
    QJsonObject object;
    object["generations"]=result.generations;
    object["bestx"]=result.bestx;
    object["besty"]=result.besty;
    object["operatingsystem"]=result.osname;
    return object;
}

void    Database::deleteResult(QString name,ProblemResult result)
{
  QSqlQuery query(db);
  QString s="delete from result where name='"+name+"' and generations="+QString::number(result.generations)+
          " and bestx='"+result.bestx+"' and operatingsystem='"+result.osname+"'";
  if(!query.exec(s))
  {
      qDebug()<<"Wrong query "<<s<<endl;
  }
}

Database::~Database()
{
    db.close();
    qDebug()<<"Database closed"<<endl;
}

