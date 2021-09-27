#include "aikwidget.h"
#include "./ui_aikwidget.h"
#include <collapse_button.h>
#include <pipe.hpp>
#include <QKeyEvent>

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
    ui->toolButton->setContent(this, ui->consoleFrame);

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

