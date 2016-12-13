# include <math.h>
# include <stdio.h>
# include <unistd.h>
# include <QJsonObject>
# include <QFile>
extern "C"{
double	potentialepsilon=1.0;
double	potentialsigma=1.0;
int	natoms=10;
int	tolminiters=2001;
QString	outputfile="potential.txt";

static double dmax(double a,double b){return a>b?a:b;}
double	*xx=NULL;
double  *yy=NULL;
double 	*zz=NULL;
void    init(QJsonObject obj)
{
    if(obj.contains("natoms")) natoms=obj["natoms"].toString().toInt();
    if(obj.contains("tolminiters")) tolminiters=obj["tolminiters"].toString().toInt();
    if(obj.contains("outputfile")) outputfile=obj["outputfile"].toString();
    xx=new double[natoms];
    yy =new double[natoms];
    zz =new double[natoms];
}

int	getdimension()
{
    return	3 *natoms;
}

int	hasmargins()
{
    return 1;
}

void	getleftmargin(double *x)
{
    for(int i=0;i<3*natoms;i++)
    {
        x[i]=-1;
    }
}

void	getrightmargin(double *x)
{
    for(int i=0;i<3*natoms;i++)
    {
        x[i]=1;
    }
}


double	Pow(double x,int n)
{
    double p=1.0;
    for(int i=0;i<n;i++) p*=x;
    return p;
}

double	pot(double r)
{
    double	eps=potentialepsilon;
    double 	sig =potentialsigma;
    double	eps4=4.0 *eps;
    double sbyr6=Pow(sig/r,6);
    return eps4 * sbyr6*(sbyr6-1.0);
}

void	done(double *x)
{
    extern double tolmin(double *,int);
    double f=tolmin(x,tolminiters);
    int i;
    //print output
    //conformation and discovered minimum
    for(i=1;i<=natoms;i++)
        {
                int i3=3 * i;
                xx[i-1]=x[i3-2-1];
                yy[i-1]=x[i3-1-1];
                zz[i-1]=x[i3-1];
        }
    QFile f1(outputfile);
    if(f1.exists())
    {
        int n=1;
        do
        {
            QString nfile=outputfile+"-"+QString::number(n);
            QFile f2(nfile);
            if(!f2.exists())
            {
                outputfile=nfile;
                break;
            }
            n++;
        }while(true);
    }
    FILE *fp=fopen(outputfile.toStdString().c_str(),"w");
    fprintf(fp,"%d\n",natoms);
    for(i=0;i<natoms;i++)
    {
        fprintf(fp,"%lf %lf %lf\n",xx[i],yy[i],zz[i]);
    }
    fprintf(fp,"%lf\n",f);
    fclose(fp);

    delete[] xx;
    delete[] yy;
    delete[] zz;
}

double	funmin(double *x)
{
    double value=0.0;
    for(int i=1;i<=natoms;i++)
    {
        int i3=3 * i;
        xx[i-1]=x[i3-2-1];
        yy[i-1]=x[i3-1-1];
        zz[i-1]=x[i3-1];
    }
        for(int i=1;i<=natoms-1;i++)
        {
                for(int j=i+1;j<=natoms;j++)
                {
                        if(i==j) continue;
                        double dx=xx[i-1]-xx[j-1];
                        double dy=yy[i-1]-yy[j-1];
                        double dz=zz[i-1]-zz[j-1];
                        double rij=sqrt(dx*dx+dy*dy+dz*dz);
                        value=value+pot(rij);
                }
        }

    return  value;
}


void	granal(double *x,double *g)
{
    for(int i=0;i<3 * natoms;i++)
    {
        double eps=pow(1e-18,1.0/3.0)*dmax(1.0,fabs(x[i]));
        x[i]+=eps;
        double v1=funmin(x);
        x[i]-=2.0 *eps;
        double v2=funmin(x);
        g[i]=(v1-v2)/(2.0 * eps);
        x[i]+=eps;
    }
}

}
