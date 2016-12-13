#include "clientchartdialog.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
# include <QtCharts/QBarSeries>
# include <QtCharts/QBarSet>
# include <QtCharts/QBarCategoryAxis>
# include <QChartView>
#include <QVBoxLayout>
#include <QDebug>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
using namespace QtCharts;
#define WIDTH 800
#define HEIGHT 600
# define ICON_WIDTH     (WIDTH/10)
# define ICON_HEIGHT    (HEIGHT/15)
ClientChartDialog::ClientChartDialog(QString title,QStringList names,
                                     QVector<qint64> &value,QWidget *parent)
 :QDialog(parent)
{
    QVector<QBarSet*> set;
    QBarSeries *series=new QBarSeries();
    for(int i=0;i<names.size();i++)
    {
        QBarSet *s=new QBarSet(QString("")+names[i]);
        *s<<value[i];
        set.append(s);
        series->append(set[i]);
    }

    this->setWindowTitle("Speed chart");
    this->setFixedSize(WIDTH,HEIGHT);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,WIDTH,HEIGHT);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Speed chart ");
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QStringList categories;
    categories.append("Clients");
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);


    chartView->resize(WIDTH/2,HEIGHT/2);

    l1->addWidget(chartView);
}

ClientChartDialog::~ClientChartDialog()
{

}
