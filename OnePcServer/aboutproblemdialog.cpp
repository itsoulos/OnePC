#include "aboutproblemdialog.h"
#define WIDTH 800
#define HEIGHT 600
# define ICON_WIDTH     (WIDTH/10)
# define ICON_HEIGHT    (HEIGHT/15)

AboutProblemDialog::AboutProblemDialog(QString name,Database *db,QWidget *parent)
{
    problemName=name;
    database=db;
    pixicon.insert("ubuntu",QPixmap(":/images/linux_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT));
    pixicon.insert("debian",QPixmap(":/images/linux_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT));
    pixicon.insert("android",QPixmap(":/images/android_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT));
    pixicon.insert("ios",QPixmap(":/images/ios_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT));
    pixicon.insert("osx",QPixmap(":/images/apple_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT));
    pixicon.insert("windows",QPixmap(":/images/windows_icon.png").scaled(ICON_WIDTH,ICON_HEIGHT));

    this->setWindowTitle("Information about "+name);
    this->setFixedSize(WIDTH,HEIGHT);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,WIDTH,HEIGHT);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);
    table=new QTableWidget();
    l1->addWidget(table);
    buildTable();
    QHBoxLayout *h4=new QHBoxLayout();
    l1->addLayout(h4);
    saveButton=new QPushButton("SAVE DATA");
    h4->addWidget(saveButton);
    plotButton=new QPushButton("PLOT DATA");
    //h4->addWidget(plotButton);
    closeButton=new QPushButton("CLOSE");
    h4->addWidget(closeButton);
    connect(closeButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(saveButton,SIGNAL(clicked()),this,SLOT(saveProblemData()));


}

QPixmap AboutProblemDialog::getOsIcon(QString name)
{ 
    return pixicon[name];
 }

void    AboutProblemDialog::buildTable()
{
    result=database->getResult(problemName);
    QStringList m_TableHeader;
    m_TableHeader<<tr("Minimum")<<tr("Generations")<<tr("Operating System")<<tr("Save")<<tr("Delete");
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels(m_TableHeader);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setShowGrid(true);
    table->setRowCount(result.size());

    QPixmap p1(":/images/floppy.png");
    p1=p1.scaled(2*ICON_WIDTH/3,2*ICON_HEIGHT/3);
    QPixmap p2(":/images/trash.png");
    p2=p2.scaled(2*ICON_WIDTH/3,2*ICON_HEIGHT/3);
    QIcon i1(p1);
    QIcon i2(p2);
    QPixmap pp=getOsIcon("ubuntu");
    for(int i=0;i<result.size();i++)
    {
        table->setItem(i,0,new QTableWidgetItem(QString::number(result[i].besty)));
        table->setItem(i,1,new QTableWidgetItem(QString::number(result[i].generations)));
        QLabel *l=new QLabel();
        table->setCellWidget(i,2,l);

         l->setPixmap(getOsIcon(result[i].osname));
        QPushButton *saveButton=new QPushButton();
        table->setCellWidget(i,3,saveButton);
        saveButton->setIcon(i1);
        saveButton->setIconSize(p1.rect().size());
        QPushButton *deleteButton=new QPushButton();
        table->setCellWidget(i,4,deleteButton);
        deleteButton->setIcon(i2);
        deleteButton->setIconSize(p2.rect().size());
        saveButton->setProperty("position",QVariant(i));
        deleteButton->setProperty("position",QVariant(i));
        connect(saveButton,SIGNAL(clicked(bool)),this,SLOT(saveResultSlot()));
        connect(deleteButton,SIGNAL(clicked(bool)),this,SLOT(deleteResultSlot()));
    }

    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setColumnWidth(2,WIDTH/4);
    table->setColumnWidth(3,WIDTH/6);
    table->setColumnWidth(4,WIDTH/6);

}

void    AboutProblemDialog::saveData(QString contents)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                   ".",
                                   tr("Text File (*.txt)"));

        QFile file(fileName);
        bool saveFlag=true;
        if(file.exists())
        {
            QMessageBox::StandardButton resBtn = QMessageBox::question( this,
                       "OnePcServer Server",
                       tr("Overwrite existing file. Are you sure ?\n"),
                      QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                      QMessageBox::Yes);
                if (resBtn != QMessageBox::Yes)
                {
                    saveFlag=false;
                }
        }
        if(saveFlag)
        {
            file.open(QIODevice::WriteOnly);
            QTextStream out(&file);
            out<<contents;
            file.close();
        }
}

void    AboutProblemDialog::saveResultSlot()
{
    QObject *sender=QObject::sender();
    QVariant property=sender->property("position");
    int position=property.toInt();
    QJsonObject object=database->result2json(result[position]);
    QJsonDocument doc(object);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    saveData(strJson);
}

void    AboutProblemDialog::deleteResultSlot()
{
    QObject *sender=QObject::sender();
    QVariant property=sender->property("position");
    int position=property.toInt();
    ProblemResult t=result[position];
    //delete elements in this position
    table->removeRow(position);
    result.remove(position);
    //change every propery
    for(int i=0;i<result.size();i++)
    {
        QObject *obj1=table->cellWidget(i,3);

        obj1->setProperty("position",QVariant(i));
        QObject *obj2=table->cellWidget(i,4);
        obj2->setProperty("position",QVariant(i));
    }
    //delete record from database
    database->deleteResult(problemName,t);
}

void    AboutProblemDialog::saveProblemData()
{
    QJsonObject object;
    object["count"]=result.size();
    QJsonArray array;
    for(int i=0;i<result.size();i++)
    {
        QJsonObject obj=database->result2json(result[i]);
        array.append(obj);
    }
    object["result"]=array;
    QJsonDocument doc(object);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    saveData(strJson);
/*
    */
}

AboutProblemDialog::~AboutProblemDialog()
{

}

