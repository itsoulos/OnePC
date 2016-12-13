#ifndef PARSER_H
#define PARSER_H
# include <QString>
# include <QVector>
# include <QStringList>
class Parser
{
private:
    QStringList command;
    int         currentCommand;
public:
    Parser(QString file);
    QString nextCommand();
    void    undoCommand();
    int     totalCommands();
};

#endif // PARSER_H
