#ifndef PLOTAREA_H
#define PLOTAREA_H

#include <QWidget>
# include <QPainter>
# include <QHBoxLayout>
# include <QVBoxLayout>
# include <problem.h>
# include <QLabel>
class PlotArea : public QWidget
{
    Q_OBJECT
public:
    explicit PlotArea(int w,int h,QWidget *parent = 0);
    void    addPoint(double x,double y);
    void    clearPoints();
    double  getminy();
signals:
    
public slots:
private:
    QLabel *generationLabel;
    QLabel *bestyLabel;
    QLabel *meanyLabel;
    QLabel *worstyLabel;

    Data xpoint;
    Data ypoint;
    double maxx,maxy;
    double minx,miny;
};

#endif // PLOTAREA_H
