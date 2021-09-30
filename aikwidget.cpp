#include "aikwidget.h"
#include "./ui_aikwidget.h"
#include <collapse_button.h>
#include <pipe.hpp>
#include <QKeyEvent>

#include <aik_worker.hpp>

aikwidget::aikwidget(QWidget *parent)
    : QWidget(parent, Qt::Window
                    | Qt::MSWindowsFixedSizeDialogHint
                    | Qt::CustomizeWindowHint
                    | Qt::WindowTitleHint
                    | Qt::WindowCloseButtonHint /*| Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint*/)
    , ui(new Ui::aikwidget)
{
    this->setWindowIcon(AIK_ICON);
    ui->setupUi(this);
    /* expanding layout */
    ui->toolButton->setContent(this, ui->consoleFrame);
    /* start aik worker */
    QThread* aik_worker_thread = new QThread();
    aik_worker* _aik_worker = new aik_worker();
    _aik_worker->moveToThread(aik_worker_thread);
    connect(_aik_worker, &aik_worker::dispatch_debug_message, this, &aikwidget::setDebugQString);
    connect( aik_worker_thread, &QThread::started, _aik_worker, &aik_worker::start);
    aik_worker_thread->start();

    ui->playerSpeedMod->installEventFilter(this);
    ui->playerAttackSpeedMod->installEventFilter(this);
    ui->targetSpeedMod->installEventFilter(this);
    ui->targetAttackSpeedMod->installEventFilter(this);

    //QObject::connect(this, ui->playerSpeedMod->returnPressed(), this, ui->playerSpeedMod->returnPressed());
    //connect()
    connect(ui->playerSpeedMod, &QLineEdit::returnPressed, this, &aikwidget::playerSpeedChanged);




    ui->consoleLogLabel->setTextFormat(Qt::MarkdownText);
    pipe* _pipe_obj = new pipe(TEXT("\\\\.\\pipe\\Pipe"));

    auto f_h = [this](auto && PH1) { setDebugString(std::forward<decltype(PH1)>(PH1)); };
    _pipe_obj->set_handler_fun(f_h);
    _pipe_obj->set_delay_ms(100);
    _pipe_obj->start();
    //ui->playerSpeedMod->returnPressed()
}

aikwidget::~aikwidget()
{
    delete ui;
}

bool aikwidget::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (const auto& key = ke->key(); key == Qt::Key_Escape || key == Qt::Key_Return) {
            this->setFocus(Qt::OtherFocusReason);
            return true;
        }
    }
    return QObject::eventFilter(object, event);
}

QString aikwidget::getPlayerSpeed() const {
    return QString::number(playerSpeed);
}

void aikwidget::playerSpeedChanged() {
    ui->playerAttackSpeedMod->setText(ui->playerSpeedMod->text());
}

void aikwidget::setPlayerSpeed(const QString& playerSpeed) {
     ui->playerSpeedMod->setText(playerSpeed);
}
void aikwidget::setPlayerAttackSpeed(const QString& playerAttackSpeed) {
    ui->playerAttackSpeedMod->setText(playerAttackSpeed);
}

void aikwidget::setTargetSpeed(const QString& targetSpeed) {
    qDebug() << targetSpeed;
}
void aikwidget::setTargetAttackSpeed(const QString& targetAttackSpeed) {
    qDebug() << targetAttackSpeed;
}

void aikwidget::setPlayerSpeed(char* playerSpeed) {
    auto str = QString(playerSpeed);
    setPlayerSpeed(str);
}
void aikwidget::_setPlayerAttackSpeed(char* playerAttackSpeed) {
    auto str = QString(playerAttackSpeed);
    setPlayerAttackSpeed(str);
}
void aikwidget::setTargetSpeed(char* targetSpeed) {
    auto str = QString(targetSpeed);
    setTargetSpeed(str);
}
void aikwidget::setTargetAttackSpeed(char* targetAttackSpeed) {
    auto str = QString(targetAttackSpeed);
    setTargetAttackSpeed(str);
}

void aikwidget::setDebugString(char* debugString) {
    auto str = QString(debugString);
    qDebug() << str;
    ui->consoleLogLabel->setText(str);
}

void aikwidget::setDebugQString(const QString& dbg_qstr) {
    qDebug() << dbg_qstr;
    ui->consoleLogLabel->setText(dbg_qstr);
}

