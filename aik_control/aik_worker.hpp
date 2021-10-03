#pragma once

#include <QObject>

#include <aik/aik.hpp>

class aik;

class aik_worker : public QObject {
    Q_OBJECT
public:
    aik_worker();

public slots:
    void start();

signals:
    void dispatch_debug_message(QString debug_msg);

    void set_player_speed(float player_speed);
    void set_player_attack_speed(quint32 player_attack_speed);

    void set_target_speed(float target_speed);
    void set_target_attack_speed(quint32 target_attack_speed);

    void set_target_x(float target_x);
    void set_target_y(float target_y);
    void set_target_z(float target_z);

private:
    void process_shared_mem_dbg_msg(const QString& dbg_msg);

    void process_read_values(const AIK_READ& up_aik_read);

    aik m_aik;
    QString m_debug_message;
};

