#ifndef PROBLEM_H
#define PROBLEM_H
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include <stdio.h>
# include <string>
# include <vector>
#include <QVector>
using namespace std;
extern double myrand();

typedef QVector<double> Data;

class Problem
{
protected:
    Data lmargin;
    Data rmargin;
    int dimension;

public:
    Problem();
    Problem(int d);
    virtual double funmin(Data x);
     virtual void   granal(Data x,Data &g);
    void setLeftMargin(Data l);
    void setRightMargin(Data r);
    Data getLeftMargin();
    Data getRightMargin();
    int  isPointIn(Data x);

    Data     getSample();
    int getDimension();
    void    random(Data &x,int p);
    void    setDimension(int dim);
    ~Problem();

};

#endif // PROBLEM_H
