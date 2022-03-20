#pragma once
#include <QtGlobal>
#include <string_view>
#include <variant>
#include <QDebug>

enum class aik_operator : char {
    PLUS = '+',
    MINUS = '-',
    MULTIPLY = '*',
    DIVIDE = '/',
    CONSTANT
};

class aik_write_instructions
{
    using operand = std::variant<std::uint32_t, float>;
public:
    aik_write_instructions(aik_operator _operator, std::uint32_t _operand);
    aik_write_instructions(aik_operator _operator, float _operand);

    aik_write_instructions(std::string _str_operation);

    template <typename T, std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, bool> = true>
    /* I hate this, but i wanna sleep */
    T apply(T& _val) {
        if (m_operand.index() == 0) {
            auto operand_val = std::get<std::uint32_t>(m_operand);
            try {
                switch(this->m_operator) {
                    case aik_operator::PLUS: {
                        return _val += operand_val;
                    }
                    case aik_operator::MINUS: {
                        return _val -= operand_val;
                    }
                    case aik_operator::MULTIPLY: {
                        return _val *= operand_val;
                    }
                    case aik_operator::DIVIDE: {
                        return _val /= operand_val;
                    }
                    case aik_operator::CONSTANT: {
                        return _val = operand_val;
                    }
                    default: {
                        return _val;
                    }
                }
            }  catch (const std::exception& e) {
                qWarning(e.what());
                return _val;
            }
        } else if (m_operand.index() == 1) {
            auto operand_val = std::get<float>(m_operand);
            try {
                switch(this->m_operator) {
                    case aik_operator::PLUS: {
                        return _val += operand_val;
                    }
                    case aik_operator::MINUS: {
                        return _val -= operand_val;
                    }
                    case aik_operator::MULTIPLY: {
                        return _val *= operand_val;
                    }
                    case aik_operator::DIVIDE: {
                        return _val /= operand_val;
                    }
                    case aik_operator::CONSTANT: {
                        return _val = operand_val;
                    }
                    default: {
                        return _val;
                    }
                }
            }  catch (const std::exception& e) {
                qWarning(e.what());
                return _val;
            }
        }
        return {};
    }

    aik_operator operation_type();

    QString operation_qstring();

private:
    aik_operator m_operator;
    operand m_operand;
};
