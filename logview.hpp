#pragma once
#include <QPlainTextEdit>
#include <QFile>

class LogView : public QPlainTextEdit
{
    Q_OBJECT
public:
    LogView(QWidget *parent, bool write_dbg_file = true);
    ~LogView();
    void write_to_file();
public slots:
    void append_message(const QString& _msg);
private:
    QFile* m_log_file;
    bool m_write_to_file;
};
