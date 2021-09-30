#include <aik_worker.hpp>
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
    for (;;QThread::msleep(1000)) {
        //AIK_READ _a;
        DISPATCH_SHARED pshared_structs;
        //pshared_structs.m_aik_read->dbg_wprint = "3232";
        m_aik.read_shared_values(pshared_structs);
        //qDebug() << pshared_structs.m_aik_read->dbg_wprint;
        this->process_read_values(*pshared_structs.m_aik_read);
    }
}

void aik_worker::process_dbg_msg(const QString& dbg_msg) {
    if (dbg_msg.isEmpty() || this->m_debug_message == dbg_msg) {
        return;
    }
    /* append to prev string if no newline for versatility */
    if (dbg_msg.contains(QChar::LineFeed) || dbg_msg.contains(QChar::CarriageReturn)) {
        this->m_debug_message = dbg_msg;
        emit dispatch_debug_message(this->m_debug_message);
        return;
    }
    this->m_debug_message.append(dbg_msg);
    emit dispatch_debug_message(this->m_debug_message);
    return;
}

void aik_worker::process_read_values(const AIK_READ& up_aik_read) {
    this->process_dbg_msg(QString::fromWCharArray(up_aik_read.dbg_wprint));
}
