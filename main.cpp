#include "aikwidget.h"
#include <QStyleFactory>
#include <QSharedMemory>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    QSharedMemory sharedMemory("aiktyshugoconsolemuchlove");
    if(!sharedMemory.create(1) || sharedMemory.error() == QSharedMemory::AlreadyExists) {
        QMessageBox msg_box;
        msg_box.setWindowIcon(AIK_ICON);
        msg_box.setText(a.tr("An instance is already running"));
        msg_box.setWindowTitle(a.tr("aiK"));
        msg_box.exec();
        return 1;
    }

    aikwidget w;
    //w.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    w.show();

    return a.exec();
}
