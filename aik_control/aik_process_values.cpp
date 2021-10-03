#include <aik_control/aik_process_values.hpp>
#include <QKeyEvent>
#include <utility>

namespace {
std::vector<int> utility_keys = { Qt::Key_Backspace
                                 ,Qt::Key_Delete
                                 ,Qt::Key_Left
                                 ,Qt::Key_Right
                                 ,Qt::Key_Control
                                 ,Qt::Key_Alt
                                 ,Qt::Key_Home
                                 ,Qt::Key_End
                                 ,Qt::Key_Shift };
}

aik_process_values::aik_process_values(QWidget* parent, std::string mask)
    : QWidget(parent)
    , m_str_mask(std::move(mask)) {
    std::sort(utility_keys.begin(), utility_keys.end());
}

void aik_process_values::process_player_speed_qstring(const QString& _operation) {
    auto _write_inst = aik_write_instructions(_operation.toStdString());
    m_player_speed_operation = _write_inst;
    if (auto _qstr_oper = _write_inst.operation_qstring(); !_qstr_oper.isEmpty()) {
        emit debug_qstr("Player speed operation: " + _qstr_oper);
        emit player_speed_write_operation(_qstr_oper);
        return;
    }
    emit player_speed_write_operation("");
    emit debug_qstr("Player speed " + _operation + " not an operation");
    //int32_t _val = 11;
    //qDebug() << m_player_speed_operation->apply<int32_t>(_val);
}
void aik_process_values::process_player_attack_speed_qstring(const QString& _operation) {
    auto _write_inst = aik_write_instructions(_operation.toStdString());
    m_player_attack_speed_operation = _write_inst;
    if (auto _qstr_oper = _write_inst.operation_qstring(); !_qstr_oper.isEmpty()) {
        emit debug_qstr("Player attack speed operation: " + _qstr_oper);
        emit player_attack_speed_write_operation(_qstr_oper);
        return;
    }
    emit player_attack_speed_write_operation("");
    emit debug_qstr("Player attack speed " + _operation + " not an operation");
}

void aik_process_values::process_target_speed_qstring(const QString& _operation) {
    m_target_speed_operation = aik_write_instructions(_operation.toStdString());
}
void aik_process_values::process_target_attack_speed_qstring(const QString& _operation) {
    m_target_attack_speed_operation = aik_write_instructions(_operation.toStdString());
}

bool aik_process_values::eventFilter(QObject *object, QEvent *event) {
    if (event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(object, event);
    }
    QKeyEvent *ke = static_cast<QKeyEvent*>(event);
    const auto key = ke->key();
    if (key == Qt::Key_Escape || key == Qt::Key_Return) {
        this->setFocus(Qt::OtherFocusReason);
        return true;
    }
    /* don't ignore utility keys than check mask */
    if (!std::binary_search(utility_keys.begin(), utility_keys.end(), key) &&
        std::strchr(m_str_mask.c_str(), key) == nullptr) {
        return true;
    }
    return false;
}
