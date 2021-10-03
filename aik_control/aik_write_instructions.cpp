#include <aik_control/aik_write_instructions.hpp>
#include <string>
#include <QDebug>

aik_write_instructions::aik_write_instructions(AIK_INSTRUCTIONS _operator, std::uint32_t _operand)
    : m_operator(_operator)
    , m_operand(_operand) {}
aik_write_instructions::aik_write_instructions(AIK_INSTRUCTIONS _operator, float _operand)
    : m_operator(_operator)
    , m_operand(_operand) {}
aik_write_instructions::aik_write_instructions(std::string _str_operation) {
    auto operand_offset = 0;
    if (std::isdigit(_str_operation[0])) {
        m_operator = AIK_INSTRUCTIONS::CONSTANT;
    } else {
        m_operator = (AIK_INSTRUCTIONS) _str_operation[0];
        operand_offset++;
    }
    std::size_t _read_sz;
    try {
        std::uint32_t tmp_int = std::stoul(_str_operation.c_str() + operand_offset, &_read_sz);
        if (_read_sz == _str_operation.length() - operand_offset) {
            //_int_val = tmp_int;
            //qDebug() << "intval" << tmp_int;
            m_operand = tmp_int;
            return;
        }
        float tmp_float = std::stof(_str_operation.c_str() + operand_offset, &_read_sz);
        if (_read_sz == _str_operation.length() - operand_offset) {
            //_float_val = tmp_float;
            //qDebug() << "floatval" << tmp_int;
            m_operand = tmp_float;
            return;
        }
    }  catch (const std::exception& e) {
        qWarning(e.what());
    }

}

QString aik_write_instructions::operation_qstring() {
    if (this->m_operator == AIK_INSTRUCTIONS::CONSTANT) {
        return {};
    }
    switch (m_operand.index()) {
        case 0: {
            auto operand_val = std::get<std::uint32_t>(m_operand);
            if (operand_val == 0) {
                return {};
            }
            return ((char)m_operator + QString::number(operand_val));
            break;
        }
        case 1: {
            auto operand_val = std::get<float>(m_operand);
            if (operand_val == 0) {
                return {};
            }
            return((char)m_operator + QString::number(operand_val));
            break;
        }
    }
    return {};
}
