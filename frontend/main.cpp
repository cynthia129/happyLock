#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    // 创建Qt应用对象
    QApplication app(argc, argv);
    // 创建主窗口并显示
    MainWindow w;
    w.show();
    // 进入Qt事件循环
    return app.exec();
} 