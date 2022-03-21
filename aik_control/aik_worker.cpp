#include <aik_control/aik_worker.hpp>

#include <aik_control/aik_process_values.hpp>

#include <aik/symbols/aik_process.hpp>
#include <aik/symbols/aion_write_builder.hpp>
#include <aik/symbols/client_values.hpp>
#include <aik/symbols/aion_write.hpp>

#include <QThread>
#include <QDebug>

namespace {
constexpr auto aik_tick_delay = 100;
}

aik_worker::aik_worker()
    : m_aik_process(new aik::aik_process()) {
    m_aik_process->force_init_async();
}

aik_worker::~aik_worker() = default;

void aik_worker::set_aik_value_handler(aik_process_values* value_handler) {
    this->m_paik_process_values = value_handler;
    qDebug() << ("Installed value handler: " + QString("0x%1").arg((std::uint64_t)value_handler, 8, 16, QLatin1Char( '0' )));
}

void aik_worker::start() {
    for (;;QThread::msleep(aik_tick_delay)) {
        this->process_read_values(m_aik_process->read_client_values());
    }
}

void aik_worker::reset_settings() {
    this->m_paik_process_values->set_player_speed_operation(std::nullopt);
    this->m_paik_process_values->set_player_attack_speed_operation(std::nullopt);

    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_speed(m_unchanged_player_speed);
    aion_write_builder.set_attack_speed(m_unchanged_player_attack_speed);

    emit player_speed_write_operation("");
    emit player_attack_speed_write_operation("");
}

void aik_worker::update_player_speed_processing_button_state(int state) {
    emit debug_qstr("Player speed button state changed: " + QString::number(state));
    this->m_player_speed_button_state = state;
    if (!m_unchanged_player_speed) {
        return;
    }
    if (state == Qt::Checked) {
        apply_existing_inst_and_write_player_speed();
        return;
    }
    //qDebug() << "writing unchanged" << m_unchanged_player_speed;
    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_speed(m_unchanged_player_speed);
    m_aik_process->write_client_values(aion_write_builder.build());
}
void aik_worker::update_player_attack_speed_processing_button_state(int state) {
    emit debug_qstr("Player attack speed button state changed: " + QString::number(state));
    this->m_player_attack_speed_button_state = state;
    if (!m_unchanged_player_attack_speed) {
        return;
    }
    if (state == Qt::Checked) {
        apply_existing_inst_and_write_player_attack_speed();
        return;
    }
    //qDebug() << "writing unchanged" << m_unchanged_player_speed;
    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_attack_speed(m_unchanged_player_attack_speed);
    m_aik_process->write_client_values(aion_write_builder.build());
}

void aik_worker::set_player_gravity_button_state(int state) {
    emit debug_qstr("No gravity button state changed: " + QString::number(state));
    this->m_no_gravity_button_state = state;
}
void aik_worker::set_radar_button_state(int state) {
    emit debug_qstr("Radar button state changed: " + QString::number(state));
    this->m_radar_button_state = state;
}
void aik_worker::set_console_button_state(int state) {
    emit debug_qstr("Console button state changed: " + QString::number(state));
    this->m_console_button_state = state;
}
void aik_worker::set_player_x_to_write(double _v) {
    this->m_player_x_to_write = (float)_v;
}
void aik_worker::set_player_y_to_write(double _v) {
    this->m_player_y_to_write = (float)_v;
}
void aik_worker::set_player_z_to_write(double _v) {
    this->m_player_z_to_write = (float)_v;
}
void aik_worker::write_player_x_to_client() {
    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_pos_x(m_player_x_to_write);
    m_aik_process->write_client_values(aion_write_builder.build());
}
void aik_worker::write_player_y_to_client() {
    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_pos_y(m_player_y_to_write);
    m_aik_process->write_client_values(aion_write_builder.build());
}
void aik_worker::write_player_z_to_client() {
    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_pos_z(m_player_z_to_write);
    m_aik_process->write_client_values(aion_write_builder.build());
}
void aik_worker::process_player_speed_qstring(const QString& _operation) {
    auto _write_inst = aik_write_instructions(_operation.toStdString());
    if (auto _qstr_oper = _write_inst.operation_qstring(); !_qstr_oper.isEmpty()) {
        this->m_paik_process_values->set_player_speed_operation(_write_inst);
        emit debug_qstr("Player speed operation: " + _qstr_oper);
        emit player_speed_write_operation(_qstr_oper);
        //qDebug() << __FUNCTION__ << " : " << m_client_player_speed;
        if (m_player_speed_button_state == Qt::Checked) {
            apply_existing_inst_and_write_player_speed();
        }
        return;
    }
    this->m_paik_process_values->set_player_speed_operation(std::nullopt);
    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_speed(m_unchanged_player_speed);
    m_aik_process->write_client_values(aion_write_builder.build());
    //m_aik.write_shared_values(AIK_WRITE{.speed = m_unchanged_player_speed}.contruct_dispatch());
    emit player_speed_write_operation("");
    emit debug_qstr("Player speed " + _operation + " not an operation");
    //int32_t _val = 11;
    //qDebug() << m_player_speed_operation->apply<int32_t>(_val);
}
void aik_worker::process_player_attack_speed_qstring(const QString& _operation) {
    auto _write_inst = aik_write_instructions(_operation.toStdString());
    if (auto _qstr_oper = _write_inst.operation_qstring(); !_qstr_oper.isEmpty()) {
        this->m_paik_process_values->set_player_attack_speed_operation(_write_inst);
        emit debug_qstr("Player attack speed operation: " + _qstr_oper);
        emit player_attack_speed_write_operation(_qstr_oper);
        if (m_player_attack_speed_button_state == Qt::Checked) {
            apply_existing_inst_and_write_player_attack_speed();
        }
        return;
    }
    this->m_paik_process_values->set_player_attack_speed_operation(std::nullopt);
    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_attack_speed(m_unchanged_player_attack_speed);
    m_aik_process->write_client_values(aion_write_builder.build());
    emit player_attack_speed_write_operation("");
    emit debug_qstr("Player attack speed " + _operation + " not an operation");
}

void aik_worker::process_target_speed_qstring(const QString& _operation) {
    //m_target_speed_operation = aik_write_instructions(_operation.toStdString());
}
void aik_worker::process_target_attack_speed_qstring(const QString& _operation) {
    //m_target_attack_speed_operation = aik_write_instructions(_operation.toStdString());
}

void aik_worker::stop_client() {
    emit debug_qstr("Stopping client");
}

void aik_worker::map_driver() {
    m_aik_process->map_driver();
}

void aik_worker::process_shared_mem_dbg_msg(const QString& dbg_msg) {
    if (dbg_msg.isEmpty()) {
        return;
    }
    // flush values
    //m_aik.write_shared_values(AIK_READ{}.contruct_dispatch());
    if (this->m_debug_message == dbg_msg) {
        return;
    }
    //prev_msg_cache = dbg_msg;
    /* append to prev string if no newline for versatility */
//    if (dbg_msg.contains(QChar::LineFeed) || dbg_msg.contains(QChar::CarriageReturn)) {
    this->m_debug_message = dbg_msg;
//    qDebug() << ".";
//        emit dispatch_debug_message(this->m_debug_message);
//        return;
//    }
    //this->m_debug_message.append(dbg_msg);
    emit debug_qstr(this->m_debug_message);
    return;
}

namespace {
float prev_target_speed = {};
qint32 prev_target_attack_speed = {};

float prev_player_x = {};
float prev_player_y = {};
float prev_player_z = {};

float prev_target_x = {};
float prev_target_y = {};
float prev_target_z = {};
}

void aik_worker::process_read_values(aik::client_values aik_client_values) {
    aik::aion_write_builder aion_write_builder;
    //AIK_WRITE _aik_write{};
    //this->process_shared_mem_dbg_msg(QString::fromWCharArray(up_aik_read.dbg_wprint));

    if (!aik_client_values.get_name().empty()) {
        // TODO: change function to use QStringView
        emit set_player_group_box_title(QString::fromStdString(std::string(aik_client_values.get_name())));
    }

    emit set_gui_ents_enable(::lock_mod::lock_ents::PLAYER_MOD, m_aik_process->is_player_found());
    emit set_gui_ents_enable(::lock_mod::lock_ents::LOAD_DRIVER, !m_aik_process->is_process_attached());
    emit set_gui_ents_enable(::lock_mod::lock_ents::CONSOLE_MOD, m_aik_process->is_console_pattern_found());

    aion_write_builder.set_gravity(m_no_gravity_button_state != Qt::Checked);
    aion_write_builder.set_radar(m_radar_button_state == Qt::Checked);
    aion_write_builder.set_console(m_console_button_state == Qt::Checked);

    if (auto player_speed = aik_client_values.get_speed(); player_speed != 0 && m_client_player_speed != player_speed) {
        if (m_player_speed_button_state != Qt::Checked) {
            m_unchanged_player_speed = player_speed;
        }
        if (m_player_speed_button_state == Qt::Checked && m_paik_process_values && m_paik_process_values->get_player_speed_operation()) {
            auto _t_sp = player_speed;
            player_speed = m_paik_process_values->get_player_speed_operation()->apply(player_speed);
            emit debug_qstr("Player speed operation found. " +
                                        QString::number(_t_sp) +
                                        " {" +
                                        m_paik_process_values->get_player_speed_operation()->operation_qstring() +
                                        "} " +
                                        "-> (float)" +
                                        QString::number(player_speed));
            aion_write_builder.set_speed(player_speed);
        } else {
            aion_write_builder.set_speed(m_unchanged_player_speed);
            emit debug_qstr("Received Player Speed: " + QString::number(player_speed));
        }
        m_client_player_speed = player_speed;

        emit set_player_speed(player_speed);
    }
    if (auto player_attack_speed = aik_client_values.get_attack_speed(); player_attack_speed && m_client_player_attack_speed != player_attack_speed) {
        if (m_player_attack_speed_button_state != Qt::Checked) {
            m_unchanged_player_attack_speed = player_attack_speed;
        }
        if (m_player_attack_speed_button_state == Qt::Checked && m_paik_process_values && m_paik_process_values->get_player_attack_speed_operation()) {
            auto _t_at_sp = player_attack_speed;
            player_attack_speed = m_paik_process_values->get_player_attack_speed_operation()->apply(player_attack_speed);
            emit debug_qstr("Player attack speed operation found. " +
                                        QString::number(_t_at_sp) +
                                        " {" +
                                        m_paik_process_values->get_player_attack_speed_operation()->operation_qstring() +
                                        "} " +
                                        "-> (int)" +
                                        QString::number(player_attack_speed));

            aion_write_builder.set_attack_speed(player_attack_speed);
        } else {
            aion_write_builder.set_attack_speed(m_unchanged_player_attack_speed);
            emit debug_qstr("Received Player Attack Speed: " + QString::number(player_attack_speed));
        }
        m_client_player_attack_speed = player_attack_speed;
        emit set_player_attack_speed(player_attack_speed);
    }
    if (prev_player_x != aik_client_values.get_pos_x()) {
        prev_player_x = aik_client_values.get_pos_x();
        emit set_ui_player_x(aik_client_values.get_pos_x());
        emit debug_qstr("Received Player X: " + QString::number(aik_client_values.get_pos_x()));
    }
    if (prev_player_y != aik_client_values.get_pos_y()) {
        prev_player_y = aik_client_values.get_pos_y();
        emit set_ui_player_y(aik_client_values.get_pos_y());
        emit debug_qstr("Received Player Y: " + QString::number(aik_client_values.get_pos_y()));
    }
    if (prev_player_z != aik_client_values.get_pos_z()) {
        prev_player_z = aik_client_values.get_pos_z();
        emit set_ui_player_z(aik_client_values.get_pos_z());
        emit debug_qstr("Received Player Z: " + QString::number(aik_client_values.get_pos_z()));
    }
//    if (prev_target_speed != up_aik_read.target_speed) {
//        prev_target_speed = up_aik_read.target_speed;
//        emit set_target_speed(up_aik_read.target_speed);
//        emit debug_qstr("Received Target Speed: " + QString::number(up_aik_read.target_speed));
//    }
//    if (prev_target_attack_speed != up_aik_read.target_attack_speed) {
//        prev_target_attack_speed = up_aik_read.target_attack_speed;
//        emit set_target_attack_speed(up_aik_read.target_attack_speed);
//        emit debug_qstr("Received Target Attack Speed: " + QString::number(up_aik_read.target_attack_speed));
//    }
//    if (prev_target_x != up_aik_read.target_x) {
//        prev_target_x = up_aik_read.target_x;
//        emit set_target_x(up_aik_read.target_x);
//        emit debug_qstr("Received Target X: " + QString::number(up_aik_read.target_x));
//    }
//    if (prev_target_y != up_aik_read.target_y) {
//        prev_target_y = up_aik_read.target_y;
//        emit set_target_y(up_aik_read.target_y);
//        emit debug_qstr("Received Target Y: " + QString::number(up_aik_read.target_y));
//    }
//    if (prev_target_z != up_aik_read.target_z) {
//        prev_target_z = up_aik_read.target_z;
//        emit set_target_z(up_aik_read.target_z);
//        emit debug_qstr("Received Target Z: " + QString::number(up_aik_read.target_z));
//    }
    m_aik_process->write_client_values(aion_write_builder.build());
}
void aik_worker::apply_existing_inst_and_write_player_speed() {
    if (!m_paik_process_values || !m_paik_process_values->get_player_speed_operation()) {
        return;
    }
    const auto _new_speed = m_paik_process_values->get_player_speed_operation()->apply(m_client_player_speed);
    emit set_player_speed(_new_speed);
    this->m_client_player_speed = _new_speed;

    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_speed(_new_speed);
    m_aik_process->write_client_values(aion_write_builder.build());
}
void aik_worker::apply_existing_inst_and_write_player_attack_speed() {
    if (!m_paik_process_values || !m_paik_process_values->get_player_attack_speed_operation()) {
        return;
    }
    const auto _new_attack_speed = m_paik_process_values->get_player_attack_speed_operation()->apply(m_client_player_attack_speed);
    emit set_player_attack_speed(_new_attack_speed);
    this->m_client_player_attack_speed = _new_attack_speed;

    aik::aion_write_builder aion_write_builder;
    aion_write_builder.set_attack_speed(_new_attack_speed);
    m_aik_process->write_client_values(aion_write_builder.build());
}
