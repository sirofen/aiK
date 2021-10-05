#pragma once

#include <QWidget>
#include <QString>

#define AIK_ICON QIcon(":/aiK/res/aiK.jpg")

QT_BEGIN_NAMESPACE
namespace Ui { class aikwidget; }
QT_END_NAMESPACE

class aikwidget : public QWidget
{

    Q_OBJECT


public:
    aikwidget(QWidget *parent = nullptr);
    ~aikwidget();

    QString get_player_speed() const;

signals:
    void input_player_speed(const QString& _val);
    void input_player_attack_speed(const QString& _val);

    void input_target_speed(const QString& _val);
    void input_target_attack_speed(const QString& _val);

    void _dbg_qstr(const QString& dbg_qstr);

    void stop_client();

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

    void set_debug_qstring(const QString& dbg_qstr);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void closeEvent(QCloseEvent *event);

private:
    Ui::aikwidget *ui;

    bool m_lmb_pressed = false;
    QPoint m_mouse_prev_pos;

    QVector<QObject*> objs_to_lose_focus_on;

    float player_speed;
    float player_attack_speed;

    float target_speed;
    float target_attack_speed;
};
