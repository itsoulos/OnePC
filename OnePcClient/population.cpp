#include "population.h"
# include <QtGui>
Population::Population(int count,Problem *p)
{
    gcount = count;
    problem = p;
    genome.resize(count);
    fitness.resize(count);
    for(int i=0;i<count;i++)
    {
        genome[i].resize(problem->getDimension());
        genome[i]=problem->getSample();
        fitness[i]=problem->funmin(genome[i]);
    }
    generation =0;
    mutation_rate=0.05;
    selection_rate=0.90;
    children.resize(0);
    lmargin=p->getLeftMargin();
    rmargin=p->getRightMargin();
    localsearchgenerations=50;
    localsearchchromosomes=20;
}

void   Population::setLocalSearchChromosomes(int c)
{
    localsearchchromosomes=c;
}

int    Population::getLocalSearchChromosomes()
{
    return localsearchchromosomes;
}

void   Population::setLocalSearchGenerations(int c)
{
    localsearchgenerations=c;
}

int    Population::getLocalSearchGenerations()
{
    return localsearchgenerations;
}

void    Population::replaceWorst(double y,Data &x)
{
    genome[genome.size()-1]=x;
    fitness[genome.size()-1]=y;
}
void    Population::localSearch(int pos)
{
        Data  g;
        int genome_size=getSize();
        g.resize(genome_size);
        int genome_count=gcount;
        for(int iters=1;iters<=100;iters++)
        {
                int randgenome=rand() % genome_count;
                int randpos=rand() % genome_size;
                for(int i=0;i<randpos;i++) g[i]=genome[pos][i];
                for(int i=randpos;i<genome_size;i++) g[i]=genome[randgenome][i];
                double f=problem->funmin(g);
                if(fabs(f)<fabs(fitness[pos]))
                {
                        for(int i=0;i<genome_size;i++) genome[pos][i]=g[i];
                        fitness[pos]=f;
                }
                else
                {
                        for(int i=0;i<randpos;i++) g[i]=genome[randgenome][i];
                        for(int i=randpos;i<genome_size;i++) g[i]=genome[pos][i];
                        f=problem->funmin(g);
                        if(fabs(f)<fabs(fitness[pos]))
                        {
                                for(int i=0;i<genome_size;i++) genome[pos][i]=g[i];
                                fitness[pos]=f;
                        }
                }
        }

}

void    Population::restart()
{
    generation=0;
    for(int i=0;i<gcount;i++)
    {
        genome[i]=problem->getSample();
        fitness[i]=problem->funmin(genome[i]);
    }
}

void     Population::calcFitness()
{
    for(int i=0;i<gcount;i++)
        fitness[i]=problem->funmin(genome[i]);
}

void     Population::select()
{
    for(int i=0;i<gcount;i++)
    {
        for(int j=0;j<gcount-1;j++)
        {
            if(fitness[j+1]<fitness[j])
            {
                Data x=genome[j];
                genome[j]=genome[j+1];
                genome[j+1]=x;
                double f=fitness[j];
                fitness[j]=fitness[j+1];
                fitness[j+1]=f;
            }
        }
    }
}

Data     Population::tournament()
{
    const int tournament_size=8;
    int totalSize=gcount+parallelGenome.size();
    Data result;
    double fresult;
    result.resize(getSize());
    for(int i=0;i<tournament_size;i++)
    {
          int  randNumber;
         do{
          randNumber=abs(rand()) % totalSize;
          }while(randNumber>=gcount && generation<2);
          Data x;
          double y;
        x=randNumber>=gcount?parallelGenome[totalSize-randNumber-1]:
                genome[randNumber];
        y=randNumber>=gcount?parallelFitness[totalSize-randNumber-1]:
                fitness[randNumber];
        if(i==0)
        {
            result=x;
            fresult=y;
        }
        if(fabs(y)<fresult)
        {
            result=x;
            fresult=y;
        }
    }
    return result;

}

void     Population::crossover()
{
    int childsize=(int)((1.0-selection_rate)*gcount);
    if(childsize%2==1) childsize++;
    if(children.size()!=childsize)
    {
        children.resize(childsize);
        for(int i=0;i<childsize;i++)
            children[i].resize(problem->getDimension());
    }
    int childcount=0;
    int parent[2];
    Data parent0,parent1;
    while(true)
    {
        /*int tournament_size=8;
        for(int i=0;i<2;i++)
        {
            double max_fitness=1e+10;
            int    max_index=-1;
            for(int j=0;j<tournament_size;j++)
            {
                int r=rand() % gcount;
                if(j==0 || fitness[r]<max_fitness)
                {
                    max_index=r;
                    max_fitness=fitness[r];
                    parent[i]=r;
                }
            }
        }
        */
        parent0=tournament();
        parent1=tournament();
        for(int i=0;i<genome[0].size();i++)
        {
          double x1=parent0[i];
            double x2=parent1[i];
            double alfa=-0.5 +2.0 *myrand();
            double g1=alfa * x1+(1.0-alfa)*x2;
            double g2=alfa * x2 +(1.0-alfa)*x1;
            children[childcount][i]=g1;
            children[childcount+1][i]=g2;

            if(g1<lmargin[i] || g1>rmargin[i])
                children[childcount][i]=x1;
            if(g2<lmargin[i] || g2>rmargin[i])
                children[childcount+1][i]=x2;
        }
        childcount+=2;
        if(childcount>=childsize) break;
    }

    for(int i=0;i<childsize;i++)
        genome[gcount-i-1]=children[i];
}

void    Population::mutate()
{
    for(int i=1;i<gcount;i++)
    {
        for(int j=0;j<problem->getDimension();j++)
        {
            double r=myrand();
            if(r<=mutation_rate)
                problem->random(genome[i],j);
        }
    }
}

int    Population::getCount()
{
    return gcount;
}

int    Population::getSize()
{
    return problem->getDimension();
}

int    Population::getGeneration()
{
    return generation;
}

void   Population::setSelectionRate(double f)
{
    if(f>=0.0 && f<=1.0)
        selection_rate=f;
}

double Population::getSelectionRate()
{
    return selection_rate;
}

void   Population::setMutationRate(double f)
{
    if(f>=0 && f<=1.0)
        mutation_rate=f;
}

double Population::getMutationRate()
{
    return mutation_rate;
}

void   Population::nextGeneration()
{

    if(generation) mutate();
    calcFitness();
    if(generation && generation%localsearchgenerations==0)
      for(int i=0;i<localsearchchromosomes;i++)
          localSearch(rand() % gcount);
    select();
    crossover();
    ++generation;
}

double Population::getBestFitness()
{
    return fitness[0];
}

Data   Population::getBestGenome()
{
    return genome[0];
}

double  Population::evaluateBestGenome()
{
    fitness[0]=problem->funmin(genome[0]);
    return fitness[0];
}
QString Population::toCSV(Data g)
{
    QString s="";
    for(int i=0;i<g.size();i++)
        if(i!=g.size()-1)
            s=s+QString::number(g[i])+",";
    else
            s=s+QString::number(g[i]);
    return s;
}
Data    Population::fromCSV(QString s)
{
    Data x;
    QStringList ss=s.split(",");
    for(int i=0;i<ss.size();i++)
        x.push_back(ss.at(i).toDouble());
    return x;
}

void   Population::getGenomes(int count,QStringList &x,QStringList &y)
{
    x.clear();
    y.clear();
    //for now we send random chromosomes and not the best ones,
    //in order to avoid stagnation.
    for(int i=0;i<count;i++)
    {
        int randpos=rand() % genome.size();
        x.append(toCSV(genome[randpos]));
        y.append(QString::number(fitness[randpos]));
    }
}
void    Population::setGenomes(QStringList x,QStringList y)
{
    parallelFitness.resize(0);
    parallelGenome.resize(0);
    Data bestx;
    double besty=1e+100;
    for(int i=0;i<y.size();i++)
    {
        Data xx=fromCSV(x[i]);

        if(xx.size()==0) continue;
        parallelGenome.push_back(xx);
        parallelFitness.push_back(y[i].toDouble());
        double yy=y[i].toDouble();
        if(fabs(yy)<besty)
        {
            bestx=xx;
            besty=fabs(yy);
        }
    }
    if(besty<1e+100)
    replaceWorst(besty,bestx);
}

Population::~Population()
{

}
