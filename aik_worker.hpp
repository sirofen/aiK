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

private:
    void process_dbg_msg(const QString& dbg_msg);

    void process_read_values(const AIK_READ& up_aik_read);

    aik m_aik;
    QString m_debug_message;
};

