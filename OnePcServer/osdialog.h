#ifndef OSDIALOG_H
#define OSDIALOG_H
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
# include <QComboBox>
# include <QJsonArray>
# include <QJsonObject>
# include <QJsonDocument>
# include <QTextEdit>
# include <QIcon>
#include <QDebug>
class OsDialog : public QDialog
{
    Q_OBJECT
private:
    QPushButton *okButton,*cancelButton;
    QStringList icons;
    QStringList names;
    QLineEdit *problemEdit;
    QComboBox *operatingSystem;
    QLineEdit *parameterNameEdit;
    QLineEdit *parameterValueEdit;
    QJsonObject parameterJson;
    QPushButton *addParameterButton;
    QPushButton *updateParameterButton;
    QPushButton *deleteParameterButton;
    QTextEdit   *parameterText;
    void    updateJson();
public:
    OsDialog(QWidget *parent=0);
    QString getSelected();
    QString getProblemName();
    QString getSettings();
    ~OsDialog();
public slots:
    void    updateSlot();
    void    addSlot();
    void    deleteSlot();
};

#endif // OSDIALOG_H
