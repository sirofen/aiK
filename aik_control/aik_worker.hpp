#pragma once

#include <QObject>

#include <aik/aik.hpp>

class aik;
class aik_process_values;

class aik_worker : public QObject {
    Q_OBJECT
public:
    aik_worker();

    void set_aik_value_handler(aik_process_values* value_handler);

public slots:
    void start();

    void update_player_speed_processing_button_state(int state);
    void update_player_attack_speed_processing_button_state(int state);

    void stop_client();

signals:
    void debug_qstr(QString debug_msg);

    void set_player_speed(float player_speed);
    void set_player_attack_speed(quint32 player_attack_speed);

    void set_target_speed(float target_speed);
    void set_target_attack_speed(quint32 target_attack_speed);

    void set_target_x(float target_x);
    void set_target_y(float target_y);
    void set_target_z(float target_z);

private:
    void process_shared_mem_dbg_msg(const QString& dbg_msg);

    void dispatch_shared_values_to_write(const AIK_WRITE& _aik_write);

    void process_read_values(const AIK_READ& up_aik_read);

    aik_process_values* m_paik_process_values;

    int m_player_speed_button_state;
    int m_player_attack_speed_button_state;

    aik m_aik;
    QString m_debug_message;
};

