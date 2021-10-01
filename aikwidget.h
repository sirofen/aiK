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
    QString get_player_speed() const;

signals:
    void input_player_speed(const QString& _val);
    void input_player_attack_speed(const QString& _val);

    void input_target_speed(const QString& _val);
    void input_target_attack_speed(const QString& _val);

public slots:
    void set_player_speed(float player_speed);
    void set_player_attack_speed(quint32 player_attack_speed);

    void set_target_speed(float target_speed);
    void set_target_attack_speed(quint32 target_attack_speed);

    void set_target_x(float target_x);
    void set_target_y(float target_y);
    void set_target_z(float target_z);

    void read_input_player_speed();
    void read_input_player_attack_speed();

    void read_input_target_speed();
    void read_input_target_attack_speed();

//    void _setPlayerSpeed(char* playerSpeed);
//    void _setPlayerAttackSpeed(char* playerAttackSpeed);

//    void _setTargetSpeed(char* targetSpeed);
//    void _setTargetAttackSpeed(char* targetAttackSpeed);

//    void setDebugString(char* debugString);
    void set_debug_qstring(const QString& dbg_qstr);

//signals:
    void player_speed_changed();
    //void playerAttackSpeedChanged();

    //void targetSpeedChanged();
    //void targetAttackSpeedChanged();

private:
    Ui::aikwidget *ui;

    QVector<QObject*> objs_to_lose_focus_on;

    float player_speed;
    float player_attack_speed;

    float target_speed;
    float target_attack_speed;

    bool eventFilter(QObject *object, QEvent *event) override;
};
#endif // AIKWIDGET_H
