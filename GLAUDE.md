# AutomaticResponse 项目文档

## 项目概述

**项目名称：** AutomaticResponse（自动应答服务端）

**项目定位：** 一个基于 Qt 框架的 TCP 自动应答服务器，用于工业设备通信测试和协议开发调试。

**核心功能：** 通过预设的规则自动识别并响应客户端发送的 16 进制指令，支持即时应答和延迟应答两种模式。

**主要特点：**
- 🚀 支持多客户端同时连接
- 🎯 基于规则的自动应答机制
- ⏱️ 可配置延迟应答功能
- 💾 SQLite 数据持久化
- 🎨 现代化可伸缩 UI 设计
- 🔧 支持三种配置模式切换（DSC/ARC/TG）

---

## 技术栈

### 核心技术
- **编程语言：** C++17
- **GUI 框架：** Qt 5/6 (QWidgets)
- **数据库：** SQLite 3
- **网络通信：** Qt Network (QTcpServer/QTcpSocket)

### Qt 模块依赖
```qmake
QT += core gui network sql widgets
```

### 主要技术特性
- 单例模式（数据库、样式管理）
- 信号槽机制
- 属性动画（UI 动效）
- 正则表达式验证
- 事件过滤器
- Qt 资源系统

---

## 项目结构

```
AutomaticResponse/
├── src/                          # 源代码目录
│   ├── main.cpp                  # 程序入口
│   ├── mainwindow.cpp/h          # 主窗口
│   ├── homepage.cpp/h            # 主页（服务器控制页）
│   ├── widget.cpp/h/ui           # 配置页
│   ├── sidebar.cpp/h             # 侧边栏基类
│   ├── sidebarwidget.cpp/h       # 侧边栏组件
│   ├── sidebaroptionsbutton.cpp/h # 侧边栏按钮
│   ├── inputrulewidget.cpp/h     # 规则输入组件
│   ├── localsqllite.cpp/h        # SQLite 数据库管理
│   ├── customstyle.cpp/h         # 自定义样式管理
│   └── comm.h                    # 公共数据结构定义
├── resources/                    # 资源目录
│   ├── customQss/                # 样式表
│   │   └── mainWindow.css
│   ├── icon_png/                 # 图标资源（40+ PNG 图标）
│   └── res.qrc                   # Qt 资源文件
├── AutomaticResponse.pro         # Qt 项目配置文件
├── .gitignore                    # Git 忽略文件
└── LICENSE                       # 许可证文件
```

---

## 核心功能详解

### 1. TCP 服务器功能

**位置：** `src/homepage.cpp`

**功能特性：**
- 监听指定端口（默认 8088）接收客户端连接
- 支持多客户端同时连接
- 实时显示客户端连接信息（IP、端口、连接状态）
- 支持 16 进制数据通信
- 可视化消息接收与发送监控

**关键代码：**
```cpp
// 启动服务器
void HomePage::on_startButton_clicked() {
    QString ip = ui->ipLineEdit->text();
    quint16 port = ui->portLineEdit->text().toUInt();

    if (tcpServer->listen(QHostAddress(ip), port)) {
        // 服务器启动成功
        ui->receiveTextEdit->append("服务器启动成功...");
    }
}

// 处理新连接
void HomePage::onNewConnection() {
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    clientSockets.append(socket);
    // 连接信号槽处理数据接收
}
```

### 2. 自动应答系统

**核心数据结构：** `src/comm.h`

```cpp
struct AutoReplyRule {
    int id;                      // 规则 ID
    bool isEnabled;              // 是否启用
    QString matchCommand;        // 匹配指令（16 进制）
    QString responseTemplate;    // 应答模板（16 进制）
    QString remarks;             // 备注
    int delayedTime;            // 延迟时间（毫秒）
    bool isDelayEnabled;        // 是否启用延迟应答
    QString timeoutResponse;     // 延时应答内容
};
```

**支持的配置类型：**
- **DSC** (`auto_reply_rules_dsc`) - 配置类型 1
- **ARC** (`auto_reply_rules_arc`) - 配置类型 2
- **TG** (`auto_reply_rules_tg`) - 配置类型 3

**应答流程：**
1. 接收客户端 16 进制数据
2. 遍历当前配置类型的所有规则
3. 匹配 `matchCommand` 字段
4. 立即发送 `responseTemplate`（即时应答）
5. 如果启用延迟应答，在 `delayedTime` 后发送 `timeoutResponse`

**关键代码：**
```cpp
// 自动应答逻辑
void HomePage::onReadyRead() {
    QByteArray data = socket->readAll();
    QString hexString = data.toHex(' ').toUpper();

    // 遍历规则进行匹配
    for (const auto& rule : currentRules) {
        if (rule.isEnabled && rule.matchCommand == hexString) {
            // 发送即时应答
            socket->write(QByteArray::fromHex(
                rule.responseTemplate.remove(' ').toUtf8()));

            // 延迟应答
            if (rule.isDelayEnabled && !rule.timeoutResponse.isEmpty()) {
                QTimer::singleShot(rule.delayedTime, [=]() {
                    socket->write(QByteArray::fromHex(
                        rule.timeoutResponse.remove(' ').toUtf8()));
                });
            }
            break;
        }
    }
}
```

### 3. 规则管理系统

**位置：** `src/inputrulewidget.cpp`

**功能特性：**
- 16 进制输入自动格式化（每 2 位插入空格）
- 输入验证（仅允许 0-9, A-F）
- 延迟时间范围限制（100-200000ms）
- 规则添加、删除、清空
- 实时保存到数据库

**16 进制格式化示例：**
```
输入：AABBCCDD
输出：AA BB CC DD
```

**输入验证器：**
```cpp
QRegularExpressionValidator* hexValidator =
    new QRegularExpressionValidator(
        QRegularExpression("^[0-9A-Fa-f ]*$"), lineEdit);
```

---

## 数据库设计

### 数据库文件
- **文件名：** `replydata.db`
- **位置：** 程序运行目录
- **类型：** SQLite 3
- **管理类：** `LocalSqlLite`（单例模式）

### 数据表结构

三个表结构相同，分别存储不同配置：

```sql
CREATE TABLE IF NOT EXISTS auto_reply_rules_dsc (
    id INTEGER PRIMARY KEY AUTOINCREMENT,   /* 自增主键 */
    is_enabled INTEGER NOT NULL DEFAULT 1,  /* 启用状态 (0/1) */
    match_command TEXT NOT NULL UNIQUE,     /* 匹配指令（唯一） */
    response_template TEXT,                 /* 应答模板 */
    remarks TEXT,                           /* 备注 */
    delayed_time INTEGER,                   /* 延迟时间（毫秒） */
    is_delay_enabled INTEGER NOT NULL DEFAULT 1, /* 延迟启用 (0/1) */
    timeout_response TEXT,                  /* 延时应答内容 */
    created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))
);

-- 另外两个表：
-- auto_reply_rules_arc
-- auto_reply_rules_tg
```

### 数据库操作 API

**文件位置：** `src/localsqllite.h` / `src/localsqllite.cpp`

**读取操作：**
```cpp
// 读取 DSC 配置数据
QVector<AutoReplyRule> readDSCAutoReplyData();

// 读取 ARC 配置数据
QVector<AutoReplyRule> readARCAutoReplyData();

// 读取 TG 配置数据
QVector<AutoReplyRule> readTGAutoReplyData();
```

**写入操作：**
```cpp
// 写入自动应答数据（支持批量写入）
void writeAutoReplyData(const DataName& dataName,
                       const QVector<AutoReplyRule>& data);

// DataName 枚举
enum class DataName {
    DSC,
    ARC,
    TG
};
```

**技术特点：**
- 使用事务保证数据一致性
- `ON CONFLICT` 实现 UPSERT 语法
- 自动初始化数据库（首次运行创建表）
- 程序关闭时自动保存数据

**单例获取：**
```cpp
LocalSqlLite* db = LocalSqlLite::getInstance();
QVector<AutoReplyRule> rules = db->readDSCAutoReplyData();
```

---

## UI 架构

### 主窗口层次结构

```
MainWindow (主窗口)
├── SidebarWidget (侧边栏 - 可伸缩)
│   ├── 主页按钮 (HomePage)
│   ├── 配置按钮 (Widget)
│   ├── 数据库按钮 (未实现)
│   └── 导航按钮 (展开/收缩控制)
└── QStackedWidget (页面堆栈)
    ├── [0] HomePage (主页 - 服务器控制)
    ├── [1] Widget (配置页)
    └── [2] 数据库页 (待实现)
```

### 主页布局（HomePage）

**文件位置：** `src/homepage.cpp:597行`

**布局结构：**

```
┌─────────────────────────────────────────────────────────────────┐
│  左侧区域          │  中间区域              │  右侧区域(可展开)  │
│                    │                        │                    │
│  ┌─────────────┐   │  ┌──────────────────┐  │  ┌──────────────┐ │
│  │服务端配置组 │   │  │  消息接收区(上)  │  │  │  应答规则表  │ │
│  │- IP地址     │   │  │  显示接收消息    │  │  │  ┌─────────┐ │ │
│  │- 端口       │   │  │  和应答记录      │  │  │  │DSC│ARC│TG│ │ │
│  │- 开启/关闭  │   │  └──────────────────┘  │  │  └─────────┘ │ │
│  │- 配置选择   │   │                        │  │              │ │
│  └─────────────┘   │  ┌──────────────────┐  │  │  规则输入组  │ │
│                    │  │  消息发送区(下)  │  │  │  - 匹配指令  │ │
│  ┌─────────────┐   │  │  手动发送消息    │  │  │  - 应答模板  │ │
│  │功能配置组   │   │  └──────────────────┘  │  │  - 延迟设置  │ │
│  │- 自动应答   │   │                        │  │  - 按钮操作  │ │
│  │- 延迟应答   │   │                        │  └──────────────┘ │
│  │- 应答编辑   │   │                        │                    │
│  └─────────────┘   │                        │                    │
│                    │                        │                    │
│  ┌─────────────┐   │                        │                    │
│  │客户端列表   │   │                        │                    │
│  │- IP:Port    │   │                        │                    │
│  │- 状态       │   │                        │                    │
│  └─────────────┘   │                        │                    │
└─────────────────────────────────────────────────────────────────┘
```

### 样式系统

**文件位置：** `resources/customQss/mainWindow.css`

**主题配色：**
```css
/* 主窗口渐变背景 */
MainWindow {
    background: qlineargradient(
        x1:0, y1:0, x2:0, y2:1,
        stop:0 #ECE9E6, stop:1 #FFFFFF
    );
}

/* 侧边栏渐变 */
SidebarWidget {
    background: qlineargradient(
        x1:0, y1:0, x2:0, y2:1,
        stop:0 #14D4FF, stop:1 #B76ADE
    );
    border-radius: 8px;
}

/* 文本框焦点效果 */
QLineEdit:focus, QTextEdit:focus {
    border-bottom: 2px solid
        qlineargradient(stop:0 #fceabb, stop:1 #f8b500);
}
```

### 图标资源

**位置：** `resources/icon_png/`

**主要图标：**
- `shouye.png` - 首页
- `shezhi.png` - 设置
- `shujuku.png` - 数据库
- `daohang.png` - 导航
- `fasong.png` - 发送
- `shanchu.png` - 删除
- `tianjia.png` - 添加
- `qingkong.png` - 清空
- 等 40+ 个图标...

**使用方式：**
```cpp
// 通过 Qt 资源系统加载
QIcon icon(":/icon_png/shouye.png");
button->setIcon(icon);
```

---

## 编译与运行

### 环境要求

- **Qt 版本：** 5.12+ 或 6.x
- **编译器：** 支持 C++17 的编译器
  - GCC 7+
  - Clang 5+
  - MSVC 2017+
- **CMake：** 3.5+ (如果使用 CMake)
- **qmake：** Qt 自带

### 使用 qmake 编译

```bash
# 1. 生成 Makefile
qmake AutomaticResponse.pro

# 2. 编译
make

# 3. 运行
./AutomaticResponse  # Linux/macOS
# 或
AutomaticResponse.exe  # Windows
```

### 使用 Qt Creator

1. 打开 Qt Creator
2. 文件 → 打开文件或项目
3. 选择 `AutomaticResponse.pro`
4. 配置构建套件（Kit）
5. 点击运行按钮（Ctrl+R）

### 发布打包

**Windows:**
```bash
windeployqt AutomaticResponse.exe
```

**Linux:**
```bash
# 使用 linuxdeployqt 工具
linuxdeployqt AutomaticResponse
```

**macOS:**
```bash
macdeployqt AutomaticResponse.app
```

---

## 使用指南

### 1. 启动服务器

1. 在左侧"服务端配置组"输入 IP 地址和端口
   - IP 默认：`127.0.0.1`（本地）
   - 端口范围：`1024-65535`（默认 8088）
2. 点击"开启服务"按钮
3. 查看消息接收区确认启动成功

### 2. 配置自动应答规则

**方法一：通过主页右侧编辑**
1. 点击"应答编辑"按钮展开右侧区域
2. 选择配置类型（DSC/ARC/TG）标签页
3. 在规则输入组填写：
   - **匹配指令：** 例如 `AA BB CC`
   - **应答模板：** 例如 `DD EE FF`
   - **备注：** 规则说明（可选）
   - **延迟时间：** 100-200000ms（可选）
   - **延时应答：** 延迟后发送的内容（可选）
4. 点击"添加"按钮保存规则

**方法二：通过配置页**
1. 点击侧边栏"配置"按钮
2. 进入配置页面操作（旧版 UI）

### 3. 启用自动应答

1. 勾选"自动应答"复选框
2. （可选）勾选"延迟应答"复选框启用延时功能
3. 在左侧选择要使用的配置类型（DSC/ARC/TG）

### 4. 测试连接

**使用客户端工具：**
- TCP 调试工具（如 NetAssist、Hercules）
- 自定义客户端程序
- telnet 命令

**测试步骤：**
1. 客户端连接到服务器 IP:端口
2. 发送 16 进制指令（例如：`AA BB CC`）
3. 观察消息接收区显示
4. 查看自动应答是否正确返回

### 5. 手动发送消息

1. 在中间区域下方"消息发送区"输入 16 进制数据
2. 点击"发送"按钮
3. 数据将发送给所有已连接的客户端

---

## 开发指南

### 代码架构模式

**采用的设计模式：**
1. **单例模式** - `LocalSqlLite`, `CustomStyle`
2. **观察者模式** - Qt 信号槽机制
3. **策略模式** - 不同配置类型的数据处理

### 模块依赖关系

```
main.cpp
  └── MainWindow
      ├── LocalSqlLite (单例) ─────┐
      ├── CustomStyle (单例)       │
      ├── SidebarWidget            │
      │   └── SidebarOptionsButton │
      └── QStackedWidget           │
          ├── HomePage ─────────────┤ (依赖数据库)
          │   ├── InputRuleWidget  │
          │   ├── QTcpServer       │
          │   └── QTableWidget (×3)│
          └── Widget ───────────────┘
```

### 添加新配置类型

**步骤：**

1. **修改数据库（localsqllite.cpp）：**
```cpp
// 1. 添加表创建语句
void LocalSqlLite::initDatabase() {
    // ... 现有代码 ...

    // 添加新表
    query.exec("CREATE TABLE IF NOT EXISTS auto_reply_rules_new (...)");
}

// 2. 添加读取方法
QVector<AutoReplyRule> LocalSqlLite::readNEWAutoReplyData() {
    return readDataFromTable("auto_reply_rules_new");
}

// 3. 修改 writeAutoReplyData() 支持新类型
void LocalSqlLite::writeAutoReplyData(const DataName& dataName, ...) {
    QString tableName;
    switch (dataName) {
        case DataName::NEW:
            tableName = "auto_reply_rules_new";
            break;
        // ...
    }
}
```

2. **修改枚举（comm.h）：**
```cpp
enum class DataName {
    DSC,
    ARC,
    TG,
    NEW  // 添加新类型
};
```

3. **添加 UI 标签页（homepage.cpp）：**
```cpp
// 在构造函数中添加新标签页
QWidget *newTab = new QWidget();
// ... 设置布局和表格 ...
ui->tabWidget->addTab(newTab, "NEW");
```

### 扩展功能建议

**待完善功能：**
1. ✅ 定时上发功能
2. ✅ 数据库页面实现
3. ✅ 日志文件记录
4. ✅ 规则导入/导出
5. ✅ 客户端管理（踢出、禁止）
6. ✅ 数据统计分析

**实现示例（定时上发）：**
```cpp
// 1. 添加 QTimer 成员变量
class HomePage : public QWidget {
    QTimer* uploadTimer;
};

// 2. 初始化定时器
uploadTimer = new QTimer(this);
connect(uploadTimer, &QTimer::timeout, this, &HomePage::onUploadTimeout);

// 3. 启动定时上发
void HomePage::startPeriodicUpload(int intervalMs) {
    uploadTimer->start(intervalMs);
}

// 4. 定时回调
void HomePage::onUploadTimeout() {
    // 发送预设数据给所有客户端
    QByteArray data = QByteArray::fromHex("AA BB CC");
    for (QTcpSocket* socket : clientSockets) {
        socket->write(data);
    }
}
```

---

## API 参考

### LocalSqlLite 类

**文件位置：** `src/localsqllite.h`

**类型：** 单例类

**获取实例：**
```cpp
LocalSqlLite* db = LocalSqlLite::getInstance();
```

**主要方法：**

```cpp
// 读取数据
QVector<AutoReplyRule> readDSCAutoReplyData();
QVector<AutoReplyRule> readARCAutoReplyData();
QVector<AutoReplyRule> readTGAutoReplyData();

// 写入数据
void writeAutoReplyData(const DataName& dataName,
                       const QVector<AutoReplyRule>& data);

// 数据库初始化（自动调用）
void initDatabase();
```

**使用示例：**
```cpp
// 读取数据
LocalSqlLite* db = LocalSqlLite::getInstance();
QVector<AutoReplyRule> rules = db->readDSCAutoReplyData();

// 写入数据
QVector<AutoReplyRule> newRules;
AutoReplyRule rule;
rule.matchCommand = "AA BB CC";
rule.responseTemplate = "DD EE FF";
rule.isEnabled = true;
newRules.append(rule);

db->writeAutoReplyData(DataName::DSC, newRules);
```

### CustomStyle 类

**文件位置：** `src/customstyle.h`

**类型：** 单例类

**获取实例：**
```cpp
CustomStyle* style = CustomStyle::getInstance();
```

**主要方法：**
```cpp
// 加载 QSS 样式表
void loadStyleSheet(QWidget* widget, const QString& qssFilePath);
```

**使用示例：**
```cpp
CustomStyle::getInstance()->loadStyleSheet(
    this,
    ":/customQss/mainWindow.css"
);
```

### InputRuleWidget 类

**文件位置：** `src/inputrulewidget.h`

**用途：** 规则输入组件（可复用）

**信号：**
```cpp
signals:
    // 数据提交信号
    void dataSubmitted(const AutoReplyRule& rule);
```

**槽函数：**
```cpp
public slots:
    // 清空所有输入
    void clearAllInputs();

    // 设置数据（用于编辑）
    void setData(const AutoReplyRule& rule);
```

**使用示例：**
```cpp
// 创建组件
InputRuleWidget* inputWidget = new InputRuleWidget(this);

// 连接信号
connect(inputWidget, &InputRuleWidget::dataSubmitted,
        this, &HomePage::onRuleSubmitted);

// 清空输入
inputWidget->clearAllInputs();
```

---

## 数据格式说明

### 16 进制数据格式

**输入格式：**
- 仅允许字符：`0-9`, `A-F`, `a-f`, 空格
- 自动格式化为每 2 位一组，用空格分隔

**示例：**
```
原始输入：  AABBCCDD
格式化后：  AA BB CC DD

原始输入：  aabbccdd
格式化后：  AA BB CC DD

原始输入：  AA BB CC DD
保持不变：  AA BB CC DD
```

### 数据库存储格式

**匹配指令和应答模板：**
- 存储格式：`AA BB CC DD`（带空格的大写 16 进制）
- 使用时需要去除空格：`AABBCCDD`

**转换示例：**
```cpp
// 读取数据库
QString dbValue = "AA BB CC DD";

// 转换为字节数组发送
QByteArray bytes = QByteArray::fromHex(
    dbValue.remove(' ').toUtf8()
);
socket->write(bytes);

// 接收数据转换为数据库格式
QByteArray receivedData = socket->readAll();
QString hexString = receivedData.toHex(' ').toUpper();
// hexString = "AA BB CC DD"
```

---

## 常见问题（FAQ）

### 1. 服务器无法启动？

**可能原因：**
- 端口被占用
- IP 地址错误
- 防火墙阻止

**解决方法：**
```bash
# Linux/macOS 检查端口占用
sudo lsof -i :8088

# Windows 检查端口占用
netstat -ano | findstr :8088

# 更换端口或关闭占用程序
```

### 2. 客户端连接后无应答？

**检查清单：**
- ✅ 是否勾选"自动应答"复选框
- ✅ 是否选择了正确的配置类型（DSC/ARC/TG）
- ✅ 规则表中是否有匹配的指令
- ✅ 规则是否启用（勾选了"开启"）
- ✅ 客户端发送的数据格式是否正确

**调试方法：**
在消息接收区查看：
- 接收到的数据是什么
- 是否触发了自动应答
- 发送的应答内容是什么

### 3. 数据库文件在哪里？

**位置：** 程序运行目录下的 `replydata.db`

**查找方法：**
```cpp
// 在代码中查看
QString dbPath = QCoreApplication::applicationDirPath()
                 + "/replydata.db";
qDebug() << "Database path:" << dbPath;
```

**手动操作数据库：**
```bash
# 使用 sqlite3 命令行工具
sqlite3 replydata.db

# 查看表
.tables

# 查看数据
SELECT * FROM auto_reply_rules_dsc;
```

### 4. 如何备份规则数据？

**方法一：备份数据库文件**
```bash
cp replydata.db replydata.db.backup
```

**方法二：导出 SQL**
```bash
sqlite3 replydata.db .dump > backup.sql

# 恢复
sqlite3 new_replydata.db < backup.sql
```

### 5. 延迟应答不生效？

**检查：**
1. 是否勾选"延迟应答"功能
2. 规则中"是否启用延迟应答"是否勾选
3. 延迟时间是否设置（100-200000ms）
4. 延时应答内容是否填写

---

## 性能优化建议

### 1. 数据库优化

```cpp
// 批量插入使用事务
void LocalSqlLite::writeAutoReplyData(...) {
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();  // 开启事务

    for (const auto& rule : data) {
        // 执行插入...
    }

    db.commit();  // 提交事务（已实现）
}
```

### 2. 网络优化

```cpp
// 设置 Socket 缓冲区大小
socket->setReadBufferSize(8192);
socket->setWriteBufferSize(8192);

// 启用 Nagle 算法优化
socket->setSocketOption(
    QAbstractSocket::LowDelayOption, 0);
```

### 3. UI 优化

```cpp
// 使用 QTableWidget 虚拟模式（数据量大时）
tableWidget->setUpdatesEnabled(false);
// ... 批量操作 ...
tableWidget->setUpdatesEnabled(true);

// 延迟更新 UI
QTimer::singleShot(100, [=]() {
    // 更新 UI
});
```

---

## 安全注意事项

### 1. 输入验证

当前已实现：
- 16 进制输入验证
- 端口范围验证（1024-65535）
- 延迟时间范围验证

建议增强：
```cpp
// IP 地址格式验证
QRegularExpression ipRegex(
    "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
);

// SQL 注入防护（已使用预编译语句，安全）
query.prepare("INSERT INTO ... VALUES (?, ?, ?)");
query.addBindValue(value);
```

### 2. 网络安全

**建议实施：**
```cpp
// 1. 限制连接数
if (clientSockets.size() >= MAX_CLIENTS) {
    socket->disconnectFromHost();
    return;
}

// 2. IP 白名单
QStringList allowedIPs = {"127.0.0.1", "192.168.1.100"};
if (!allowedIPs.contains(socket->peerAddress().toString())) {
    socket->disconnectFromHost();
}

// 3. 数据包大小限制
if (data.size() > MAX_PACKET_SIZE) {
    socket->disconnectFromHost();
}
```

---

## 测试指南

### 单元测试

建议使用 Qt Test 框架：

```cpp
// 测试数据库操作
class TestLocalSqlLite : public QObject {
    Q_OBJECT
private slots:
    void testReadWrite() {
        LocalSqlLite* db = LocalSqlLite::getInstance();

        QVector<AutoReplyRule> rules;
        AutoReplyRule rule;
        rule.matchCommand = "AA BB";
        rule.responseTemplate = "CC DD";
        rules.append(rule);

        db->writeAutoReplyData(DataName::DSC, rules);

        QVector<AutoReplyRule> readRules = db->readDSCAutoReplyData();
        QCOMPARE(readRules.size(), 1);
        QCOMPARE(readRules[0].matchCommand, QString("AA BB"));
    }
};
```

### 集成测试

**测试客户端代码：**
```cpp
QTcpSocket testSocket;
testSocket.connectToHost("127.0.0.1", 8088);
testSocket.waitForConnected();

// 发送测试数据
QByteArray testData = QByteArray::fromHex("AABBCC");
testSocket.write(testData);
testSocket.waitForBytesWritten();

// 接收应答
testSocket.waitForReadyRead(1000);
QByteArray response = testSocket.readAll();
qDebug() << "Response:" << response.toHex(' ');
```

---

## 贡献指南

### 代码风格

**遵循 Qt 编码规范：**
- 类名使用大驼峰：`MainWindow`
- 方法名使用小驼峰：`onReadyRead()`
- 成员变量使用小驼峰：`tcpServer`
- 槽函数前缀：`on_` (自动连接) 或 `on` (手动连接)

### 提交规范

```bash
# 提交格式
git commit -m "类型: 简短描述

详细说明（可选）

影响范围（可选）"

# 类型说明：
# - feat: 新功能
# - fix: 修复 bug
# - docs: 文档更新
# - style: 代码格式调整
# - refactor: 重构
# - test: 测试相关
# - chore: 构建/工具相关
```

**示例：**
```bash
git commit -m "feat: 添加定时上发功能

- 添加 QTimer 定时器
- 实现周期性数据发送
- 添加 UI 配置选项

影响文件：src/homepage.cpp, src/homepage.h"
```

---

## 许可证

详见 `LICENSE` 文件。

---

## 联系方式

**项目仓库：** （请填写 GitHub/Gitee 地址）

**问题反馈：** （请填写 Issue 地址）

**开发团队：** （请填写团队信息）

---

## 版本历史

### v1.0.0（当前版本）
- ✅ TCP 服务器基础功能
- ✅ 自动应答系统
- ✅ 延迟应答功能
- ✅ 三种配置类型支持（DSC/ARC/TG）
- ✅ SQLite 数据持久化
- ✅ 可伸缩侧边栏 UI
- ✅ 16 进制数据自动格式化

### 未来规划
- 🔲 v1.1.0 - 定时上发功能
- 🔲 v1.2.0 - 数据库管理页面
- 🔲 v1.3.0 - 日志系统
- 🔲 v2.0.0 - 规则导入/导出
- 🔲 v2.1.0 - 客户端管理功能
- 🔲 v2.2.0 - 数据统计分析

---

**最后更新：** 2025-11-19
**文档版本：** 1.0.0
