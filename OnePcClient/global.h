#ifndef GLOBAL_H
#define GLOBAL_H
# include <QString>
class Global
{
public:
    Global();
    void    setMachineName(QString s);
    QString getMachineName();
    void    setIp(QString s);
    QString getIp();
    void    setPort(int p);
    int     getPort();
    void    setGenerations(int g);
    int     getGenerations();
    void    setChromosomes(int g);
    int     getChromosomes();
    void    setSelectionRate(double s);
    double  getSelectionRate();
    void    setMutationRate(double s);
    double  getMutationRate();
    void    setParallelChromosomes(int c);
    int     getParallelChromosomes();
    void    setLocalSearchChromosomes(int c);
    int     getLocalSearchChromosomes();
    void    setLocalSearchGenerations(int c);
    int     getLocalSearchGenerations();
    void setSettings(double s,double m,int c,int g);
    void getSettings(double &s,double &m,int &c,int &g);
    ~Global();
private:
    double selection_rate;
    double mutation_rate;
    int     chromosomes;
    int     generations;
    QString serverip;
    int     serverport;
    QString machineName;
    int     parallelchromosomes;
    int     localsearchchromosomes;
    int     localsearchgenerations;
};
#endif // GLOBAL_H
