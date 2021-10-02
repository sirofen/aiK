#include <aik_control/aik_worker.hpp>
#include <aik/aik.hpp>

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

void aik_worker::start() {
    DISPATCH_SHARED pshared_structs;
    for (;;QThread::msleep(1000)) {
        //AIK_READ _a;
        //pshared_structs.m_aik_read->dbg_wprint = "3232";
        m_aik.read_shared_values(pshared_structs);
        //qDebug() << pshared_structs.m_aik_read->dbg_wprint;
        this->process_read_values(*pshared_structs.m_aik_read);
    }
}

//namespace  {
//    QString prev_msg_cache;
//}

void aik_worker::process_dbg_msg(const QString& dbg_msg) {
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
    emit dispatch_debug_message(this->m_debug_message);
    return;
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
    this->process_dbg_msg(QString::fromWCharArray(up_aik_read.dbg_wprint));
    if (prev_player_speed != up_aik_read.player_speed) {
        prev_player_speed = up_aik_read.player_speed;
        emit set_player_speed(up_aik_read.player_speed);
    }
    if (prev_player_attack_speed != up_aik_read.player_attack_speed) {
        prev_player_attack_speed = up_aik_read.player_attack_speed;
        emit set_player_attack_speed(up_aik_read.player_attack_speed);
    }
    if (prev_target_speed != up_aik_read.target_speed) {
        prev_target_speed = up_aik_read.target_speed;
        emit set_target_speed(up_aik_read.target_speed);
    }
    if (prev_target_attack_speed != up_aik_read.target_attack_speed) {
        prev_target_attack_speed = up_aik_read.target_attack_speed;
        emit set_target_attack_speed(up_aik_read.target_attack_speed);
    }
    if (prev_target_x != up_aik_read.target_x) {
        prev_target_x = up_aik_read.target_x;
        emit set_target_x(up_aik_read.target_x);
    }
    if (prev_target_y != up_aik_read.target_y) {
        prev_target_y = up_aik_read.target_y;
        emit set_target_y(up_aik_read.target_y);
    }
    if (prev_target_z != up_aik_read.target_z) {
        prev_target_z = up_aik_read.target_z;
        emit set_target_z(up_aik_read.target_z);
    }
}
