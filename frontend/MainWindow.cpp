#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include "NetworkClient.h"
#include <QInputDialog>
#include <QDateTime>
#include <QTextBrowser>
#include <QRegularExpression>
#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QColor>
#include <QFont>
#include <QDebug>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>

// 构造函数，初始化主窗口和UI组件，连接信号与槽
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupMenu();
    setupLayout();
    setupStatusBar();
    setWindowTitle("HappyLock 协同文档");
    resize(900, 600);

    netClient = new NetworkClient(this);
    // 连接网络相关信号槽
    connect(netClient, &NetworkClient::connected, this, [this]() {
        statusBar()->showMessage("已连接服务器", 2000);
        statusLabel->setText(QString("用户: %1 | 版本: v1.1 | 已连接").arg(username));
        // 连接后自动切换到当前文档
        if (netClient->isConnected()) {
            QJsonObject obj;
            obj["type"] = "switch_doc";
            obj["docId"] = currentDocId;
            obj["user"] = username;
            netClient->sendJson(obj);
        }
    });
    connect(netClient, &NetworkClient::disconnected, this, [this]() {
        statusBar()->showMessage("已断开服务器", 2000);
        statusLabel->setText(QString("用户: %1 | 版本: v1.1 | 未连接").arg(username));
    });
    connect(netClient, &NetworkClient::jsonReceived, this, [this](const QJsonObject& obj) {
        // 处理后端返回的各种消息类型
        if (obj["type"] == "edit" && obj["docId"].toInt() == currentDocId) {
            ignoreTextChanged = true;
            editor->setPlainText(obj["content"].toString());
            ignoreTextChanged = false;
        } else if (obj["type"] == "switch_doc" && obj["docId"].toInt() == currentDocId) {
            ignoreTextChanged = true;
            editor->setPlainText(obj["content"].toString());
            ignoreTextChanged = false;
        } else if (obj["type"] == "versions" && obj["docId"].toInt() == currentDocId) {
            // 弹出版本列表
            if (versionDialog) { delete versionDialog; versionDialog = nullptr; }
            versionDialog = new QDialog(this);
            versionDialog->setWindowTitle("历史版本");
            QVBoxLayout* layout = new QVBoxLayout(versionDialog);
            versionListWidget = new QListWidget(versionDialog);
            for (const QJsonValue& v : obj["versions"].toArray()) {
                QJsonObject ver = v.toObject();
                int id = ver["id"].toInt();
                QString author = ver["author"].toString();
                QDateTime ts = QDateTime::fromSecsSinceEpoch(ver["timestamp"].toInt());
                QString text = QString("版本ID:%1 作者:%2 时间:%3").arg(id).arg(author).arg(ts.toString("yyyy-MM-dd hh:mm:ss"));
                QListWidgetItem* item = new QListWidgetItem(text, versionListWidget);
                item->setData(Qt::UserRole, id);
            }
            connect(versionListWidget, &QListWidget::itemClicked, this, &MainWindow::onVersionItemClicked);
            connect(versionListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onVersionItemDoubleClicked);
            layout->addWidget(versionListWidget);
            versionDialog->setLayout(layout);
            versionDialog->exec();
        } else if (obj["type"] == "rollback_result" && obj["docId"].toInt() == currentDocId) {
            handleRollbackResult(obj);
            if (versionDialog) { versionDialog->close(); delete versionDialog; versionDialog = nullptr; }
        } else if (obj["type"] == "user_list" && obj["docId"].toInt() == currentDocId) {
            userListWidget->clear();
            for (const QJsonValue& v : obj["users"].toArray()) {
                userListWidget->addItem(v.toString());
            }
        } else if (obj["type"] == "doc_list") {
            // 弹窗选择或输入文档名
            QDialog dialog(this);
            dialog.setWindowTitle("切换文档");
            dialog.setFixedSize(350, 180);
            dialog.setModal(true);
            QVBoxLayout* layout = new QVBoxLayout(&dialog);
            QLabel* docLabel = new QLabel("选择或输入文档名:", &dialog);
            QComboBox* docCombo = new QComboBox(&dialog);
            docCombo->setEditable(true);
            QJsonArray docs = obj["docs"].toArray();
            for (const QJsonValue& v : docs) {
                QJsonObject d = v.toObject();
                docCombo->addItem(d["title"].toString(), d["id"].toInt());
            }
            docCombo->setEditText(docTitle->text());
            QPushButton* renameBtn = new QPushButton("重命名当前文档", &dialog);
            QHBoxLayout* buttonLayout = new QHBoxLayout();
            QPushButton* okButton = new QPushButton("切换", &dialog);
            QPushButton* cancelButton = new QPushButton("取消", &dialog);
            buttonLayout->addWidget(okButton);
            buttonLayout->addWidget(cancelButton);
            layout->addWidget(docLabel);
            layout->addWidget(docCombo);
            layout->addWidget(renameBtn);
            layout->addStretch();
            layout->addLayout(buttonLayout);
            connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
            connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
            connect(renameBtn, &QPushButton::clicked, this, [this, &dialog, docCombo]() {
                QString newTitle = QInputDialog::getText(&dialog, "重命名文档", "新文档名:");
                if (!newTitle.isEmpty()) {
                    int docId = docCombo->currentData().toInt();
                    QJsonObject req;
                    req["type"] = "rename_doc";
                    req["docId"] = docId;
                    req["newTitle"] = newTitle;
                    netClient->sendJson(req);
                }
            });
            docCombo->setFocus();
            if (dialog.exec() == QDialog::Accepted) {
                QString docName = docCombo->currentText().trimmed();
                if (docName.isEmpty()) {
                    statusBar()->showMessage("文档名不能为空", 2000);
                    return;
                }
                QJsonObject obj;
                obj["type"] = "switch_doc";
                obj["docTitle"] = docName;
                obj["user"] = username;
                netClient->sendJson(obj);
                docTitle->setText(docName);
                editor->clear();
            }
        } else if (obj["type"] == "rename_result") {
            if (obj["ok"].toBool()) {
                statusBar()->showMessage("重命名成功", 2000);
            } else {
                statusBar()->showMessage("重命名失败", 2000);
            }
        }
    });
    connect(netClient, &NetworkClient::errorOccurred, this, [this](const QString& err) {
        statusBar()->showMessage("网络错误: " + err, 3000);
    });

    connect(editor, &QTextEdit::textChanged, this, [this]() {
        if (ignoreTextChanged) return;
        if (!netClient->isConnected()) {
            // 未连接时不发送数据
            return;
        }
        QJsonObject obj;
        obj["type"] = "edit";
        obj["docId"] = currentDocId;
        obj["user"] = username;
        obj["content"] = editor->toPlainText();
        netClient->sendJson(obj);
    });

    versionDialog = nullptr;
    versionListWidget = nullptr;
}

// 析构函数
MainWindow::~MainWindow() {}

// 初始化菜单栏
void MainWindow::setupMenu() {
    QMenu* fileMenu = menuBar()->addMenu("文件");
    newAction = fileMenu->addAction("新建");
    openAction = fileMenu->addAction("打开");
    saveAction = fileMenu->addAction("保存");
    fileMenu->addSeparator();
    connectAction = fileMenu->addAction("连接服务器");
    switchDocAction = fileMenu->addAction("切换文档");
    fileMenu->addSeparator();
    exitAction = fileMenu->addAction("退出");

    QMenu* versionMenu = menuBar()->addMenu("版本");
    versionAction = versionMenu->addAction("版本管理");

    connect(newAction, &QAction::triggered, this, &MainWindow::onNew);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpen);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSave);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);
    connect(versionAction, &QAction::triggered, this, &MainWindow::onShowVersionManager);
    connect(connectAction, &QAction::triggered, this, &MainWindow::onConnectServer);
    connect(switchDocAction, &QAction::triggered, this, &MainWindow::onSwitchDocument);
}

// 初始化主界面布局
void MainWindow::setupLayout() {
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    docTitle = new QLabel("文档标题", this);
    docTitle->setAlignment(Qt::AlignCenter);
    docTitle->setStyleSheet("font-weight: bold; font-size: 18px;");

    editor = new QTextEdit(this);
    editor->setPlaceholderText("在此编辑文档内容...");

    versionList = new QListWidget(this);
    versionList->setFixedWidth(200);
    versionList->addItem("v1.0 - 创建文档");
    versionList->addItem("v1.1 - 编辑内容");
    connect(versionList, &QListWidget::itemClicked, this, &MainWindow::onVersionSelected);

    userListWidget = new QListWidget(this);
    userListWidget->setFixedWidth(120);
    userListWidget->setMinimumWidth(100);
    userListWidget->setMaximumWidth(150);
    userListWidget->setStyleSheet("background:#f0f0f0;font-size:12px;");
    userListWidget->setToolTip("当前在线用户");

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(editor, 1);
    hLayout->addWidget(versionList);
    hLayout->addWidget(userListWidget);

    mainLayout->addWidget(docTitle);
    mainLayout->addLayout(hLayout);

    setCentralWidget(central);
}

// 初始化状态栏
void MainWindow::setupStatusBar() {
    statusLabel = new QLabel("用户: Lily | 版本: v1.1 | 未连接", this);
    statusBar()->addWidget(statusLabel);
}

// 槽函数：新建文档
void MainWindow::onNew() {
    editor->clear();
    docTitle->setText("新建文档");
}
// 槽函数：打开文档
void MainWindow::onOpen() {
    QString fileName = QFileDialog::getOpenFileName(this, "打开文档", "", "文本文档 (*.txt);;所有文件 (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            editor->setPlainText(file.readAll());
            docTitle->setText(QFileInfo(fileName).fileName());
        }
    }
}
// 槽函数：保存文档
void MainWindow::onSave() {
    QString fileName = QFileDialog::getSaveFileName(this, "保存文档", "", "文本文档 (*.txt);;所有文件 (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(editor->toPlainText().toUtf8());
            statusBar()->showMessage("保存成功", 2000);
        }
    }
}
// 槽函数：版本管理弹窗
void MainWindow::onVersion() {
    QMessageBox::information(this, "版本管理", "这里可以实现版本对比、回滚等功能。");
}
// 槽函数：退出程序
void MainWindow::onExit() {
    close();
}
// 槽函数：点击版本列表项
void MainWindow::onVersionSelected(QListWidgetItem* item) {
    QMessageBox::information(this, "版本详情", item->text());
}
// 槽函数：连接服务器弹窗
void MainWindow::onConnectServer() {
    // 创建自定义连接对话框，输入服务器地址、端口和用户名
    QDialog dialog(this);
    dialog.setWindowTitle("连接服务器");
    dialog.setFixedSize(300, 200);
    dialog.setModal(true);
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QLabel* hostLabel = new QLabel("服务器地址:", &dialog);
    QLineEdit* hostEdit = new QLineEdit("127.0.0.1", &dialog);
    QLabel* portLabel = new QLabel("端口:", &dialog);
    QSpinBox* portEdit = new QSpinBox(&dialog);
    portEdit->setRange(1, 65535);
    portEdit->setValue(12345);
    QLabel* userLabel = new QLabel("用户名:", &dialog);
    QLineEdit* userEdit = new QLineEdit(username, &dialog);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* okButton = new QPushButton("连接", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addWidget(hostLabel);
    layout->addWidget(hostEdit);
    layout->addWidget(portLabel);
    layout->addWidget(portEdit);
    layout->addWidget(userLabel);
    layout->addWidget(userEdit);
    layout->addStretch();
    layout->addLayout(buttonLayout);
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    hostEdit->setFocus();
    if (dialog.exec() == QDialog::Accepted) {
        QString host = hostEdit->text().trimmed();
        int port = portEdit->value();
        QString newUsername = userEdit->text().trimmed();
        if (!host.isEmpty() && !newUsername.isEmpty()) {
            username = newUsername;
            netClient->connectToServer(host, port);
        } else {
            QMessageBox::warning(this, "输入错误", "请填写完整的连接信息");
        }
    }
}
// 槽函数：切换文档弹窗
void MainWindow::onSwitchDocument() {
    // 请求文档列表，弹窗将在收到jsonReceived后处理
    if (!netClient->isConnected()) {
        statusBar()->showMessage("未连接到服务器", 2000);
        return;
    }
    QJsonObject req;
    req["type"] = "list_docs";
    netClient->sendJson(req);
}
// 槽函数：弹出历史版本管理
void MainWindow::onShowVersionManager() {
    if (netClient->isConnected()) {
        QJsonObject req;
        req["type"] = "get_versions";
        req["docId"] = currentDocId;
        netClient->sendJson(req);
    } else {
        statusBar()->showMessage("未连接到服务器", 2000);
    }
}
// 槽函数：点击历史版本项，发起回滚请求
void MainWindow::onVersionItemClicked(QListWidgetItem* item) {
    bool ok = false;
    int versionId = item->data(Qt::UserRole).toInt(&ok);
    if (!ok) return;
    if (netClient->isConnected()) {
        QJsonObject req;
        req["type"] = "rollback";
        req["docId"] = currentDocId;
        req["versionId"] = versionId;
        netClient->sendJson(req);
    } else {
        statusBar()->showMessage("未连接到服务器", 2000);
    }
}
// 槽函数：双击历史版本项，弹出内容对比
void MainWindow::onVersionItemDoubleClicked(QListWidgetItem* item) {
    bool ok = false;
    int versionId = item->data(Qt::UserRole).toInt(&ok);
    if (!ok) return;
    if (netClient->isConnected()) {
        QJsonObject req;
        req["type"] = "get_versions";
        req["docId"] = currentDocId;
        netClient->sendJson(req);
    } else {
        statusBar()->showMessage("未连接到服务器", 2000);
        return;
    }
    // 简单diff高亮
    QString currentText = editor->toPlainText();
    QString oldText = item->toolTip();
    QDialog dlg(this);
    dlg.setWindowTitle("内容对比");
    QVBoxLayout* layout = new QVBoxLayout(&dlg);
    QTextBrowser* browser = new QTextBrowser(&dlg);
    browser->setFont(QFont("Consolas", 10));
    QString html;
    int minLen = qMin(currentText.length(), oldText.length());
    for (int i = 0; i < minLen; ++i) {
        if (currentText[i] != oldText[i]) {
            html += QString("<span style='background:yellow'>%1</span>").arg(currentText[i]);
        } else {
            html += currentText[i];
        }
    }
    if (currentText.length() > minLen)
        html += QString("<span style='background:lightgreen'>%1</span>").arg(currentText.mid(minLen).toHtmlEscaped());
    if (oldText.length() > minLen)
        html += QString("<span style='background:pink;text-decoration:line-through'>%1</span>").arg(oldText.mid(minLen).toHtmlEscaped());
    browser->setHtml(html);
    layout->addWidget(browser);
    dlg.setLayout(layout);
    dlg.exec();
}
// 槽函数：处理回滚结果
void MainWindow::handleRollbackResult(const QJsonObject& obj) {
    if (obj["ok"].toBool()) {
        statusBar()->showMessage("回滚成功", 2000);
        // 回滚后自动刷新内容
        if (netClient->isConnected()) {
            QJsonObject req;
            req["type"] = "switch_doc";
            req["docId"] = currentDocId;
            req["user"] = username;
            netClient->sendJson(req);
        }
    } else {
        statusBar()->showMessage("回滚失败", 2000);
    }
} 