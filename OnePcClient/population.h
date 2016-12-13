#ifndef POPULATION_H
#define POPULATION_H
# include <problem.h>
# include <QString>
# include <QVector>
# include <QMutex>
# include <QMutexLocker>
class Population
{
private:
        int gcount;
        int generation;
        int localsearchchromosomes;
        int localsearchgenerations;
        Problem *problem;
        double selection_rate;
        double mutation_rate;
        vector<Data> genome;
        vector<Data> children;
        Data    lmargin,rmargin;
        vector<Data>    parallelGenome;
        Data            parallelFitness;
        Data         fitness;
        void     calcFitness();
        void     select();
        void     crossover();
        void     mutate();
        QString  toCSV(Data g);
        Data     fromCSV(QString s);
        Data     tournament();
        QMutex mutex;
        void    localSearch(int gpos);
public:
     Population(int count,Problem *p);
     void   restart();
     int    getCount();
     int    getSize();
     int    getGeneration();
     void   setSelectionRate(double f);
     double getSelectionRate();
     void   setMutationRate(double f);
     double getMutationRate();
     void   nextGeneration();
     double getBestFitness();
     Data   getBestGenome();
     double evaluateBestGenome();
     void   replaceWorst(double y,Data &x);
     void   getGenomes(int count,QStringList &x,QStringList &y);
     void   setGenomes(QStringList x,QStringList y);
     void   setLocalSearchChromosomes(int c);
     int    getLocalSearchChromosomes();
     void   setLocalSearchGenerations(int c);
     int    getLocalSearchGenerations();
    ~Population();
};

#endif // POPULATION_H
