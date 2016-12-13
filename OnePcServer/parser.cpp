#include "parser.h"
# include <QFile>
# include <QTextStream>
/**
  5/11/2016
  Available commands
  SET LOGFILE logfile
  SET PROBLEM fname
  SET PARAMETER paramname paramvalue
  RUN
 */
Parser::Parser(QString file)
{
    currentCommand=0;
    QFile f(file);
    if (!f.exists())
    {
        printf("Unable to open command file, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        QString line="";
        while(true)
        {
            line=ts.readLine();
            command.append(line);
            if(line.size()==0) break;
        }
        f.close();
    }
}

QString Parser::nextCommand()
{
    if(currentCommand>=command.size()) return "";
    QString line=command[currentCommand];
    currentCommand++;
    return line.simplified();
}

void    Parser::undoCommand()
{
    --currentCommand;
    if(currentCommand<0) currentCommand=0;
}

int     Parser::totalCommands()
{
    return command.size();
}
