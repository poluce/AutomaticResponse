# AutomaticResponse 改进计划

## 📋 目录

- [功能扩展计划](#功能扩展计划)
- [性能优化方向](#性能优化方向)
- [用户体验改进](#用户体验改进)
- [代码质量提升](#代码质量提升)
- [安全性增强](#安全性增强)
- [测试与文档](#测试与文档)
- [未来展望](#未来展望)

---

## 功能扩展计划

### 🎯 高优先级功能（v1.1.0 - v1.3.0）

#### 1. 定时上发功能 ⭐⭐⭐⭐⭐

**当前状态：** 已有 UI 框架（`scheduledReportTimer`），但功能未实现

**实现方案：**

```cpp
// 新增数据结构 comm.h
struct ScheduledReport {
    int id;
    bool isEnabled;              // 是否启用
    QString reportData;          // 上发数据（16进制）
    int intervalMs;              // 上发间隔（毫秒）
    QString remarks;             // 备注
    bool sendToAll;              // 是否发送给所有客户端
    QStringList targetClients;   // 指定客户端列表
};

// HomePage 新增方法
void startScheduledReport();
void stopScheduledReport();
void onScheduledReportTimeout();

// 数据库新增表
CREATE TABLE scheduled_reports (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    is_enabled INTEGER,
    report_data TEXT,
    interval_ms INTEGER,
    remarks TEXT,
    send_to_all INTEGER,
    target_clients TEXT,  -- JSON 数组存储
    created_at TEXT
);
```

**UI 设计：**
- 定时上发配置对话框
- 支持多条定时任务
- 可视化任务列表（显示下次发送时间）
- 支持启用/禁用单个任务
- 支持选择发送目标（全部/指定客户端）

**预期收益：**
- 自动化测试场景支持
- 模拟心跳包发送
- 周期性数据推送

---

#### 2. 数据库管理页面 ⭐⭐⭐⭐⭐

**当前状态：** MainWindow 中已预留位置（index=2），未实现

**实现内容：**

**2.1 数据统计面板**
```cpp
struct DatabaseStats {
    int totalRulesDSC;
    int totalRulesARC;
    int totalRulesTG;
    int enabledRulesCount;
    int totalScheduledReports;
    qint64 databaseSize;
    QString lastModifiedTime;
};

// 显示内容：
- 规则总数统计（各配置类型）
- 数据库文件大小
- 最后修改时间
- 启用规则数量
- 可视化图表（饼图/柱状图）
```

**2.2 数据导入/导出**
```cpp
// 支持格式
- JSON 格式导入/导出
- CSV 格式导入/导出
- XML 格式导入/导出

// 功能
bool exportRulesToJSON(const QString& filePath);
bool importRulesFromJSON(const QString& filePath);
bool exportRulesToCSV(const QString& filePath);
```

**2.3 数据备份与恢复**
```cpp
// 自动备份
- 每日自动备份数据库
- 保留最近 7 天的备份
- 备份文件命名：replydata_backup_YYYYMMDD.db

// 手动备份
void createBackup(const QString& backupPath);
void restoreFromBackup(const QString& backupPath);

// 备份管理界面
- 查看备份列表
- 删除旧备份
- 恢复到指定备份
```

**2.4 数据清理与优化**
```cpp
// 功能
- 删除重复规则
- 清理无效数据
- 数据库 VACUUM 优化
- 重建索引

void removeDuplicateRules();
void optimizeDatabase();
```

**预期收益：**
- 数据管理更便捷
- 数据安全性提升
- 支持数据迁移
- 提高数据库性能

---

#### 3. 高级规则匹配系统 ⭐⭐⭐⭐

**当前限制：** 只支持精确匹配

**改进方案：**

**3.1 模糊匹配**
```cpp
enum MatchType {
    EXACT_MATCH,       // 精确匹配（现有）
    PREFIX_MATCH,      // 前缀匹配
    SUFFIX_MATCH,      // 后缀匹配
    CONTAINS_MATCH,    // 包含匹配
    REGEX_MATCH,       // 正则表达式匹配
    WILDCARD_MATCH     // 通配符匹配（* 和 ?）
};

struct AutoReplyRule {
    // ... 现有字段 ...
    MatchType matchType;      // 新增：匹配类型
    int priority;             // 新增：优先级（多规则匹配时）
};
```

**3.2 条件组合**
```cpp
// 支持多条件匹配
struct MatchCondition {
    QString field;           // "command", "length", "checksum"
    QString operation;       // "==", "!=", ">", "<", "contains"
    QString value;
};

struct AdvancedRule {
    QVector<MatchCondition> conditions;
    QString logicalOperator; // "AND", "OR"
    QString response;
};

// 示例：
// 条件1: 指令前两位 == "AA BB"
// 条件2: 指令长度 > 6
// 逻辑: AND
// 则: 发送应答A
```

**3.3 变量应答**
```cpp
// 支持动态变量
QString responseTemplate = "AA BB {TIMESTAMP} {CLIENT_IP} {CHECKSUM}";

// 内置变量：
{TIMESTAMP}      - 当前时间戳
{DATE}           - 当前日期
{TIME}           - 当前时间
{CLIENT_IP}      - 客户端IP
{CLIENT_PORT}    - 客户端端口
{RECEIVED_DATA}  - 接收到的数据
{CHECKSUM}       - 校验和
{COUNTER}        - 计数器（每次自增）
{RANDOM}         - 随机数

// 实现
QString replaceVariables(const QString& template, QTcpSocket* socket);
```

**预期收益：**
- 支持更复杂的通信协议
- 减少规则配置数量
- 提高匹配灵活性
- 支持动态数据生成

---

#### 4. 消息日志系统 ⭐⭐⭐⭐

**功能概述：**

**4.1 日志记录**
```cpp
struct MessageLog {
    QDateTime timestamp;
    QString clientIP;
    quint16 clientPort;
    QString direction;      // "RECV" / "SEND"
    QString data;           // 16进制数据
    QString matchedRule;    // 匹配的规则ID
    bool isAutoReply;       // 是否自动应答
    QString remarks;
};

// 日志级别
enum LogLevel {
    DEBUG,    // 调试信息
    INFO,     // 一般信息
    WARNING,  // 警告
    ERROR     // 错误
};

// 日志管理
class MessageLogger {
public:
    void log(LogLevel level, const QString& message);
    void logMessage(const MessageLog& log);
    QVector<MessageLog> queryLogs(const QDateTime& start,
                                   const QDateTime& end);
    void exportLogs(const QString& filePath);
    void clearOldLogs(int daysToKeep);
};
```

**4.2 日志查看器**
- 实时日志显示（类似终端）
- 日志过滤（按时间、客户端、方向）
- 日志搜索（关键字、16进制）
- 日志统计（消息数量、应答成功率）
- 日志导出（TXT、CSV、HTML）

**4.3 日志存储**
```sql
CREATE TABLE message_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp TEXT NOT NULL,
    client_ip TEXT,
    client_port INTEGER,
    direction TEXT,
    data TEXT,
    matched_rule_id INTEGER,
    is_auto_reply INTEGER,
    remarks TEXT,
    log_level TEXT
);

-- 索引优化
CREATE INDEX idx_timestamp ON message_logs(timestamp);
CREATE INDEX idx_client ON message_logs(client_ip, client_port);
```

**4.4 日志分析**
```cpp
// 统计分析
struct LogStatistics {
    int totalMessages;
    int receivedCount;
    int sentCount;
    int autoReplyCount;
    int manualSendCount;
    QMap<QString, int> topClients;     // 最活跃客户端
    QMap<int, int> topRules;           // 最常用规则
    double autoReplyRate;              // 自动应答成功率
};

LogStatistics analyzeLogsByTimeRange(QDateTime start, QDateTime end);
```

**预期收益：**
- 问题追踪和调试
- 通信历史查询
- 性能分析
- 合规性记录

---

#### 5. 客户端管理功能 ⭐⭐⭐

**当前状态：** 仅显示客户端列表

**增强功能：**

**5.1 客户端详细信息**
```cpp
struct ClientInfo {
    QString ip;
    quint16 port;
    QDateTime connectTime;
    int messagesSent;
    int messagesReceived;
    qint64 bytesSent;
    qint64 bytesReceived;
    QString lastActivity;
    bool isBlocked;
    QString alias;          // 别名（用户自定义）
    QStringList tags;       // 标签
};

// 显示内容：
- IP:Port
- 连接时长
- 收发消息统计
- 流量统计
- 最后活动时间
- 客户端别名
```

**5.2 客户端操作**
```cpp
// 功能
void kickClient(QTcpSocket* socket);           // 踢出客户端
void blockClient(const QString& ip);           // 封禁IP
void unblockClient(const QString& ip);         // 解封IP
void sendToClient(QTcpSocket* socket, ...);    // 单独发送
void broadcastToAll(const QByteArray& data);   // 广播

// IP 黑名单/白名单
QStringList blackList;
QStringList whiteList;
bool isIPAllowed(const QString& ip);
```

**5.3 客户端分组**
```cpp
struct ClientGroup {
    QString groupName;
    QStringList clientIPs;
    QString description;
};

// 功能
- 创建客户端分组（如：测试组、生产组）
- 按组发送消息
- 按组应用规则
```

**5.4 连接限制**
```cpp
// 配置项
int maxConnections = 100;           // 最大连接数
int maxConnectionsPerIP = 5;        // 单IP最大连接数
int connectionTimeout = 300000;     // 连接超时（5分钟）

// 实现
void enforceConnectionLimits();
void closeIdleConnections();
```

**预期收益：**
- 更好的客户端控制
- 防止恶意连接
- 支持分组管理
- 提高安全性

---

### 🎨 中优先级功能（v1.4.0 - v2.0.0）

#### 6. 协议解析插件系统 ⭐⭐⭐⭐

**设计思路：** 支持自定义协议解析器

**6.1 插件接口**
```cpp
// 插件基类
class IProtocolParser {
public:
    virtual ~IProtocolParser() = default;

    // 解析接收数据
    virtual QVariantMap parse(const QByteArray& data) = 0;

    // 生成发送数据
    virtual QByteArray generate(const QVariantMap& fields) = 0;

    // 验证数据
    virtual bool validate(const QByteArray& data) = 0;

    // 计算校验和
    virtual QString calculateChecksum(const QByteArray& data) = 0;

    // 插件信息
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString description() const = 0;
};

// 插件管理器
class ProtocolPluginManager {
public:
    void loadPlugin(const QString& pluginPath);
    void unloadPlugin(const QString& pluginName);
    IProtocolParser* getParser(const QString& name);
    QStringList availableParsers() const;
};
```

**6.2 内置协议示例**
```cpp
// Modbus RTU 协议解析器
class ModbusRTUParser : public IProtocolParser {
public:
    QVariantMap parse(const QByteArray& data) override {
        QVariantMap result;
        result["address"] = data[0];
        result["function"] = data[1];
        result["crc"] = calculateCRC16(data);
        return result;
    }

    QByteArray generate(const QVariantMap& fields) override {
        QByteArray data;
        data.append(fields["address"].toInt());
        data.append(fields["function"].toInt());
        // ... 添加数据 ...
        data.append(calculateCRC16(data));
        return data;
    }
};

// 自定义协议解析器
class CustomParser : public IProtocolParser {
    // 用户通过 Lua/JavaScript 脚本定义
};
```

**6.3 UI 集成**
- 协议选择下拉框
- 协议字段可视化（解析后显示字段）
- 协议编辑器（生成数据时填写字段）

**预期收益：**
- 支持多种工业协议
- 易于扩展新协议
- 降低配置难度
- 提高通用性

---

#### 7. 脚本引擎支持 ⭐⭐⭐⭐

**设计目标：** 通过脚本实现复杂逻辑

**7.1 脚本语言选择**
```cpp
// 方案1: Lua 集成（轻量级）
#include <lua.hpp>

// 方案2: JavaScript 集成（Qt 内置）
#include <QJSEngine>

// 方案3: Python 集成（功能强大）
#include <Python.h>
```

**推荐：JavaScript（QJSEngine）**
- Qt 原生支持，无需额外依赖
- 语法简单，易于学习
- 性能良好

**7.2 脚本功能**
```javascript
// 示例：自定义应答逻辑
function onDataReceived(data, clientInfo) {
    // 解析数据
    var cmd = data.substring(0, 6);

    // 自定义逻辑
    if (cmd === "AA BB CC") {
        var currentTime = new Date().getTime();
        var response = "DD EE FF " + toHex(currentTime);

        // 发送应答
        sendResponse(response);

        // 延迟发送
        setTimeout(function() {
            sendResponse("11 22 33");
        }, 1000);
    }

    // 记录日志
    log("Received from " + clientInfo.ip + ": " + data);
}

// 示例：数据验证
function validateData(data) {
    // 检查长度
    if (data.length < 8) {
        return false;
    }

    // 检查校验和
    var checksum = calculateChecksum(data.slice(0, -2));
    var receivedChecksum = data.slice(-2);

    return checksum === receivedChecksum;
}

// 示例：动态生成应答
function generateResponse(receivedData) {
    var counter = getGlobalCounter();
    var timestamp = getCurrentTimestamp();

    return "AA BB " + toHex(counter) + " " + toHex(timestamp);
}
```

**7.3 API 提供**
```cpp
// C++ 暴露给 JavaScript 的 API
class ScriptAPI : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE void sendResponse(const QString& data);
    Q_INVOKABLE void log(const QString& message);
    Q_INVOKABLE QString toHex(int value);
    Q_INVOKABLE int fromHex(const QString& hex);
    Q_INVOKABLE QString getCurrentTimestamp();
    Q_INVOKABLE int getGlobalCounter();
    Q_INVOKABLE void setGlobalVariable(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant getGlobalVariable(const QString& key);
};
```

**7.4 脚本编辑器**
- 语法高亮
- 代码补全
- 错误提示
- 调试功能（断点、单步执行）

**预期收益：**
- 极大提高灵活性
- 无需重新编译
- 支持复杂业务逻辑
- 降低开发门槛

---

#### 8. 数据模拟与测试工具 ⭐⭐⭐

**功能概述：**

**8.1 数据生成器**
```cpp
class DataGenerator {
public:
    // 生成随机16进制数据
    QByteArray generateRandomHex(int length);

    // 生成序列数据
    QByteArray generateSequence(int start, int end);

    // 根据模板生成
    QByteArray generateFromTemplate(const QString& pattern);

    // 示例：
    // 模板: "AA BB {RAND:2} {SEQ:2}"
    // 结果: "AA BB 3F 2A 00 01"
};
```

**8.2 压力测试**
```cpp
struct StressTestConfig {
    int clientCount;           // 模拟客户端数量
    int messagesPerSecond;     // 每秒发送消息数
    int testDuration;          // 测试持续时间（秒）
    QString dataPattern;       // 数据模式
    bool randomDelay;          // 随机延迟
};

class StressTester {
public:
    void startTest(const StressTestConfig& config);
    void stopTest();
    StressTestReport getReport();
};

struct StressTestReport {
    int totalMessagesSent;
    int totalMessagesReceived;
    double averageLatency;
    double maxLatency;
    double minLatency;
    int failedMessages;
    double successRate;
};
```

**8.3 协议调试助手**
```cpp
// 16进制编辑器
- 支持16进制/ASCII双显示
- 支持插入、删除、修改字节
- 计算校验和
- 查找/替换

// 数据分析
- 自动识别协议特征
- 显示数据统计（字节分布）
- 数据对比（两条消息的差异）
```

**预期收益：**
- 快速协议调试
- 性能测试
- 数据生成自动化
- 提高测试效率

---

#### 9. 多语言支持（国际化）⭐⭐⭐

**实现方案：**

**9.1 Qt 国际化**
```cpp
// 1. 使用 tr() 包裹所有字符串
QPushButton* button = new QPushButton(tr("Start Server"));

// 2. 生成翻译文件
// lupdate AutomaticResponse.pro
// 生成 .ts 文件

// 3. 翻译
// 使用 Qt Linguist 工具翻译

// 4. 发布
// lrelease AutomaticResponse.pro
// 生成 .qm 文件

// 5. 加载
QTranslator translator;
translator.load(":/i18n/zh_CN.qm");
app.installTranslator(&translator);
```

**9.2 支持语言**
- 简体中文（zh_CN）- 默认
- 英语（en_US）
- 繁体中文（zh_TW）
- 日语（ja_JP）

**9.3 语言切换**
```cpp
// 动态切换语言
void switchLanguage(const QString& locale) {
    qApp->removeTranslator(&currentTranslator);
    currentTranslator.load(":/i18n/" + locale + ".qm");
    qApp->installTranslator(&currentTranslator);

    // 刷新所有UI
    ui->retranslateUi(this);
}
```

**预期收益：**
- 支持国际用户
- 提高产品专业度
- 扩大用户群体

---

### 🔮 低优先级功能（v2.1.0+）

#### 10. 数据可视化 ⭐⭐⭐

**实现内容：**

**10.1 实时图表**
```cpp
// 使用 QCustomPlot 或 QtCharts

// 消息流量图（折线图）
- X轴: 时间
- Y轴: 消息数/秒

// 客户端分布（饼图）
- 显示各客户端消息占比

// 规则使用统计（柱状图）
- 显示各规则触发次数
```

**10.2 网络拓扑图**
```cpp
// 显示服务器和客户端连接关系
- 中心节点：服务器
- 周边节点：客户端
- 连线：连接状态
- 动画：数据传输方向
```

**预期收益：**
- 直观的数据展示
- 便于监控和分析
- 提升用户体验

---

#### 11. 远程管理接口 ⭐⭐

**功能概述：**

**11.1 HTTP API**
```cpp
// 使用 Qt HTTP Server（Qt 6.4+）

// RESTful API 接口
GET  /api/status              // 服务器状态
GET  /api/clients             // 客户端列表
POST /api/rules               // 添加规则
PUT  /api/rules/:id           // 更新规则
DELETE /api/rules/:id         // 删除规则
POST /api/send                // 发送消息
GET  /api/logs                // 查询日志
```

**11.2 WebSocket 接口**
```cpp
// 实时推送
ws://localhost:8089/ws

// 推送事件：
- client_connected
- client_disconnected
- message_received
- message_sent
- rule_triggered
```

**11.3 Web 管理界面**
```html
<!-- 使用 Vue.js + Element UI 开发 -->
- 实时监控面板
- 规则配置
- 客户端管理
- 日志查看
```

**预期收益：**
- 支持远程管理
- 方便集成到其他系统
- 提高自动化程度

---

#### 12. 模块化架构重构 ⭐⭐⭐

**目标：** 将功能拆分为独立模块

**模块划分：**

```
AutomaticResponse/
├── Core/                  # 核心模块
│   ├── TcpServer         # TCP服务器
│   ├── RuleEngine        # 规则引擎
│   └── DatabaseManager   # 数据库管理
├── Plugins/              # 插件模块
│   ├── ProtocolParsers   # 协议解析器
│   └── ScriptEngines     # 脚本引擎
├── UI/                   # 界面模块
│   ├── MainWindow
│   ├── HomePage
│   └── ConfigPage
├── Utils/                # 工具模块
│   ├── Logger
│   ├── DataGenerator
│   └── HexConverter
└── API/                  # 接口模块
    ├── HttpServer
    └── WebSocketServer
```

**预期收益：**
- 代码更清晰
- 易于维护
- 支持插件化
- 提高可测试性

---

## 性能优化方向

### 🚀 网络性能优化

#### 1. 异步 I/O 优化
```cpp
// 当前：同步处理
// 优化：异步处理大量数据

// 使用 QTcpSocket 的异步机制
socket->setReadBufferSize(65536);  // 增加缓冲区

// 批量处理数据
QByteArray buffer;
void HomePage::onClientDataReady() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    buffer.append(socket->readAll());

    // 达到一定大小或超时再处理
    if (buffer.size() >= BATCH_SIZE) {
        processBuffer(buffer);
        buffer.clear();
    }
}
```

#### 2. 多线程处理
```cpp
// 将耗时操作放到线程中
class DataProcessThread : public QThread {
    Q_OBJECT
public:
    void run() override {
        // 处理数据
        // 规则匹配
        // 数据解析
    }
signals:
    void dataProcessed(const QByteArray& result);
};

// 使用线程池
QThreadPool* pool = QThreadPool::globalInstance();
pool->setMaxThreadCount(8);
```

#### 3. 连接池优化
```cpp
// 预分配 Socket 对象
// 重用已断开的连接对象
class SocketPool {
public:
    QTcpSocket* acquire();
    void release(QTcpSocket* socket);
private:
    QVector<QTcpSocket*> available;
    QVector<QTcpSocket*> inUse;
};
```

---

### 💾 数据库性能优化

#### 1. 索引优化
```sql
-- 添加索引
CREATE INDEX idx_match_command ON auto_reply_rules_dsc(match_command);
CREATE INDEX idx_enabled ON auto_reply_rules_dsc(is_enabled);

-- 复合索引
CREATE INDEX idx_enabled_command
ON auto_reply_rules_dsc(is_enabled, match_command);
```

#### 2. 查询优化
```cpp
// 缓存查询结果
class RuleCache {
public:
    void updateCache() {
        cachedRules = db->readDSCAutoReplyData();
        lastUpdateTime = QDateTime::currentDateTime();
    }

    QVector<AutoReplyRule> getRules() {
        if (needsRefresh()) {
            updateCache();
        }
        return cachedRules;
    }

private:
    QVector<AutoReplyRule> cachedRules;
    QDateTime lastUpdateTime;
    int cacheTimeout = 60000; // 60秒
};
```

#### 3. 批量操作优化
```cpp
// 批量插入使用一个事务
void LocalSqlLite::batchInsert(const QVector<AutoReplyRule>& rules) {
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QSqlQuery query;
    query.prepare("INSERT INTO ... VALUES (?, ?, ?)");

    for (const auto& rule : rules) {
        query.addBindValue(rule.field1);
        query.addBindValue(rule.field2);
        query.addBindValue(rule.field3);
        query.exec();
    }

    db.commit();
}
```

#### 4. 数据库连接优化
```cpp
// 使用连接池
class DatabasePool {
public:
    QSqlDatabase getConnection() {
        if (availableConnections.isEmpty()) {
            return createNewConnection();
        }
        return availableConnections.dequeue();
    }

    void releaseConnection(QSqlDatabase db) {
        availableConnections.enqueue(db);
    }

private:
    QQueue<QSqlDatabase> availableConnections;
    int maxConnections = 10;
};
```

---

### 🎨 UI 性能优化

#### 1. 虚拟化列表
```cpp
// QTableWidget 大数据量优化
// 使用 QAbstractItemModel + QTableView
class RuleTableModel : public QAbstractTableModel {
    // 只渲染可见行
    // 支持延迟加载
};
```

#### 2. 减少重绘
```cpp
// 批量更新时禁用重绘
tableWidget->setUpdatesEnabled(false);
for (int i = 0; i < 1000; ++i) {
    tableWidget->insertRow(i);
}
tableWidget->setUpdatesEnabled(true);
```

#### 3. 延迟加载
```cpp
// 日志显示延迟加载
void LogViewer::scrollToBottom() {
    if (needsLoadMore()) {
        QTimer::singleShot(100, [this]() {
            loadMoreLogs(100);
        });
    }
}
```

---

### 📊 内存优化

#### 1. 智能指针使用
```cpp
// 使用 Qt 智能指针
QScopedPointer<Widget> widget;
QSharedPointer<Data> data;

// 替代原始指针
// Widget* widget = new Widget;  // 旧方式
```

#### 2. 对象池
```cpp
// 重用对象，减少内存分配
template<typename T>
class ObjectPool {
public:
    T* acquire() {
        if (pool.isEmpty()) {
            return new T();
        }
        return pool.takeFirst();
    }

    void release(T* obj) {
        obj->reset();
        pool.append(obj);
    }

private:
    QList<T*> pool;
};
```

#### 3. 数据压缩
```cpp
// 日志压缩存储
QByteArray compressLog(const QString& log) {
    return qCompress(log.toUtf8(), 9);
}

QString decompressLog(const QByteArray& compressed) {
    return QString::fromUtf8(qUncompress(compressed));
}
```

---

## 用户体验改进

### 🎨 界面优化

#### 1. 现代化主题
```cpp
// 添加暗色主题
class ThemeManager {
public:
    enum Theme {
        LIGHT,
        DARK,
        AUTO  // 跟随系统
    };

    void applyTheme(Theme theme);

private:
    void loadLightTheme();
    void loadDarkTheme();
};

// 暗色主题样式
QMainWindow {
    background-color: #1e1e1e;
    color: #d4d4d4;
}

QTextEdit {
    background-color: #252526;
    color: #cccccc;
    border: 1px solid #3e3e42;
}
```

#### 2. 响应式布局
```cpp
// 支持窗口大小调整
void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    adjustLayoutForSize(event->size());
}

void adjustLayoutForSize(const QSize& size) {
    if (size.width() < 1024) {
        // 紧凑布局
        sidebar->hide();
        editWidget->hide();
    } else {
        // 标准布局
        sidebar->show();
    }
}
```

#### 3. 图标美化
```cpp
// 使用 SVG 图标（支持缩放）
// 替换 PNG 图标

// 添加图标主题
resources/
├── icons/
│   ├── light/
│   │   ├── home.svg
│   │   ├── settings.svg
│   │   └── database.svg
│   └── dark/
│       ├── home.svg
│       ├── settings.svg
│       └── database.svg
```

#### 4. 动画效果
```cpp
// 平滑过渡动画
QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry");
animation->setDuration(300);
animation->setStartValue(QRect(0, 0, 100, 100));
animation->setEndValue(QRect(0, 0, 300, 300));
animation->setEasingCurve(QEasingCurve::OutCubic);
animation->start(QAbstractAnimation::DeleteWhenStopped);

// 淡入淡出
QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
widget->setGraphicsEffect(effect);
QPropertyAnimation* fadeIn = new QPropertyAnimation(effect, "opacity");
fadeIn->setDuration(500);
fadeIn->setStartValue(0.0);
fadeIn->setEndValue(1.0);
fadeIn->start();
```

---

### 🎯 交互优化

#### 1. 快捷键支持
```cpp
// 添加快捷键
QShortcut* startServerShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
connect(startServerShortcut, &QShortcut::activated,
        this, &HomePage::startServer);

// 快捷键列表
Ctrl+S      - 启动/停止服务
Ctrl+N      - 新建规则
Ctrl+E      - 编辑规则
Ctrl+D      - 删除规则
Ctrl+F      - 查找
Ctrl+L      - 清空日志
Ctrl+,      - 设置
F5          - 刷新
ESC         - 取消
```

#### 2. 拖放支持
```cpp
// 支持拖放文件导入规则
void HomePage::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void HomePage::dropEvent(QDropEvent* event) {
    QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl& url : urls) {
        if (url.toLocalFile().endsWith(".json")) {
            importRulesFromFile(url.toLocalFile());
        }
    }
}
```

#### 3. 右键菜单增强
```cpp
// 添加更多右键菜单选项
QMenu* contextMenu = new QMenu(this);
contextMenu->addAction("复制", this, &HomePage::copySelected);
contextMenu->addAction("粘贴", this, &HomePage::paste);
contextMenu->addSeparator();
contextMenu->addAction("编辑", this, &HomePage::editSelected);
contextMenu->addAction("复制规则", this, &HomePage::duplicateRule);
contextMenu->addAction("启用/禁用", this, &HomePage::toggleRule);
contextMenu->addSeparator();
contextMenu->addAction("删除", this, &HomePage::deleteSelected);
contextMenu->addAction("清空", this, &HomePage::clearAll);
```

#### 4. 搜索和过滤
```cpp
// 规则搜索
void HomePage::onSearchTextChanged(const QString& text) {
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            QString cellText = tableWidget->item(row, col)->text();
            if (cellText.contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        tableWidget->setRowHidden(row, !match);
    }
}

// 高级过滤
struct FilterCriteria {
    bool enabledOnly;
    QString matchCommand;
    QString remarks;
    int minDelayTime;
    int maxDelayTime;
};
```

---

### 💡 智能提示

#### 1. 输入建议
```cpp
// 16进制输入自动补全
class HexCompleter : public QCompleter {
public:
    QStringList suggestions(const QString& prefix) {
        // 常用指令建议
        QStringList common = {
            "AA BB CC",
            "FF FF FF",
            "00 00 00",
            "AA 55 "
        };

        QStringList result;
        for (const QString& cmd : common) {
            if (cmd.startsWith(prefix, Qt::CaseInsensitive)) {
                result.append(cmd);
            }
        }
        return result;
    }
};
```

#### 2. 输入验证提示
```cpp
// 实时验证提示
void InputRuleWidget::validateInput() {
    QString input = lineEdit->text();

    if (!isValidHex(input)) {
        showTooltip("请输入有效的16进制数据", TooltipType::ERROR);
        lineEdit->setStyleSheet("border: 2px solid red;");
    } else {
        lineEdit->setStyleSheet("");
    }
}

// 显示提示气泡
void showTooltip(const QString& message, TooltipType type) {
    QToolTip::showText(
        lineEdit->mapToGlobal(QPoint(0, lineEdit->height())),
        message,
        lineEdit
    );
}
```

#### 3. 向导模式
```cpp
// 新用户引导
class FirstRunWizard : public QWizard {
public:
    FirstRunWizard() {
        addPage(new WelcomePage);
        addPage(new ServerConfigPage);
        addPage(new RuleConfigPage);
        addPage(new CompletePage);
    }
};

// 功能提示
class FeatureTip : public QWidget {
    // 首次使用某功能时显示提示
    void showTip(const QString& feature, const QString& description);
};
```

---

### 📢 通知系统

#### 1. 系统通知
```cpp
// 使用 Qt 系统托盘通知
QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
trayIcon->showMessage(
    "AutomaticResponse",
    "服务器已启动",
    QSystemTrayIcon::Information,
    3000
);
```

#### 2. 应用内通知
```cpp
// 非侵入式通知（类似 Toast）
class NotificationWidget : public QWidget {
public:
    void show(const QString& message, NotificationType type) {
        // 从顶部滑入
        QPropertyAnimation* slideIn = new QPropertyAnimation(this, "pos");
        slideIn->setStartValue(QPoint(x(), -height()));
        slideIn->setEndValue(QPoint(x(), 0));
        slideIn->setDuration(300);
        slideIn->start();

        // 3秒后自动消失
        QTimer::singleShot(3000, [this]() {
            fadeOut();
        });
    }
};
```

---

## 代码质量提升

### 🔧 代码重构

#### 1. 消除代码重复
```cpp
// 当前：重复代码
QVector<AutoReplyRule> HomePage::getTableData(QString name) {
    if (name == "DSC") {
        return readTableData(responseTableDSC);
    } else if (name == "ARC") {
        return readTableData(responseTableARC);
    } else if (name == "TG") {
        return readTableData(responseTableTG);
    }
}

// 优化：使用映射表
QMap<QString, QTableWidget*> tableMap = {
    {"DSC", responseTableDSC},
    {"ARC", responseTableARC},
    {"TG", responseTableTG}
};

QVector<AutoReplyRule> HomePage::getTableData(QString name) {
    return readTableData(tableMap.value(name));
}
```

#### 2. 提取公共方法
```cpp
// 提取工具类
class HexUtils {
public:
    static bool isValidHex(const QString& str);
    static QString formatHex(const QString& str);
    static QByteArray hexToBytes(const QString& hex);
    static QString bytesToHex(const QByteArray& bytes);
    static QString calculateChecksum(const QByteArray& data, ChecksumType type);
};

class ValidationUtils {
public:
    static bool isValidIP(const QString& ip);
    static bool isValidPort(int port);
    static bool isValidDelay(int delay);
};
```

#### 3. 使用设计模式优化
```cpp
// 工厂模式：创建不同类型的规则表格
class TableWidgetFactory {
public:
    static QTableWidget* createRuleTable(RuleType type) {
        QTableWidget* table = new QTableWidget;
        setupCommonProperties(table);
        setupTypeSpecificProperties(table, type);
        return table;
    }
};

// 策略模式：不同的数据处理策略
class IDataProcessor {
public:
    virtual ~IDataProcessor() = default;
    virtual QByteArray process(const QByteArray& input) = 0;
};

class HexProcessor : public IDataProcessor {
    QByteArray process(const QByteArray& input) override;
};

class AsciiProcessor : public IDataProcessor {
    QByteArray process(const QByteArray& input) override;
};

// 观察者模式：事件通知
class EventBus {
public:
    void subscribe(const QString& event, QObject* receiver, const char* slot);
    void publish(const QString& event, const QVariant& data);
};
```

---

### 📝 代码规范

#### 1. 命名规范
```cpp
// 统一命名风格

// 类名：大驼峰
class HomePage;
class InputRuleWidget;

// 方法名：小驼峰
void startServer();
void onClientConnected();

// 成员变量：小驼峰，私有成员加下划线前缀
private:
    QTcpServer* _tcpServer;  // 或 m_tcpServer
    LocalSqlLite* _database;

// 常量：全大写下划线分隔
const int MAX_CONNECTIONS = 100;
const QString DEFAULT_IP = "127.0.0.1";

// 枚举：大驼峰，值全大写
enum class MatchType {
    EXACT_MATCH,
    PREFIX_MATCH,
    REGEX_MATCH
};
```

#### 2. 注释规范
```cpp
/**
 * @brief 启动 TCP 服务器
 * @param ip 监听的 IP 地址
 * @param port 监听的端口号（1024-65535）
 * @return 启动成功返回 true，失败返回 false
 * @throws 无异常抛出
 *
 * 示例：
 * @code
 * if (startServer("127.0.0.1", 8088)) {
 *     qDebug() << "Server started successfully";
 * }
 * @endcode
 */
bool startServer(const QString& ip, quint16 port);

// 行内注释
int timeout = 5000;  // 超时时间（毫秒）
```

#### 3. 错误处理
```cpp
// 统一错误处理机制
class ErrorHandler {
public:
    enum ErrorCode {
        SUCCESS = 0,
        NETWORK_ERROR = 1000,
        DATABASE_ERROR = 2000,
        INVALID_INPUT = 3000
    };

    static void handleError(ErrorCode code, const QString& message) {
        // 记录日志
        Logger::error(QString("[%1] %2").arg(code).arg(message));

        // 显示错误对话框
        QMessageBox::critical(nullptr, "错误", message);

        // 可选：发送错误报告
    }
};

// 使用
if (!db->isOpen()) {
    ErrorHandler::handleError(
        ErrorHandler::DATABASE_ERROR,
        "数据库打开失败"
    );
    return false;
}
```

---

### ✅ 单元测试

#### 1. 测试框架搭建
```cpp
// 使用 Qt Test 框架

// tests/test_localsqllite.cpp
#include <QtTest/QtTest>
#include "localsqllite.h"

class TestLocalSqlLite : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // 测试开始前执行
        db = LocalSqlLite::getInstance();
    }

    void cleanupTestCase() {
        // 测试结束后执行
    }

    void testReadWriteDSC() {
        QVector<AutoReplyRule> rules;
        AutoReplyRule rule;
        rule.matchCommand = "AA BB CC";
        rule.responseTemplate = "DD EE FF";
        rule.isEnabled = true;
        rules.append(rule);

        db->writeAutoReplyData(DataName::DSC, rules);

        QVector<AutoReplyRule> readRules = db->readDSCAutoReplyData();
        QCOMPARE(readRules.size(), 1);
        QCOMPARE(readRules[0].matchCommand, QString("AA BB CC"));
        QCOMPARE(readRules[0].responseTemplate, QString("DD EE FF"));
    }

    void testDuplicateCommand() {
        // 测试唯一性约束
        QVector<AutoReplyRule> rules;

        AutoReplyRule rule1;
        rule1.matchCommand = "AA BB";
        rules.append(rule1);

        AutoReplyRule rule2;
        rule2.matchCommand = "AA BB";  // 重复
        rules.append(rule2);

        db->writeAutoReplyData(DataName::DSC, rules);

        QVector<AutoReplyRule> readRules = db->readDSCAutoReplyData();
        QCOMPARE(readRules.size(), 1);  // 应该只有一条
    }

private:
    LocalSqlLite* db;
};

QTEST_MAIN(TestLocalSqlLite)
#include "test_localsqllite.moc"
```

#### 2. 测试覆盖率
```bash
# 使用 gcov 或 lcov 生成覆盖率报告

# 编译时添加覆盖率标志
QMAKE_CXXFLAGS += --coverage
QMAKE_LFLAGS += --coverage

# 运行测试
./tests

# 生成报告
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

#### 3. 持续集成
```yaml
# .github/workflows/ci.yml
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v2

    - name: Install Qt
      run: |
        sudo apt-get update
        sudo apt-get install -y qt5-default

    - name: Build
      run: |
        qmake AutomaticResponse.pro
        make

    - name: Run Tests
      run: |
        cd tests
        ./run_all_tests.sh

    - name: Upload Coverage
      run: |
        bash <(curl -s https://codecov.io/bash)
```

---

## 安全性增强

### 🔒 网络安全

#### 1. SSL/TLS 加密
```cpp
// 支持加密连接
#include <QSslSocket>

class SecureTcpServer : public QTcpServer {
protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QSslSocket* sslSocket = new QSslSocket(this);

        if (sslSocket->setSocketDescriptor(socketDescriptor)) {
            // 设置证书
            sslSocket->setLocalCertificate("server.crt");
            sslSocket->setPrivateKey("server.key");

            // 开始加密握手
            sslSocket->startServerEncryption();

            addPendingConnection(sslSocket);
        }
    }
};
```

#### 2. 认证机制
```cpp
// 客户端认证
struct AuthRequest {
    QString username;
    QString password;
    QString token;
};

class AuthManager {
public:
    bool authenticate(const AuthRequest& req) {
        // 验证用户名密码
        if (validateCredentials(req.username, req.password)) {
            return true;
        }

        // 或验证 Token
        if (validateToken(req.token)) {
            return true;
        }

        return false;
    }

private:
    bool validateCredentials(const QString& username, const QString& password) {
        // 从数据库查询
        // 密码应该加密存储（bcrypt, PBKDF2等）
        QString hashedPassword = hashPassword(password);
        return db->checkCredentials(username, hashedPassword);
    }

    QString hashPassword(const QString& password) {
        // 使用 QCryptographicHash
        return QString(QCryptographicHash::hash(
            password.toUtf8(),
            QCryptographicHash::Sha256
        ).toHex());
    }
};
```

#### 3. 访问控制
```cpp
// IP 白名单/黑名单
class AccessControl {
public:
    bool isAllowed(const QString& ip) {
        // 黑名单优先
        if (blackList.contains(ip)) {
            return false;
        }

        // 如果有白名单，只允许白名单IP
        if (!whiteList.isEmpty()) {
            return whiteList.contains(ip);
        }

        return true;
    }

    // 支持IP段
    bool isInRange(const QString& ip, const QString& range) {
        // 例如：192.168.1.0/24
        QHostAddress address(ip);
        auto [network, prefix] = parseRange(range);
        return address.isInSubnet(network, prefix);
    }

private:
    QStringList blackList;
    QStringList whiteList;
};
```

#### 4. 防护措施
```cpp
// 防止 DoS 攻击
class DDoSProtection {
public:
    bool checkRateLimit(const QString& ip) {
        qint64 now = QDateTime::currentMSecsSinceEpoch();

        // 清理过期记录
        cleanupOldRecords(now);

        // 检查该IP的请求频率
        int count = requestCounts[ip];
        if (count > MAX_REQUESTS_PER_MINUTE) {
            // 加入黑名单
            blackList.insert(ip);
            return false;
        }

        requestCounts[ip]++;
        return true;
    }

private:
    QMap<QString, int> requestCounts;
    QSet<QString> blackList;
    const int MAX_REQUESTS_PER_MINUTE = 100;
};

// 数据包大小限制
const int MAX_PACKET_SIZE = 65536;  // 64KB

void HomePage::onClientDataReady() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    if (socket->bytesAvailable() > MAX_PACKET_SIZE) {
        qWarning() << "Packet too large, closing connection";
        socket->disconnectFromHost();
        return;
    }

    QByteArray data = socket->readAll();
    processData(data);
}
```

---

### 🛡️ 数据安全

#### 1. 敏感数据加密
```cpp
// 配置文件加密存储
class ConfigEncryption {
public:
    QString encrypt(const QString& plainText, const QString& key) {
        QByteArray encrypted = QAESEncryption::Crypt(
            QAESEncryption::AES_256,
            QAESEncryption::CBC,
            plainText.toUtf8(),
            key.toUtf8()
        );
        return QString(encrypted.toBase64());
    }

    QString decrypt(const QString& encrypted, const QString& key) {
        QByteArray decrypted = QAESEncryption::Decrypt(
            QAESEncryption::AES_256,
            QAESEncryption::CBC,
            QByteArray::fromBase64(encrypted.toUtf8()),
            key.toUtf8()
        );
        return QString(decrypted);
    }
};
```

#### 2. SQL 注入防护
```cpp
// 已实现：使用预编译语句
// 确保所有查询都使用 prepare + bindValue

// ✅ 安全
query.prepare("SELECT * FROM rules WHERE match_command = ?");
query.addBindValue(userInput);

// ❌ 不安全
query.exec("SELECT * FROM rules WHERE match_command = '" + userInput + "'");
```

#### 3. 输入验证与过滤
```cpp
// 严格的输入验证
class InputValidator {
public:
    static bool validateHexInput(const QString& input) {
        // 只允许 0-9, A-F, a-f, 空格
        QRegularExpression regex("^[0-9A-Fa-f ]*$");
        return regex.match(input).hasMatch();
    }

    static bool validateIPAddress(const QString& ip) {
        QRegularExpression ipRegex(
            "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
        );
        return ipRegex.match(ip).hasMatch();
    }

    static QString sanitizeInput(const QString& input) {
        // 移除危险字符
        QString safe = input;
        safe.remove(QRegularExpression("[<>\"';]"));
        return safe;
    }
};
```

---

## 测试与文档

### 📖 文档完善

#### 1. API 文档
```cpp
// 使用 Doxygen 生成

/**
 * @file localsqllite.h
 * @brief SQLite 数据库管理类
 * @author Your Name
 * @date 2025-11-19
 */

/**
 * @class LocalSqlLite
 * @brief 单例模式的 SQLite 数据库管理器
 *
 * 该类负责管理应用程序的 SQLite 数据库连接和操作。
 * 使用单例模式确保全局只有一个数据库连接。
 *
 * @see AutoReplyRule
 * @see DataName
 */
class LocalSqlLite {
public:
    /**
     * @brief 获取单例实例
     * @return LocalSqlLite* 数据库管理器实例指针
     */
    static LocalSqlLite* getInstance();

    // ... 其他方法
};
```

#### 2. 用户手册
```markdown
# AutomaticResponse 用户手册

## 目录
1. 快速开始
2. 功能介绍
3. 配置指南
4. 常见问题
5. 故障排除

## 1. 快速开始

### 1.1 安装

#### Windows
1. 下载安装包 `AutomaticResponse-1.0.0-Windows.exe`
2. 双击运行安装程序
3. 按照向导完成安装

#### Linux
...

### 1.2 首次使用

1. 启动程序
2. 配置服务器IP和端口
3. 点击"开启服务"
4. 添加自动应答规则
...
```

#### 3. 开发文档
- 架构设计文档
- 模块说明文档
- 编码规范文档
- 数据库设计文档

---

### 🧪 测试计划

#### 1. 功能测试
- [ ] TCP 服务器启动/停止
- [ ] 客户端连接/断开
- [ ] 数据接收/发送
- [ ] 自动应答规则匹配
- [ ] 延迟应答功能
- [ ] 规则增删改查
- [ ] 数据库读写
- [ ] 配置切换（DSC/ARC/TG）

#### 2. 性能测试
- [ ] 并发连接测试（100+客户端）
- [ ] 高频消息测试（1000+消息/秒）
- [ ] 内存泄漏测试
- [ ] CPU 占用测试
- [ ] 数据库性能测试

#### 3. 压力测试
- [ ] 长时间运行测试（24小时+）
- [ ] 大数据量测试（10000+规则）
- [ ] 极限连接测试
- [ ] 异常情况测试

#### 4. 兼容性测试
- [ ] Windows 7/8/10/11
- [ ] Ubuntu 18.04/20.04/22.04
- [ ] macOS 10.15+
- [ ] Qt 5.12/5.15/6.x

---

## 未来展望

### 🚀 长期规划（v3.0+）

#### 1. 云端服务
- 云端规则同步
- 多设备协同
- 云端日志存储
- 远程监控和管理

#### 2. AI 增强
- 智能规则推荐
- 异常流量检测
- 自动协议识别
- 数据分析和预测

#### 3. 生态系统
- 插件市场
- 规则模板库
- 社区分享平台
- 在线文档和教程

#### 4. 企业版功能
- 多租户支持
- 权限管理系统
- 审计日志
- 高可用部署
- 负载均衡

---

## 总结

本改进计划涵盖了从功能扩展到性能优化，从用户体验到代码质量的全方位改进建议。

### 优先级建议

**第一阶段（v1.1 - v1.3）：**
1. ✅ 定时上发功能
2. ✅ 数据库管理页面
3. ✅ 消息日志系统
4. ✅ 客户端管理功能

**第二阶段（v1.4 - v2.0）：**
1. ✅ 高级规则匹配
2. ✅ 脚本引擎支持
3. ✅ 协议解析插件
4. ✅ 性能优化

**第三阶段（v2.1+）：**
1. ✅ 数据可视化
2. ✅ 远程管理接口
3. ✅ 模块化重构
4. ✅ AI 功能

### 持续改进
- 定期收集用户反馈
- 跟踪技术发展趋势
- 保持代码质量
- 完善文档和测试

---

**文档版本：** 1.0.0
**最后更新：** 2025-11-19
**维护者：** AutomaticResponse 开发团队
