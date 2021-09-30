#ifndef AIKWIDGET_H
#define AIKWIDGET_H

#include <QWidget>
#include <QString>

#define AIK_ICON QIcon(":/aiK/res/aiK.jpg")

QT_BEGIN_NAMESPACE
namespace Ui { class aikwidget; }
QT_END_NAMESPACE

class aikwidget : public QWidget
{

    Q_OBJECT

    //Q_PROPERTY(READ ui->playerSpeedMod->returnPressed() WRITE ui->playerAttackSpeedMod->returnPressed() NOTIFY playerAttackSpeedChanged)
    //Q_PROPERTY(QString playerSpeed READ getPlayerSpeed WRITE setPlayerSpeed NOTIFY playerSpeedChanged)

public:
    aikwidget(QWidget *parent = nullptr);
    ~aikwidget();
    QString getPlayerSpeed() const;

public slots:
    void setPlayerSpeed(const QString& playerSpeed);
    void setPlayerAttackSpeed(const QString& playerAttackSpeed);

    void setTargetSpeed(const QString& targetSpeed);
    void setTargetAttackSpeed(const QString& targetAttackSpeed);

    void setPlayerSpeed(char* playerSpeed);
    void _setPlayerAttackSpeed(char* playerAttackSpeed);

    void setTargetSpeed(char* targetSpeed);
    void setTargetAttackSpeed(char* targetAttackSpeed);

    void setDebugString(char* debugString);
    void setDebugQString(const QString& dbg_qstr);

//signals:
    void playerSpeedChanged();
    //void playerAttackSpeedChanged();

    //void targetSpeedChanged();
    //void targetAttackSpeedChanged();

private:
    Ui::aikwidget *ui;

    QVector<QObject*> objs_to_lose_focus_on;

    float playerSpeed;
    float playerAttackSpeed;

    float targetSpeed;
    float targetAttackSpeed;

    bool eventFilter(QObject *object, QEvent *event) override;
};
#endif // AIKWIDGET_H
