#include "aikwidget.h"
#include "./ui_aikwidget.h"

#include <QKeyEvent>
#include <QProcess>
#include <QThread>

#include <collapse_button.h>
#include <aik_control/aik_process_values.hpp>
#include <aik_control/aik_worker.hpp>
#include <style/style.hpp>

#include <wintaskscheduler.hpp>

namespace {
constexpr auto kAikTaskName = "aik";

constexpr auto kInputMask = "1234567890.,*+=";
constexpr auto aik_bin_rel_path = "usermode.exe";
constexpr auto kAikBinLoadDrvArgs = "-LOADDRIVER";
}

aikwidget::aikwidget(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
    , ui(new Ui::aikwidget)
{
    this->setWindowIcon(AIK_ICON);
    ui->setupUi(this);
    // TODO: change to relative path
    this->m_aik_bin = QString("C:/Users/deale/source/repos/sirofen/aion_kernel/x64/Release") + "/" + aik_bin_rel_path;
    //this->m_aik_bin = qApp->applicationDirPath() + "/" + aik_bin_rel_path;
    ui->drvOnStartupCheck->setChecked(WinTaskScheduler::isTaskInstalled(kAikTaskName));
    connect(ui->drvOnStartupCheck, &QCheckBox::stateChanged, this, &aikwidget::set_load_driver_on_startup);
    connect(ui->loadDriverButton, &QPushButton::pressed, this, &aikwidget::load_driver);

    connect(this, &aikwidget::_dbg_qstr, ui->consolePlainText, &LogView::append_message);
    /* expanding layout */
    ui->toolButton->setContent(this, ui->consoleFrame, ui->consolePlainText);
    /* elements style */
    ui->minimize_button->setStyleSheet(StyleHelper::getMinimizeStyleSheet());
    ui->close_button->setStyleSheet(StyleHelper::getCloseStyleSheet());

    connect(ui->minimize_button, &QToolButton::clicked, this, &QWidget::showMinimized);
    connect(ui->close_button, &QToolButton::clicked, this, &QWidget::close);

    /* qline filters */
    aik_process_values *aik_proc_val = new aik_process_values(this, kInputMask);
    ui->playerSpeedMod->installEventFilter(aik_proc_val);
    ui->playerAttackSpeedMod->installEventFilter(aik_proc_val);
//    ui->targetSpeedMod->installEventFilter(aik_proc_val);
//    ui->targetAttackSpeedMod->installEventFilter(aik_proc_val);

    /* lose focus on enter/escape */
    ui->playerXCoordBox->installEventFilter(this);
    ui->playerYCoordBox->installEventFilter(this);
    ui->playerZCoordBox->installEventFilter(this);

//    /* qline process entered values */
//    connect(this, &aikwidget::input_player_speed, aik_proc_val, &aik_process_values::process_player_speed_qstring);
//    connect(ui->playerSpeedMod, SIGNAL(editingFinished()), this, SLOT(read_input_player_speed()));

//    connect(this, &aikwidget::input_player_attack_speed, aik_proc_val, &aik_process_values::process_player_attack_speed_qstring);
//    connect(ui->playerAttackSpeedMod, SIGNAL(editingFinished()), this, SLOT(read_input_player_attack_speed()));
//    // display operations
//    connect(aik_proc_val, &aik_process_values::player_speed_write_operation, ui->playerSpeedFunLabel, &QLabel::setText);
//    connect(aik_proc_val, &aik_process_values::player_attack_speed_write_operation, ui->playerAttackSpeedFunLabel, &QLabel::setText);
    // connect debug msg from aik_process_values
    connect(aik_proc_val, &aik_process_values::debug_qstr, this, &aikwidget::set_debug_qstring);

    /* start aik worker */
    QThread* aik_worker_thread = new QThread();
    aik_worker* _aik_worker = new aik_worker();
    _aik_worker->moveToThread(aik_worker_thread);
    // setup value handler
    _aik_worker->set_aik_value_handler(aik_proc_val);

    connect(_aik_worker, &aik_worker::debug_qstr, this, &aikwidget::set_debug_qstring);

    //connect( aik_worker_thread, &QThread::started, this, &aikwidget::start_aik);
    connect( aik_worker_thread, &QThread::started, _aik_worker, &aik_worker::start);

    connect(ui->resetSettingsButton, &QPushButton::clicked, _aik_worker, &aik_worker::reset_settings, Qt::DirectConnection);
    connect(ui->resetSettingsButton, &QPushButton::clicked, this, &aikwidget::reset_settings);

    /* qline process entered values */
    connect(this, &aikwidget::input_player_speed, _aik_worker, &aik_worker::process_player_speed_qstring, Qt::DirectConnection);
    connect(ui->playerSpeedMod, SIGNAL(editingFinished()), this, SLOT(read_input_player_speed()));

    connect(this, &aikwidget::input_player_attack_speed, _aik_worker, &aik_worker::process_player_attack_speed_qstring, Qt::DirectConnection);
    connect(ui->playerAttackSpeedMod, SIGNAL(editingFinished()), this, SLOT(read_input_player_attack_speed()));

    /* player pos process entered values */
    connect(ui->playerXCoordBox, &QDoubleSpinBox::valueChanged, _aik_worker, &aik_worker::set_player_x_to_write, Qt::DirectConnection);
    connect(ui->playerXCoordBox, &QDoubleSpinBox::editingFinished, _aik_worker, &aik_worker::write_player_x_to_client, Qt::DirectConnection);
    connect(ui->playerYCoordBox, &QDoubleSpinBox::valueChanged, _aik_worker, &aik_worker::set_player_y_to_write, Qt::DirectConnection);
    connect(ui->playerYCoordBox, &QDoubleSpinBox::editingFinished, _aik_worker, &aik_worker::write_player_y_to_client, Qt::DirectConnection);
    connect(ui->playerZCoordBox, &QDoubleSpinBox::valueChanged, _aik_worker, &aik_worker::set_player_z_to_write, Qt::DirectConnection);
    connect(ui->playerZCoordBox, &QDoubleSpinBox::editingFinished, _aik_worker, &aik_worker::write_player_z_to_client, Qt::DirectConnection);
    // pos send read values to ui
    connect(_aik_worker, &aik_worker::set_ui_player_x, ui->playerXCoordBox, &QDoubleSpinBox::setValue);
    connect(_aik_worker, &aik_worker::set_ui_player_y, ui->playerYCoordBox, &QDoubleSpinBox::setValue);
    connect(_aik_worker, &aik_worker::set_ui_player_z, ui->playerZCoordBox, &QDoubleSpinBox::setValue);

    // display operations
    connect(_aik_worker, &aik_worker::player_speed_write_operation, ui->playerSpeedFunLabel, &QLabel::setText);
    connect(_aik_worker, &aik_worker::player_attack_speed_write_operation, ui->playerAttackSpeedFunLabel, &QLabel::setText);

    connect(this, &aikwidget::stop_client, _aik_worker, &aik_worker::stop_client, Qt::DirectConnection);

    connect(ui->enablePlayerSpeed, &QCheckBox::stateChanged, _aik_worker, &aik_worker::update_player_speed_processing_button_state, Qt::DirectConnection);
    connect(ui->enablePlayerAttackSpeed, &QCheckBox::stateChanged, _aik_worker, &aik_worker::update_player_attack_speed_processing_button_state, Qt::DirectConnection);

    connect(ui->enableNoGravity, &QCheckBox::stateChanged, _aik_worker, &aik_worker::set_player_gravity_button_state, Qt::DirectConnection);
    connect(ui->enableRadar, &QCheckBox::stateChanged, _aik_worker, &aik_worker::set_radar_button_state, Qt::DirectConnection);
    connect(ui->enableConsole, &QCheckBox::stateChanged, _aik_worker, &aik_worker::set_console_button_state, Qt::DirectConnection);

    connect(_aik_worker, &aik_worker::set_player_group_box_title, ui->playerSettingsGroupBox, &QGroupBox::setTitle);

    connect(_aik_worker, &aik_worker::set_player_speed, this, &aikwidget::set_player_speed);
    connect(_aik_worker, &aik_worker::set_player_attack_speed, this, &aikwidget::set_player_attack_speed);

    connect(_aik_worker, &aik_worker::set_gui_ents_enable, this, &aikwidget::lock_gui_ents);

//    connect(_aik_worker, &aik_worker::set_target_speed, this, &aikwidget::set_target_speed);
//    connect(_aik_worker, &aik_worker::set_target_attack_speed, this, &aikwidget::set_target_attack_speed);

//    connect(_aik_worker, &aik_worker::set_target_x, this, &aikwidget::set_target_x);
//    connect(_aik_worker, &aik_worker::set_target_y, this, &aikwidget::set_target_y);
//    connect(_aik_worker, &aik_worker::set_target_z, this, &aikwidget::set_target_z);
    aik_worker_thread->start();
}

aikwidget::~aikwidget()
{
    delete ui;
}

void aikwidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lmb_pressed = true;
        m_mouse_prev_pos = event->pos();
    }
    return QWidget::mousePressEvent(event);
}

void aikwidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lmb_pressed = false;
    }
    return QWidget::mouseReleaseEvent(event);
}

void aikwidget::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << event;
    if (m_lmb_pressed) {
        auto dx = event->position().x() - m_mouse_prev_pos.x();
        auto dy = event->position().y() - m_mouse_prev_pos.y();
        move(x() + dx,y() + dy);
    }
    return QWidget::mouseMoveEvent(event);
}

void aikwidget::closeEvent(QCloseEvent *event) {
    emit stop_client();
}

QString aikwidget::get_player_speed() const {
    return QString::number(player_speed);
}

void aikwidget::load_driver() {
    start_proc(this->m_aik_bin, QStringList(kAikBinLoadDrvArgs), true);
    start_proc(this->m_aik_bin);
}

void aikwidget::start_aik() {
    start_proc(this->m_aik_bin);
}

void aikwidget::reset_settings() {
    this->ui->drvOnStartupCheck->setCheckState(Qt::Unchecked);
    this->ui->enablePlayerSpeed->setCheckState(Qt::Unchecked);
    this->ui->enablePlayerAttackSpeed->setCheckState(Qt::Unchecked);
    this->ui->enableNoGravity->setCheckState(Qt::Unchecked);
    this->ui->enableRadar->setCheckState(Qt::Unchecked);
    this->ui->enableConsole->setCheckState(Qt::Unchecked);
}

void aikwidget::lock_gui_ents(::lock_mod::lock_ents le, bool enable) {
    switch (le) {
        case ::lock_mod::lock_ents::LOAD_DRIVER: {
            this->ui->loadDriverButton->setEnabled(enable);
            break;
        }
        case ::lock_mod::lock_ents::CONSOLE_MOD: {
            this->ui->enableConsole->setEnabled(enable);
            break;
        }
        case ::lock_mod::lock_ents::PLAYER_MOD: {
            this->ui->enableNoGravity->setEnabled(enable);
            this->ui->enablePlayerAttackSpeed->setEnabled(enable);
            this->ui->enablePlayerSpeed->setEnabled(enable);
            this->ui->enableRadar->setEnabled(enable);
            this->ui->playerAttackSpeedFunLabel->setEnabled(enable);
            this->ui->playerAttackSpeedMod->setEnabled(enable);
            this->ui->playerSpeedFunLabel->setEnabled(enable);
            this->ui->playerSpeedMod->setEnabled(enable);
            this->ui->playerPosGroupBox->setEnabled(enable);
            break;
        }
        default: {
            break;
        }
    }
}

void aikwidget::set_load_driver_on_startup(int button_state) {
// https://github.com/grenadium/ShugoConsole
    if(button_state == Qt::Checked)
    {
        emit _dbg_qstr("Creating win task");
        WinTaskScheduler::deleteTask(kAikTaskName);
        WinTaskScheduler::createStartupTask(kAikTaskName, this->m_aik_bin + kAikBinLoadDrvArgs);
    }
    else
    {
        emit _dbg_qstr("Removing win task");
        WinTaskScheduler::deleteTask(kAikTaskName);
    }
}

void aikwidget::set_player_speed(float player_speed) {
    if (QWidget::focusWidget() != ui->playerSpeedMod) {
         ui->playerSpeedMod->setText(QString::number(player_speed));
    }
}
void aikwidget::set_player_attack_speed(quint32 player_attack_speed) {
    if (QWidget::focusWidget() != ui->playerAttackSpeedMod) {
        ui->playerAttackSpeedMod->setText(QString::number(player_attack_speed));
    }
}

//void aikwidget::set_target_speed(float target_speed) {
//    ui->targetSpeedMod->setText(QString::number(target_speed));
//}
//void aikwidget::set_target_attack_speed(quint32 target_attack_speed) {
//    ui->targetAttackSpeedMod->setText(QString::number(target_attack_speed));
//}
//void aikwidget::set_target_x(float target_x) {
//    ui->targetXCoordBox->setValue(target_x);
//}
//void aikwidget::set_target_y(float target_y) {
//    ui->targetYCoordBox->setValue(target_y);
//}
//void aikwidget::set_target_z(float target_z) {
//    ui->targetZCoordBox->setValue(target_z);
//}

void aikwidget::read_input_player_speed() {
    emit input_player_speed(ui->playerSpeedMod->text());
}
void aikwidget::read_input_player_attack_speed() {
    emit input_player_attack_speed(ui->playerAttackSpeedMod->text());
}
//void aikwidget::read_input_target_speed() {
//    emit input_target_speed(ui->targetSpeedMod->text());
//}
//void aikwidget::read_input_target_attack_speed() {
//    emit input_target_attack_speed(ui->targetAttackSpeedMod->text());
//}

void aikwidget::set_debug_qstring(const QString& dbg_qstr) {
    //qDebug() << dbg_qstr;
    emit _dbg_qstr(dbg_qstr);
    //ui->consoleLogLabel->setText(dbg_qstr);
}

bool aikwidget::eventFilter(QObject* object, QEvent* event) {
    if (event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(object, event);
    }
    QKeyEvent *ke = static_cast<QKeyEvent*>(event);
    const auto key = ke->key();
    if (key == Qt::Key_Escape || key == Qt::Key_Return) {
        this->setFocus(Qt::OtherFocusReason);
        return true;
    }
    return false;
}

void aikwidget::start_proc(QString program, QStringList args, bool blocking) {
     if (m_paik_proc) {
        m_paik_proc->close();
    }
     m_paik_proc = new QProcess(this);
     m_paik_proc->start(program, args);
     if (blocking) {
              m_paik_proc->waitForFinished();
     }
//    emit _dbg_qstr("Starting process: " + cmd_line);
//    STARTUPINFO si;
//    PROCESS_INFORMATION pi;

//    ZeroMemory( &si, sizeof(si) );
//    si.cb = sizeof(si);
//    ZeroMemory( &pi, sizeof(pi) );

//    if( !CreateProcess( NULL,
//                       _wcsdup(cmd_line.toStdWString().c_str()),
//                       NULL,
//                       NULL,
//                       FALSE,
//                       0,
//                       NULL,
//                       NULL,
//                       &si,
//                       &pi )
//        )
//    {
//        qDebug() << "CreateProcess failed (" <<  GetLastError() << ").";
//        return;
//    }
////    if (pi.hProcess == NULL || pi.hThread == NULL) {
////        return;
////    }

//    WaitForSingleObject( pi.hProcess, INFINITE );

//    CloseHandle( pi.hProcess );
//    CloseHandle( pi.hThread );
}
