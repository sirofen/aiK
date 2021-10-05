#include "aikwidget.h"
#include "./ui_aikwidget.h"
#include <collapse_button.h>
#include <pipe.hpp>
#include <QKeyEvent>
#include <aik_control/aik_process_values.hpp>

#include <aik_control/aik_worker.hpp>

#include <style/style.hpp>

namespace {
constexpr auto kInputMask = "1234567890.,*+=";
}

aikwidget::aikwidget(QWidget *parent)
    : QWidget(parent,/* Qt::Window
                    | Qt::MSWindowsFixedSizeDialogHint
                    | Qt::CustomizeWindowHint
                    | Qt::WindowTitleHint
                    | Qt::WindowCloseButtonHint *//*| Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint*/
                    Qt::FramelessWindowHint)
    , ui(new Ui::aikwidget)
{
    this->setWindowIcon(AIK_ICON);
    ui->setupUi(this);

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
    ui->targetSpeedMod->installEventFilter(aik_proc_val);
    ui->targetAttackSpeedMod->installEventFilter(aik_proc_val);

    /* qline process entered values */
    connect(this, &aikwidget::input_player_speed, aik_proc_val, &aik_process_values::process_player_speed_qstring);
    connect(ui->playerSpeedMod, SIGNAL(editingFinished()), this, SLOT(read_input_player_speed()));

    connect(this, &aikwidget::input_player_attack_speed, aik_proc_val, &aik_process_values::process_player_attack_speed_qstring);
    connect(ui->playerAttackSpeedMod, SIGNAL(editingFinished()), this, SLOT(read_input_player_attack_speed()));
    // display operations
    connect(aik_proc_val, &aik_process_values::player_speed_write_operation, ui->playerSpeedFunLabel, &QLabel::setText);
    connect(aik_proc_val, &aik_process_values::player_attack_speed_write_operation, ui->playerAttackSpeedFunLabel, &QLabel::setText);
    // connect debug msg from aik_process_values
    connect(aik_proc_val, &aik_process_values::debug_qstr, this, &aikwidget::set_debug_qstring);

    /* start aik worker */
    QThread* aik_worker_thread = new QThread();
    aik_worker* _aik_worker = new aik_worker();
    _aik_worker->moveToThread(aik_worker_thread);
    // setup value handler
    _aik_worker->set_aik_value_handler(aik_proc_val);

    connect(_aik_worker, &aik_worker::debug_qstr, this, &aikwidget::set_debug_qstring);
    connect( aik_worker_thread, &QThread::started, _aik_worker, &aik_worker::start);

    // only direct connection works
    connect(this, &aikwidget::stop_client, _aik_worker, &aik_worker::stop_client, Qt::DirectConnection);

    connect(ui->enablePlayerSpeed, &QCheckBox::stateChanged, _aik_worker, &aik_worker::update_player_speed_processing_button_state, Qt::DirectConnection);
    connect(ui->enablePlayerAttackSpeed, &QCheckBox::stateChanged, _aik_worker, &aik_worker::update_player_attack_speed_processing_button_state, Qt::DirectConnection);

    connect(_aik_worker, &aik_worker::set_player_speed, this, &aikwidget::set_player_speed);
    connect(_aik_worker, &aik_worker::set_player_attack_speed, this, &aikwidget::set_player_attack_speed);

    connect(_aik_worker, &aik_worker::set_target_speed, this, &aikwidget::set_target_speed);
    connect(_aik_worker, &aik_worker::set_target_attack_speed, this, &aikwidget::set_target_attack_speed);

    connect(_aik_worker, &aik_worker::set_target_x, this, &aikwidget::set_target_x);
    connect(_aik_worker, &aik_worker::set_target_y, this, &aikwidget::set_target_y);
    connect(_aik_worker, &aik_worker::set_target_z, this, &aikwidget::set_target_z);
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

void aikwidget::set_target_speed(float target_speed) {
    ui->targetSpeedMod->setText(QString::number(target_speed));
}
void aikwidget::set_target_attack_speed(quint32 target_attack_speed) {
    ui->targetAttackSpeedMod->setText(QString::number(target_attack_speed));
}
void aikwidget::set_target_x(float target_x) {
    ui->targetXCordBox->setValue(target_x);
}
void aikwidget::set_target_y(float target_y) {
    ui->targetYCordBox->setValue(target_y);
}
void aikwidget::set_target_z(float target_z) {
    ui->targetZCordBox->setValue(target_z);
}

void aikwidget::read_input_player_speed() {
    emit input_player_speed(ui->playerSpeedMod->text());
}
void aikwidget::read_input_player_attack_speed() {
    emit input_player_attack_speed(ui->playerAttackSpeedMod->text());
}
void aikwidget::read_input_target_speed() {
    emit input_target_speed(ui->targetSpeedMod->text());
}
void aikwidget::read_input_target_attack_speed() {
    emit input_target_attack_speed(ui->targetAttackSpeedMod->text());
}

void aikwidget::set_debug_qstring(const QString& dbg_qstr) {
    //qDebug() << dbg_qstr;
    emit _dbg_qstr(dbg_qstr);
    //ui->consoleLogLabel->setText(dbg_qstr);
}
