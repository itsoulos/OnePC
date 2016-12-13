#include "problem.h"
#include <QDebug>
double myrand()
{
    return rand()*1.0/RAND_MAX;
}

Problem::Problem()
{
    dimension = 1;
    lmargin.resize(dimension);
    rmargin.resize(dimension);
    lmargin[0]=-1.0;
    rmargin[0]= 1.0;
}

Problem::Problem(int d)
{
    dimension = d;
    lmargin.resize(dimension);
    rmargin.resize(dimension);
}

double Problem::funmin(Data x)
{
    return 0.0;
}

void    Problem::setDimension(int dim)
{
    dimension = dim;
    lmargin.resize(dim);
    rmargin.resize(dim);
}

void   Problem::granal(Data x,Data &g)
{

}

void    Problem::setLeftMargin(Data l)
{
    lmargin=l;
}

void    Problem::setRightMargin(Data r)
{
    rmargin=r;
}

Data    Problem::getLeftMargin()
{
    return lmargin;
}

Data    Problem::getRightMargin()
{
    return rmargin;
}

int     Problem::isPointIn(Data x)
{
    for(int i=0;i<dimension;i++)
        if(x[i]<lmargin[i] || x[i]>rmargin[i])
            return 0;
    return 1;
}

Data     Problem::getSample()
{
    Data x;
    x.resize(dimension);
    for(int i=0;i<dimension;i++)
        x[i]=lmargin[i]+myrand()*(rmargin[i]-lmargin[i]);
    return x;
}

int     Problem::getDimension()
{
    return dimension;
}

void    Problem::random(Data &x, int p)
{
    x[p]=lmargin[p]+myrand()*(rmargin[p]-lmargin[p]);
}

Problem::~Problem()
{

}
