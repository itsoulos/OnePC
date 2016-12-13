#ifndef ABOUTPROBLEMDIALOG_H
#define ABOUTPROBLEMDIALOG_H
#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QFile>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTabWidget>
#include <QTableView>
#include <QTableWidget>
#include <QHeaderView>
#include "database.h"
class AboutProblemDialog : public QDialog
{
    Q_OBJECT
private:
    QTableWidget  *table;
    QPushButton *closeButton;
    QPushButton *saveButton;
    QPushButton *plotButton;
    Database     *database;
    QString     problemName;
    QPixmap getOsIcon(QString name);
    void buildTable();
    void    saveData(QString contents);
    QVector<ProblemResult> result;
    QMap<QString,QPixmap> pixicon;
public:
    AboutProblemDialog(QString name,Database *db,QWidget *parent=0);
    ~AboutProblemDialog();
public slots:
    void    saveProblemData();
    void    saveResultSlot();
    void    deleteResultSlot();
};

#endif // ABOUTPROBLEMDIALOG_H
