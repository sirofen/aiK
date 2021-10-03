#include <logview.hpp>
#include <QScrollBar>
#include <QString>
#include <QDebug>
#include <QDateTime>

namespace {
constexpr auto filename = "dbg.txt";
constexpr auto time_format = "yyyy-MM-dd HH:mm:ss";
}

LogView::LogView(QWidget *parent, bool write_dbg_file)
    : QPlainTextEdit(parent)
    , m_write_to_file(write_dbg_file) {
    if (!m_write_to_file) {
        return;
    }
    m_log_file = new QFile(filename);
    if (!m_log_file->open(QIODevice::WriteOnly | QIODevice::Append |QIODevice::Text)) {
        m_write_to_file = false;
    }
}
LogView::~LogView() {
    if (!m_log_file) {
        return;
    }
    m_log_file->close();
    delete m_log_file;
}
void LogView::write_to_file() {
    if (m_log_file) {
        return;
    }
    m_log_file = new QFile(filename);
    if (!m_log_file->open(QIODevice::WriteOnly | QIODevice::Append |QIODevice::Text)) {
        m_write_to_file = false;
        return;
    }
    m_write_to_file = true;
}

void LogView::append_message(const QString& _msg) {
    this->appendPlainText(_msg);
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
    if (m_write_to_file && m_log_file) {
        //qDebug() << "writing:" << _msg;
        m_log_file->write("["+QDateTime::currentDateTime().toString(time_format).toUtf8()+"] ");
        m_log_file->write(_msg.toUtf8());
        m_log_file->write("\n");
    }
}
