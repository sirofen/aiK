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

    void reset_settings();

    void update_player_speed_processing_button_state(int state);
    void update_player_attack_speed_processing_button_state(int state);

    void set_player_gravity_button_state(int state);
    void set_radar_button_state(int state);

    void set_player_x_to_write(double _v);
    void set_player_y_to_write(double _v);
    void set_player_z_to_write(double _v);

    void write_player_x_to_client();
    void write_player_y_to_client();
    void write_player_z_to_client();

    void process_player_speed_qstring(const QString& _operation);
    void process_player_attack_speed_qstring(const QString& _operation);

    void process_target_speed_qstring(const QString& _operation);
    void process_target_attack_speed_qstring(const QString& _operation);

    void stop_client();

signals:
    void debug_qstr(QString debug_msg);

    void set_player_speed(float player_speed);
    void set_player_attack_speed(quint32 player_attack_speed);

    void set_ui_player_x(double _v);
    void set_ui_player_y(double _v);
    void set_ui_player_z(double _v);

    void set_target_speed(float target_speed);
    void set_target_attack_speed(quint32 target_attack_speed);

    void set_target_x(float target_x);
    void set_target_y(float target_y);
    void set_target_z(float target_z);

    void player_speed_write_operation(const QString& _op_str);
    void player_attack_speed_write_operation(const QString& _op_str);

private:
    void process_shared_mem_dbg_msg(const QString& dbg_msg);

    void dispatch_shared_values_to_write(const AIK_WRITE& _aik_write);

    void process_read_values(const AIK_READ& up_aik_read);

    void apply_existing_inst_and_write_player_speed();
    void apply_existing_inst_and_write_player_attack_speed();


    aik_process_values* m_paik_process_values;

    int m_player_speed_button_state;
    int m_player_attack_speed_button_state;

    int m_no_gravity_button_state;
    int m_client_radar_button_state;

    std::uint32_t m_client_player_attack_speed = 0;
    float m_client_player_speed = 0;

    float m_unchanged_player_speed = 0;
    std::uint32_t m_unchanged_player_attack_speed = 0;

    float m_player_x_to_write;
    float m_player_y_to_write;
    float m_player_z_to_write;

    aik m_aik;
    QString m_debug_message;
};

