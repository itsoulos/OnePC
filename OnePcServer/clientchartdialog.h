#ifndef CLIENTCHARTDIALOG_H
#define CLIENTCHARTDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
class ClientChartDialog : public QDialog
{
Q_OBJECT
public:
    ClientChartDialog(QString title,QStringList names,
                      QVector<qint64> &value,QWidget *parent=0);
    ~ClientChartDialog();
};

#endif // CLIENTCHARTDIALOG_H
