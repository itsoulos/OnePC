#include "osdialog.h"
#define WIDTH 800
#define HEIGHT 600
OsDialog::OsDialog(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle(tr("Problem settings"));
    this->setFixedSize(WIDTH,HEIGHT);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,WIDTH,HEIGHT);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);

    names.append("ubuntu");
    names.append("android");
    names.append("ios");
    names.append("osx");
    names.append("windows");

    icons.append(":/images/linux_icon.png");
    icons.append(":/images/android_icon.png");
    icons.append(":/images/ios_icon.png");
    icons.append(":/images/apple_icon.png");
    icons.append(":/images/windows_icon.png");

    QHBoxLayout *editLayout=new QHBoxLayout();
    l1->addLayout(editLayout);
    editLayout->addWidget(new QLabel("Function Name: "));
    problemEdit=new QLineEdit();
    editLayout->addWidget(problemEdit);
    problemEdit->setFixedHeight(HEIGHT/12);
    problemEdit->setFixedWidth(75*WIDTH/100);

    QHBoxLayout *l2=new QHBoxLayout();
    l1->addLayout(l2);
    l2->setAlignment(Qt::AlignTop);

    operatingSystem=new QComboBox();
    operatingSystem->setIconSize(QSize(50,50));
    operatingSystem->setFixedWidth(problemEdit->width());
    l2->addWidget(new QLabel(tr("Operating System: ")));
    l2->addWidget(operatingSystem);
    for(int i=0;i<names.size();i++)
    {
        operatingSystem->addItem(QIcon(icons[i]),names[i]);
    }

    l1->addWidget(new QLabel("<h2 align=center>Parameter setup</h2><hr>"));
    QHBoxLayout *l3=new QHBoxLayout();
    l1->addLayout(l3);
    l3->setAlignment(Qt::AlignTop);
    l3->addWidget(new QLabel(tr("NAME: ")));
    parameterNameEdit=new QLineEdit();
    l3->addWidget(parameterNameEdit);
    l3->addWidget(new QLabel(tr("VALUE: ")));
    parameterValueEdit=new QLineEdit();
    l3->addWidget(parameterValueEdit);
    parameterNameEdit->setFixedWidth(WIDTH/3);
    parameterValueEdit->setFixedWidth(parameterNameEdit->width());
    QHBoxLayout *l4=new QHBoxLayout();
    l1->addLayout(l4);
    l4->setAlignment(Qt::AlignTop);
    addParameterButton=new QPushButton();
    addParameterButton->setText(tr("ADD"));
    l4->addWidget(addParameterButton);
    updateParameterButton=new QPushButton();
    updateParameterButton->setText(tr("UPDATE"));
    l4->addWidget(updateParameterButton);
    deleteParameterButton=new QPushButton();
    deleteParameterButton->setText(tr("DELETE"));
    l4->addWidget(deleteParameterButton);
    addParameterButton->setFixedWidth(3*WIDTH/10);
    updateParameterButton->setFixedWidth(addParameterButton->width());
    deleteParameterButton->setFixedWidth(addParameterButton->width());
    QHBoxLayout *l5=new QHBoxLayout();
    l1->addLayout(l5);
    parameterText=new QTextEdit();
    parameterText->setFixedWidth(8*WIDTH/10);
    parameterText->setReadOnly(true);
    parameterText->setAlignment(Qt::AlignTop);
    l5->addWidget(parameterText,0,Qt::AlignCenter);
    connect(addParameterButton,SIGNAL(clicked(bool)),this,SLOT(addSlot()));
    connect(updateParameterButton,SIGNAL(clicked(bool)),this,SLOT(updateSlot()));
    connect(deleteParameterButton,SIGNAL(clicked(bool)),this,SLOT(deleteSlot()));

    QHBoxLayout *buttonLayout=new QHBoxLayout();
    l1->addLayout(buttonLayout);
    okButton=new QPushButton("OK");
    buttonLayout->addWidget(okButton);
    cancelButton=new QPushButton("CANCEL");
    buttonLayout->addWidget(cancelButton);
    connect(okButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}

void    OsDialog::updateJson()
{

    QJsonDocument doc(parameterJson);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    parameterText->setText(strJson);
}

void    OsDialog::updateSlot()
{
    if(parameterNameEdit->text().size()==0) return;
    parameterJson[parameterNameEdit->text()]=parameterValueEdit->text();
    updateJson();
}

void    OsDialog::addSlot()
{
      if(parameterNameEdit->text().size()==0) return;
    parameterJson[parameterNameEdit->text()]=parameterValueEdit->text();
    updateJson();
}

void    OsDialog::deleteSlot()
{
      if(parameterNameEdit->text().size()==0) return;
    parameterJson.remove(parameterNameEdit->text());
    updateJson();
}

QString OsDialog::getProblemName()
{
    return problemEdit->text();
}

QString OsDialog::getSelected()
{
    return names[operatingSystem->currentIndex()];
}

QString OsDialog::getSettings()
{
    QJsonDocument doc(parameterJson);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    return strJson;
}

OsDialog::~OsDialog()
{

}

