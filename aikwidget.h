#pragma once

#include <QWidget>
#include <QString>

#include <lock_mod.hpp>

#define AIK_ICON QIcon(":/aiK/res/aiK.jpg")

QT_BEGIN_NAMESPACE
namespace Ui { class aikwidget; }
QT_END_NAMESPACE

class QProcess;

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

    void _dbg_qstr(const QString& dbg_qstr);

    void stop_client();

public slots:
    void start_aik();
    void reset_settings();
    void lock_gui_ents(::lock_mod::lock_ents le, bool enable);

    void set_load_driver_on_startup(int button_state);

    void set_player_speed(float player_speed);
    void set_player_attack_speed(quint32 player_attack_speed);

    void read_input_player_speed();
    void read_input_player_attack_speed();

    void set_debug_qstring(const QString& dbg_qstr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

    bool eventFilter(QObject* object, QEvent* event) override;

private:
    void start_proc(QString program, QStringList args = {}, bool blocking = false);
    QProcess *m_paik_proc{};

    Ui::aikwidget *ui;

    bool m_lmb_pressed = false;
    QPoint m_mouse_prev_pos;

    QVector<QObject*> objs_to_lose_focus_on;

    float player_speed;
    float player_attack_speed;

    float target_speed;
    float target_attack_speed;

    QString m_aik_bin;
};
