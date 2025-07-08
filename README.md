# HappyLock 协同文档系统

## 项目简介
HappyLock 是一个基于 C++/Qt 的实时协同文档编辑系统，支持多用户同时编辑、文档版本管理、历史回滚等功能。后端采用 Boost.Asio 实现高性能 TCP 服务器，前端基于 Qt 提供现代化桌面界面。适合团队协作、教学演示、代码/文档协同编辑等场景。

## 功能特性
- 多用户实时协同编辑同一文档
- 文档版本管理与历史回滚
- 支持文档重命名、切换、创建
- 在线用户列表实时显示
- 断线重连与异常处理
- 现代化桌面UI，支持Windows平台

## 目录结构
```
happyLock/
├── backend/                # 后端服务端代码
│   ├── model/              # 文档与版本数据模型
│   ├── server/             # 服务器主逻辑与会话管理
│   ├── service/            # 协同服务与广播
│   ├── session.cpp/.h      # 通用会话类
│   └── CMakeLists.txt      # 后端构建脚本
├── frontend/               # 前端Qt桌面客户端
│   ├── MainWindow.cpp/.h   # 主窗口与UI逻辑
│   ├── NetworkClient.cpp/.h# 网络通信客户端
│   ├── main.cpp            # 前端入口
│   ├── CMakeLists.txt      # 前端构建脚本
│   └── ...                 # 其它UI资源
├── sqlite3.c/.h            # sqlite3数据库源码（本地依赖）
├── CMakeLists.txt          # 项目根构建脚本
└── README.md               # 项目说明文档
```

## 编译与运行
### 依赖环境
- C++17 编译器（推荐 MSVC 2019+/MinGW 64）
- [Qt 6.x](https://www.qt.io/download)（建议6.5及以上，需Widgets/Network模块）
- [Boost 1.70+](https://www.boost.org/)（仅后端，需system模块）
- [sqlite3](https://www.sqlite.org/index.html)（已集成源码）
- CMake 3.16 及以上

### 编译步骤
1. **准备依赖**
   - 安装 Qt 并配置环境变量（如 `CMAKE_PREFIX_PATH`）。
   - 安装 Boost 并配置 `BOOST_ROOT`。
   - 确保 `sqlite3.c/.h` 在项目根目录。
2. **生成构建文件**
   ```sh
   mkdir build
   cd build
   cmake ..
   ```
3. **编译项目**
   ```sh
   cmake --build .
   ```
4. **运行后端服务器**
   ```sh
   ./backend/server/happyLockServer
   ```
5. **运行前端客户端**
   ```sh
   ./frontend/frontend
   ```

> Windows 下建议用 Qt Creator 或 Visual Studio 打开 CMake 工程直接编译。

## 使用说明
1. 启动后端服务器（默认监听12345端口）。
2. 启动前端客户端，点击“连接服务器”，输入服务器地址（如127.0.0.1）、端口、用户名。
3. 编辑文档，所有在线用户实时同步。
4. 可切换/新建/重命名文档，历史版本可回滚。
5. 支持多客户端同时连接同一服务器。

## 主要模块说明
- **backend/model/**：文档、版本数据结构与数据库操作
- **backend/server/**：TCP服务器、会话管理、消息分发
- **backend/service/**：协同服务、广播、用户列表
- **frontend/**：Qt桌面UI、网络通信、用户交互

## 协作开发建议
- 代码已详细注释，便于二次开发和维护
- 推荐使用分支开发新特性，合并前请确保通过编译和基本测试
- 数据库文件默认 `happylock.db`，可在 `DocumentRepository` 构造参数中修改
- 如需支持更多协同算法，可扩展 `ICollabAlgorithm` 接口

## 常见问题
- **Qt/Boost找不到？**
  - 检查 `CMAKE_PREFIX_PATH` 和 `BOOST_ROOT` 是否正确
- **端口被占用？**
  - 修改服务器启动端口或释放端口
- **中文乱码？**
  - 确保源码文件UTF-8编码，Qt项目设置编码一致

## 许可证
本项目仅供学习和交流，禁止用于商业用途。

---
如有问题或建议，欢迎 issue 或 PR！