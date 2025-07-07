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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // UI Components
    QTextEdit* editor;
    QListWidget* versionList;
    QLabel* docTitle;
    QLabel* statusLabel;

    // 版本管理弹窗和列表
    QDialog* versionDialog = nullptr;
    QListWidget* versionListWidget = nullptr;

    // Menu actions
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* versionAction;
    QAction* exitAction;
    QAction* connectAction;
    QAction* switchDocAction;

    NetworkClient* netClient;
    int currentDocId = 1;
    QString username = "Lily";
    bool ignoreTextChanged = false;
    QListWidget* userListWidget;

    void setupMenu();
    void setupLayout();
    void setupStatusBar();

private slots:
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