#include "plotarea.h"
# include <QtGui>
# define MARGIN 30
PlotArea::PlotArea(int w,int h,QWidget *parent) :
    QWidget(parent)
{
    xpoint.resize(0);
    ypoint.resize(0);
    QWidget *mainWidget=new QWidget(this);
    mainWidget->setGeometry(0,0,w,h);
    this->setFixedSize(w,h);
    QVBoxLayout *l=new QVBoxLayout;
    mainWidget->setLayout(l);
    QHBoxLayout *l1=new QHBoxLayout;
    QHBoxLayout *l2=new QHBoxLayout;
    l->addLayout(l1);
    l->addLayout(l2);

    QLabel *t1,*t2,*t3,*t4;
    t1=new QLabel("ITER:");
    t1->setFixedWidth(2*w/10);
    t2=new QLabel("MIN(F):");
    t2->setFixedWidth(2*w/10);
    t3=new QLabel("AVG(F):");
    t3->setFixedWidth(2*w/10);
    t4=new QLabel("MAX(F):");
    t4->setFixedWidth(2*w/10);

    l1->addWidget(t1);
    generationLabel=new QLabel("0");
    l1->addWidget(generationLabel);
    l1->addWidget(t2);
    bestyLabel=new QLabel("0");
    l1->addWidget(bestyLabel);
    l1->addWidget(new QLabel(" "));

    l2->addWidget(t3);
    meanyLabel=new QLabel("0");
    l2->addWidget(meanyLabel);
    l2->addWidget(t4);
    worstyLabel=new QLabel("0");
    l2->addWidget(worstyLabel);
    l2->addWidget(new QLabel(" "));

    bestyLabel->setStyleSheet("color: rgb(100,100,0) ; border: 1px solid  #ff0000 ;");
    meanyLabel->setStyleSheet("color: rgb(100,100,0) ; border: 1px solid  #ff0000 ;");
    generationLabel->setStyleSheet("color: rgb(100,100,0) ; border: 1px solid  #ff0000 ;");
    worstyLabel->setStyleSheet("color: rgb(100,100,0) ; border: 1px solid  #ff0000 ;");
    bestyLabel->setAlignment(Qt::AlignCenter);
    generationLabel->setAlignment(Qt::AlignCenter);
    meanyLabel->setAlignment(Qt::AlignCenter);
    worstyLabel->setAlignment(Qt::AlignCenter);
    generationLabel->setFixedWidth(w/4);
    meanyLabel->setFixedWidth(w/4);
    worstyLabel->setFixedWidth(w/4);
    bestyLabel->setFixedWidth(w/4);
}

double  PlotArea::getminy()
{
    return maxy;
}

void    PlotArea::addPoint(double x,double y)
{
    if(xpoint.size()==0 || x>maxx) maxx=x;
    if(xpoint.size()==0 || x<minx) minx=x;
    if(ypoint.size()==0 || y>maxy) maxy=y;
    if(ypoint.size()==0 || y<miny) miny=y;
    xpoint.push_back(x);
    ypoint.push_back(y);
    generationLabel->setText(QString::number(xpoint.size()));
    bestyLabel->setText(QString::number(-maxy));
    worstyLabel->setText(QString::number(-miny));
    double s=0.0;
    for(int i=0;i<ypoint.size();i++) s+=(-ypoint[i]);
    s=s/ypoint.size();
    meanyLabel->setText(QString::number(s));
}

void    PlotArea::clearPoints()
{
    xpoint.resize(0);
    ypoint.resize(0);
    generationLabel->setText("0");
    bestyLabel->setText("0");
    worstyLabel->setText("0");
    meanyLabel->setText("0");
}
