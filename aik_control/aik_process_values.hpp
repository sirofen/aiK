#pragma once
#include <QWidget>
#include <optional>

#include <aik_control/aik_write_instructions.hpp>

class aik_process_values : public QWidget {
    Q_OBJECT
public:
    explicit aik_process_values(QWidget* parent, std::string mask);
public slots:
    void process_player_speed_qstring(const QString& _operation);
    void process_player_attack_speed_qstring(const QString& _operation);

    void process_target_speed_qstring(const QString& _operation);
    void process_target_attack_speed_qstring(const QString& _operation);

protected:
    bool eventFilter(QObject* object, QEvent* event) override;
private:
    std::string m_str_mask;

    std::optional<aik_write_instructions> m_player_speed_operation;
    std::optional<aik_write_instructions> m_player_attack_speed_operation;

    std::optional<aik_write_instructions> m_target_speed_operation;
    std::optional<aik_write_instructions> m_target_attack_speed_operation;
};
