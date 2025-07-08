#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "NetworkClient.h"
#include <QDialog>
#include <QTextBrowser>

// MainWindow类是前端主窗口，负责文档编辑、版本管理、用户列表等UI交互。
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    // 构造函数，初始化主窗口
    MainWindow(QWidget *parent = nullptr);
    // 析构函数
    ~MainWindow();

private:
    // UI组件
    QTextEdit* editor;                // 文档编辑器
    QListWidget* versionList;         // 版本列表控件
    QLabel* docTitle;                 // 文档标题标签
    QLabel* statusLabel;              // 状态栏标签

    // 版本管理弹窗和列表
    QDialog* versionDialog = nullptr;         // 版本管理对话框
    QListWidget* versionListWidget = nullptr; // 版本管理列表控件

    // 菜单动作
    QAction* newAction;           // 新建文档动作
    QAction* openAction;          // 打开文档动作
    QAction* saveAction;          // 保存文档动作
    QAction* versionAction;       // 版本管理动作
    QAction* exitAction;          // 退出动作
    QAction* connectAction;       // 连接服务器动作
    QAction* switchDocAction;     // 切换文档动作

    NetworkClient* netClient;     // 网络通信客户端
    int currentDocId = 1;         // 当前文档ID
    QString username = "Lily";    // 当前用户名
    bool ignoreTextChanged = false; // 是否忽略文本变化信号
    QListWidget* userListWidget;  // 在线用户列表控件

    // UI初始化相关方法
    //设置菜单栏
    void setupMenu();
    //设置主布局
    void setupLayout();
    //设置状态栏
    void setupStatusBar();

private slots:
    // 槽函数：新建、打开、保存、版本、退出等操作
    void onNew();
    void onOpen();
    void onSave();
    void onVersion();
    void onExit();
    void onVersionSelected(QListWidgetItem* item);
    void onConnectServer();
    void onSwitchDocument();
    void onShowVersionManager();
    void onVersionItemClicked(QListWidgetItem* item);
    void handleRollbackResult(const QJsonObject& obj);
    void onVersionItemDoubleClicked(QListWidgetItem* item);
}; 