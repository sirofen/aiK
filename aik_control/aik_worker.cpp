#include <aik_control/aik_worker.hpp>
#include <aik/aik.hpp>

#include <aik_control/aik_process_values.hpp>

#include <QThread>
#include <QDebug>

namespace {
constexpr LPCTSTR mutex_name = TEXT("aiKMutex");
constexpr LPCTSTR file_mapping_name = TEXT("Global\\aiKFileMapping");

constexpr auto aik_tick_delay = 100;
}

aik_worker::aik_worker() {
    if (!m_aik.init_shared_mutex(mutex_name, AIK_INIT_APPROACH::CREATE)) {
        return;
    }
    qDebug() << "Init shared mutex success";
    if (!m_aik.init_shared_memory(file_mapping_name, AIK_INIT_APPROACH::CREATE)) {
        return;
    }
    qDebug() << "Init shared memory success";
}

void aik_worker::set_aik_value_handler(aik_process_values* value_handler) {
    this->m_paik_process_values = value_handler;
    qDebug() << ("Installed value handler: " + QString("0x%1").arg((std::uint64_t)value_handler, 8, 16, QLatin1Char( '0' )));
}

void aik_worker::start() {
    DISPATCH_SHARED pshared_structs;
    for (;;QThread::msleep(aik_tick_delay)) {
        //AIK_READ _a;
        //pshared_structs.m_aik_read->dbg_wprint = "3232";
        m_aik.read_shared_values(pshared_structs);
        //qDebug() << pshared_structs.m_aik_read->player_speed;
        this->process_read_values(*pshared_structs.m_aik_read);
    }
}

void aik_worker::reset_settings() {
    this->m_paik_process_values->set_player_speed_operation(std::nullopt);
    this->m_paik_process_values->set_player_attack_speed_operation(std::nullopt);

    m_aik.write_shared_values(AIK_WRITE{.speed = m_unchanged_player_speed,
                                        .attack_speed = m_unchanged_player_attack_speed}.contruct_dispatch());

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
    m_aik.write_shared_values(AIK_WRITE{.speed = m_unchanged_player_speed}.contruct_dispatch());
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
    m_aik.write_shared_values(AIK_WRITE{.attack_speed = m_unchanged_player_attack_speed}.contruct_dispatch());
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
    m_aik.write_shared_values(AIK_WRITE{.player_x = m_player_x_to_write}.contruct_dispatch());
}
void aik_worker::write_player_y_to_client() {
    m_aik.write_shared_values(AIK_WRITE{.player_y = m_player_y_to_write}.contruct_dispatch());
}
void aik_worker::write_player_z_to_client() {
    m_aik.write_shared_values(AIK_WRITE{.player_z = m_player_z_to_write}.contruct_dispatch());
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
    m_aik.write_shared_values(AIK_WRITE{.speed = m_unchanged_player_speed}.contruct_dispatch());
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
    m_aik.write_shared_values(AIK_WRITE{.attack_speed = m_unchanged_player_attack_speed}.contruct_dispatch());
    emit player_attack_speed_write_operation("");
    emit debug_qstr("Player attack speed " + _operation + " not an operation");
}

void aik_worker::process_target_speed_qstring(const QString& _operation) {
    //m_target_speed_operation = aik_write_instructions(_operation.toStdString());
}
void aik_worker::process_target_attack_speed_qstring(const QString& _operation) {
    //m_target_attack_speed_operation = aik_write_instructions(_operation.toStdString());
}
#include <aik/shared_mutex.hpp>
#include <aik/shared_memory.hpp>
void aik_worker::stop_client() {
    emit debug_qstr("Stopping client");
    m_aik.write_shared_values(AIK_READ{.m_run = false}.contruct_dispatch());
    if (m_aik.m_shared_mutex) {
        m_aik.m_shared_mutex->~shared_mutex();
    }
    if (m_aik.m_shared_memory) {
        m_aik.m_shared_memory->~shared_memory();
    }

}

void aik_worker::process_shared_mem_dbg_msg(const QString& dbg_msg) {
    if (dbg_msg.isEmpty() || this->m_debug_message == dbg_msg) {
        return;
    }
    //prev_msg_cache = dbg_msg;
    /* append to prev string if no newline for versatility */
//    if (dbg_msg.contains(QChar::LineFeed) || dbg_msg.contains(QChar::CarriageReturn)) {
    this->m_debug_message = dbg_msg;
    // flush values
    m_aik.write_shared_values(AIK_READ{}.contruct_dispatch());
//        emit dispatch_debug_message(this->m_debug_message);
//        return;
//    }
    //this->m_debug_message.append(dbg_msg);
    emit debug_qstr(this->m_debug_message);
    return;
}

void aik_worker::dispatch_shared_values_to_write(const AIK_WRITE& _aik_write) {
    m_aik.write_shared_values(_aik_write.contruct_dispatch());
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

void aik_worker::process_read_values(const AIK_READ& up_aik_read) {
    AIK_WRITE _aik_write{};
    this->process_shared_mem_dbg_msg(QString::fromWCharArray(up_aik_read.dbg_wprint));

    _aik_write.no_gravity = m_no_gravity_button_state == Qt::Checked;
    _aik_write.radar = m_radar_button_state == Qt::Checked;
    _aik_write.disable_console = m_console_button_state != Qt::Checked;
    //qDebug() << __FUNCTION__ << m_client_player_speed << " : " << up_aik_read.player_speed;
    if (auto player_speed = up_aik_read.player_speed; player_speed != 0 && m_client_player_speed != player_speed) {
        if (m_player_speed_button_state != Qt::Checked) {
            m_unchanged_player_speed = player_speed;
        }
        if (m_player_speed_button_state == Qt::Checked && m_paik_process_values && m_paik_process_values->get_player_speed_operation()) {
            //qDebug() << m_paik_process_values->get_player_speed_operation()->operation_qstring();
            auto _t_sp = player_speed;
            player_speed = m_paik_process_values->get_player_speed_operation()->apply(player_speed);
            emit debug_qstr("Player speed operation found. " +
                                        QString::number(_t_sp) +
                                        " {" +
                                        m_paik_process_values->get_player_speed_operation()->operation_qstring() +
                                        "} " +
                                        "-> (float)" +
                                        QString::number(player_speed));
            _aik_write.speed = player_speed;
        } else {
            _aik_write.speed = m_unchanged_player_speed;
            //qDebug() << m_unchanged_player_speed << " unch:cur " << m_client_player_speed;
            emit debug_qstr("Received Player Speed: " + QString::number(player_speed));
        }
        m_client_player_speed = player_speed;
        //qDebug() << __FUNCTION__ << " : "<< m_client_player_speed << " : " << player_speed;

        emit set_player_speed(player_speed);
    }
    if (auto player_attack_speed = up_aik_read.player_attack_speed; player_attack_speed && m_client_player_attack_speed != player_attack_speed) {
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

            _aik_write.attack_speed = player_attack_speed;
        } else {
            _aik_write.attack_speed = m_unchanged_player_attack_speed;
            emit debug_qstr("Received Player Attack Speed: " + QString::number(player_attack_speed));
        }
        m_client_player_attack_speed = player_attack_speed;
        emit set_player_attack_speed(player_attack_speed);
    }
    if (prev_player_x != up_aik_read.player_x) {
        prev_player_x = up_aik_read.player_x;
        emit set_ui_player_x(up_aik_read.player_x);
        emit debug_qstr("Received Player X: " + QString::number(up_aik_read.player_x));
    }
    if (prev_player_y != up_aik_read.player_y) {
        prev_player_y = up_aik_read.player_y;
        emit set_ui_player_y(up_aik_read.player_y);
        emit debug_qstr("Received Player Y: " + QString::number(up_aik_read.player_y));
    }
    if (prev_player_z != up_aik_read.player_z) {
        prev_player_z = up_aik_read.player_z;
        emit set_ui_player_z(up_aik_read.player_z);
        emit debug_qstr("Received Player Z: " + QString::number(up_aik_read.player_z));
    }
    if (prev_target_speed != up_aik_read.target_speed) {
        prev_target_speed = up_aik_read.target_speed;
        emit set_target_speed(up_aik_read.target_speed);
        emit debug_qstr("Received Target Speed: " + QString::number(up_aik_read.target_speed));
    }
    if (prev_target_attack_speed != up_aik_read.target_attack_speed) {
        prev_target_attack_speed = up_aik_read.target_attack_speed;
        emit set_target_attack_speed(up_aik_read.target_attack_speed);
        emit debug_qstr("Received Target Attack Speed: " + QString::number(up_aik_read.target_attack_speed));
    }
    if (prev_target_x != up_aik_read.target_x) {
        prev_target_x = up_aik_read.target_x;
        emit set_target_x(up_aik_read.target_x);
        emit debug_qstr("Received Target X: " + QString::number(up_aik_read.target_x));
    }
    if (prev_target_y != up_aik_read.target_y) {
        prev_target_y = up_aik_read.target_y;
        emit set_target_y(up_aik_read.target_y);
        emit debug_qstr("Received Target Y: " + QString::number(up_aik_read.target_y));
    }
    if (prev_target_z != up_aik_read.target_z) {
        prev_target_z = up_aik_read.target_z;
        emit set_target_z(up_aik_read.target_z);
        emit debug_qstr("Received Target Z: " + QString::number(up_aik_read.target_z));
    }
    m_aik.write_shared_values(_aik_write.contruct_dispatch());
}
void aik_worker::apply_existing_inst_and_write_player_speed() {
    if (!m_paik_process_values || !m_paik_process_values->get_player_speed_operation()) {
        return;
    }
    const auto _new_speed = m_paik_process_values->get_player_speed_operation()->apply(m_client_player_speed);
    emit set_player_speed(_new_speed);
    //qDebug() << __FUNCTION__ << ": " << m_client_player_speed << " : " << _new_speed;
    this->m_client_player_speed = _new_speed;
    m_aik.write_shared_values(AIK_WRITE{.speed = _new_speed}.contruct_dispatch());
}
void aik_worker::apply_existing_inst_and_write_player_attack_speed() {
    if (!m_paik_process_values || !m_paik_process_values->get_player_attack_speed_operation()) {
        return;
    }
    const auto _new_attack_speed = m_paik_process_values->get_player_attack_speed_operation()->apply(m_client_player_attack_speed);
    emit set_player_attack_speed(_new_attack_speed);
    this->m_client_player_attack_speed = _new_attack_speed;
    m_aik.write_shared_values(AIK_WRITE{.attack_speed = _new_attack_speed}.contruct_dispatch());
}
