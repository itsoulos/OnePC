#include "global.h"

Global::Global()
{
    selection_rate=0.10;
    mutation_rate=0.05;
    chromosomes=200;
    generations=200;
    parallelchromosomes=10;
    localsearchchromosomes=20;
    localsearchgenerations=50;
    serverip="";
    serverport=0;
    machineName="";
}

void    Global::setMachineName(QString s)
{
    machineName=s;
}

QString Global::getMachineName()
{
    return machineName;
}

void    Global::setSettings(double s,double m,int c,int g)
{
    selection_rate=s;
    mutation_rate=m;
    chromosomes=c;
    generations=g;
}

void    Global::getSettings(double &s,double &m,int &c,int &g)
{
    s=selection_rate;
    m=mutation_rate;
    c=chromosomes;
    g=generations;
}
void    Global::setIp(QString s)
{
    serverip=s;
}

QString Global::getIp()
{
    return serverip;
}

void    Global::setPort(int p)
{
    serverport=p;
}

int    Global::getPort()
{
    return serverport;
}
void    Global::setParallelChromosomes(int c)
{
    parallelchromosomes=c;
}

int     Global::getParallelChromosomes()
{
    return parallelchromosomes;
}

void    Global::setGenerations(int g)
{
    generations=g;
}

int     Global::getGenerations()
{
    return generations;
}

void    Global::setChromosomes(int g)
{
    chromosomes=g;
}

int     Global::getChromosomes()
{
    return chromosomes;
}

void    Global::setSelectionRate(double s)
{
    selection_rate=s;
}

double  Global::getSelectionRate()
{
    return selection_rate;
}

void    Global::setMutationRate(double s)
{
    mutation_rate=s;
}

double  Global::getMutationRate()
{
    return mutation_rate;
}

void    Global::setLocalSearchChromosomes(int c)
{
    localsearchchromosomes=c;
}

int     Global::getLocalSearchChromosomes()
{
    return localsearchchromosomes;
}

void    Global::setLocalSearchGenerations(int c)
{
    localsearchgenerations=c;
}

int     Global::getLocalSearchGenerations()
{
    return localsearchgenerations;
}

Global::~Global()
{

}
