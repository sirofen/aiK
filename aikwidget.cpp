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
    /* expanding layout */
    ui->toolButton->setContent(this, ui->consoleFrame);
    /* elements style */
    //m_task_bar_rect = ui->labelAik->geometry();
    //m_task_bar_rect.setWidth(this->width());
    //qDebug() << m_task_bar_rect;
    //this->setMouseTracking(true);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    //this->setAttribute(Qt::WA_TranslucentBackground);
    //this->setStyleSheet(StyleHelper::getWindowStyleSheet());
    //this->installEventFilter(this);
    //ui->toolBar->installEventFilter(this);
    qDebug() << this->styleSheet();

    //ui->labelAik->setStyleSheet(StyleHelper::getLabelStyleSheet());
    ui->minimize_button->setStyleSheet(StyleHelper::getMinimizeStyleSheet());
    ui->close_button->setStyleSheet(StyleHelper::getCloseStyleSheet());

    connect(ui->minimize_button, &QToolButton::clicked, this, &QWidget::showMinimized);
    connect(ui->close_button, &QToolButton::clicked, this, &QWidget::close);
    /* start aik worker */
    QThread* aik_worker_thread = new QThread();
    aik_worker* _aik_worker = new aik_worker();
    _aik_worker->moveToThread(aik_worker_thread);
    connect(_aik_worker, &aik_worker::dispatch_debug_message, this, &aikwidget::set_debug_qstring);
    connect( aik_worker_thread, &QThread::started, _aik_worker, &aik_worker::start);

    connect(_aik_worker, &aik_worker::set_player_speed, this, &aikwidget::set_player_speed);
    connect(_aik_worker, &aik_worker::set_player_attack_speed, this, &aikwidget::set_player_attack_speed);

    connect(_aik_worker, &aik_worker::set_target_speed, this, &aikwidget::set_target_speed);
    connect(_aik_worker, &aik_worker::set_target_attack_speed, this, &aikwidget::set_target_attack_speed);

    connect(_aik_worker, &aik_worker::set_target_x, this, &aikwidget::set_target_x);
    connect(_aik_worker, &aik_worker::set_target_y, this, &aikwidget::set_target_y);
    connect(_aik_worker, &aik_worker::set_target_z, this, &aikwidget::set_target_z);
    aik_worker_thread->start();
    /* qline filters */
    aik_process_values *aik_proc_val = new aik_process_values(this, kInputMask);
    ui->playerSpeedMod->installEventFilter(aik_proc_val);
    ui->playerAttackSpeedMod->installEventFilter(aik_proc_val);
    ui->targetSpeedMod->installEventFilter(aik_proc_val);
    ui->targetAttackSpeedMod->installEventFilter(aik_proc_val);
    //ui->playerSpeedMod->editingFinished
    /* qline process entered values */
    connect(this, &aikwidget::input_player_speed, aik_proc_val, &aik_process_values::process_player_speed_qstring);
    connect(ui->playerSpeedMod, SIGNAL(editingFinished()), this, SLOT(read_input_player_speed()));
    //QObject::connect(this, ui->playerSpeedMod->returnPressed(), this, ui->playerSpeedMod->returnPressed());
    //connect()
    //connect(ui->playerSpeedMod, &QLineEdit::returnPressed, this, &aikwidget::playerSpeedChanged);

//    ui->consoleLogLabel->setTextFormat(Qt::MarkdownText);
//    pipe* _pipe_obj = new pipe(TEXT("\\\\.\\pipe\\Pipe"));

//    auto f_h = [this](auto && PH1) { set_debug_string(std::forward<decltype(PH1)>(PH1)); };
//    _pipe_obj->set_handler_fun(f_h);
//    _pipe_obj->set_delay_ms(100);
//    _pipe_obj->start();
//    ui->playerSpeedMod->returnPressed()
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

QString aikwidget::get_player_speed() const {
    return QString::number(player_speed);
}

void aikwidget::player_speed_changed() {
    //ui->playerAttackSpeedMod->setText(ui->playerSpeedMod->text());
}

void aikwidget::set_player_speed(float player_speed) {
    if (QWidget::focusWidget() != ui->playerSpeedMod) {
         ui->playerSpeedMod->setText(QString::number(player_speed));
    }
}
void aikwidget::set_player_attack_speed(quint32 player_attack_speed) {
    ui->playerAttackSpeedMod->setText(QString::number(player_attack_speed));
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

//void aikwidget::_setPlayerSpeed(char* playerSpeed) {
//    auto str = QString(playerSpeed);
//    //setPlayerSpeed(str);
//}
//void aikwidget::_setPlayerAttackSpeed(char* playerAttackSpeed) {
//    auto str = QString(playerAttackSpeed);
//    //setPlayerAttackSpeed(str);
//}
//void aikwidget::_setTargetSpeed(char* targetSpeed) {
//    auto str = QString(targetSpeed);
//    //setTargetSpeed(str);
//}
//void aikwidget::_setTargetAttackSpeed(char* targetAttackSpeed) {
//    auto str = QString(targetAttackSpeed);
//    //setTargetAttackSpeed(str);
//}

//void aikwidget::setDebugString(char* debugString) {
//    auto str = QString(debugString);
//    qDebug() << str;
//    ui->consoleLogLabel->setText(str);
//}

void aikwidget::set_debug_qstring(const QString& dbg_qstr) {
    qDebug() << dbg_qstr;
    ui->consoleLogLabel->setText(dbg_qstr);
}

