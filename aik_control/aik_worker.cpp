#include <aik_control/aik_worker.hpp>
#include <aik/aik.hpp>

#include <aik_control/aik_process_values.hpp>

#include <QThread>
#include <QDebug>

namespace {
constexpr LPCTSTR mutex_name = TEXT("aiKMutex");
constexpr LPCTSTR file_mapping_name = TEXT("Global\\aiKFileMapping");
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
    for (;;QThread::msleep(1000)) {
        //AIK_READ _a;
        //pshared_structs.m_aik_read->dbg_wprint = "3232";
        m_aik.read_shared_values(pshared_structs);
        //qDebug() << pshared_structs.m_aik_read->player_speed;
        this->process_read_values(*pshared_structs.m_aik_read);
    }
}

void aik_worker::update_player_speed_processing_button_state(int state) {
    emit debug_qstr("Player speed button state changed: " + QString::number(state));
    this->m_player_speed_button_state = state;
}
void aik_worker::update_player_attack_speed_processing_button_state(int state) {
    emit debug_qstr("Player attack speed button state changed: " + QString::number(state));
    this->m_player_attack_speed_button_state = state;
}

//void aik_worker::set_player_speed_processing_button_state(bool _b) {
//    qDebug() << "here";
//    qDebug() << _b;
//    //bool button_checked = _b == Qt::Checked;
//    //this->m_player_speed_processing_enabled = button_checked;
//    //emit dispatch_debug_message(QString("Player speed processing ") + (button_checked? "enabled" : "disabled"));
//}
//void aik_worker::set_player_attack_speed_processing_button_state(int _b) {
//    qDebug() << _b;
//    bool button_checked = _b == Qt::Checked;
//    this->m_player_attack_speed_processing_enabled = button_checked;
//    emit dispatch_debug_message(QString("Player attack speed processing ") + (button_checked? "enabled" : "disabled"));
//}

//namespace  {
//    QString prev_msg_cache;
//}

void aik_worker::process_shared_mem_dbg_msg(const QString& dbg_msg) {
    if (dbg_msg.isEmpty() || this->m_debug_message == dbg_msg) {
        return;
    }
    //prev_msg_cache = dbg_msg;
    /* append to prev string if no newline for versatility */
//    if (dbg_msg.contains(QChar::LineFeed) || dbg_msg.contains(QChar::CarriageReturn)) {
        this->m_debug_message = dbg_msg;
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
float prev_player_speed = {};
qint32 prev_player_attack_speed = {};

float prev_target_speed = {};
qint32 prev_target_attack_speed = {};

float prev_target_x = {};
float prev_target_y = {};
float prev_target_z = {};
}

void aik_worker::process_read_values(const AIK_READ& up_aik_read) {
    AIK_WRITE _aik_write{};
    this->process_shared_mem_dbg_msg(QString::fromWCharArray(up_aik_read.dbg_wprint));
    if (auto player_speed = up_aik_read.player_speed; prev_player_speed != player_speed) {
        prev_player_speed = player_speed;

        if (m_player_speed_button_state == Qt::Checked && m_paik_process_values && m_paik_process_values->get_player_speed_operation()) {
            player_speed = m_paik_process_values->get_player_speed_operation()->apply(player_speed);
            emit debug_qstr("Player speed operation found. " +
                                        QString::number(prev_player_speed) +
                                        " {" +
                                        m_paik_process_values->get_player_speed_operation()->operation_qstring() +
                                        "} " +
                                        "-> (float)" +
                                        QString::number(player_speed));

            _aik_write.speed = player_speed;
        } else {
            emit debug_qstr("Received Player Speed: " + QString::number(player_speed));
        }
        emit set_player_speed(player_speed);
    }
    if (auto player_attack_speed = up_aik_read.player_attack_speed; prev_player_attack_speed != player_attack_speed) {
        prev_player_attack_speed = player_attack_speed;

        if (m_player_attack_speed_button_state == Qt::Checked && m_paik_process_values && m_paik_process_values->get_player_attack_speed_operation()) {
            player_attack_speed = m_paik_process_values->get_player_attack_speed_operation()->apply(player_attack_speed);
            emit debug_qstr("Player attack speed operation found. " +
                                        QString::number(prev_player_attack_speed) +
                                        " {" +
                                        m_paik_process_values->get_player_attack_speed_operation()->operation_qstring() +
                                        "} " +
                                        "-> (int)" +
                                        QString::number(player_attack_speed));

            _aik_write.attack_speed = player_attack_speed;
        } else {
            emit debug_qstr("Received Player Attack Speed: " + QString::number(player_attack_speed));
        }
        emit set_player_attack_speed(player_attack_speed);
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
    dispatch_shared_values_to_write(_aik_write);
}
