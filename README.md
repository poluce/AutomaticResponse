# AutomaticResponse - Qt 自动应答系统

## 项目概述

AutomaticResponse 是一个基于 Qt 框架开发的桌面应用程序，提供 TCP 服务器监听和自动应答功能。系统支持规则匹配、延迟响应和多客户端连接，适用于自动化客服、聊天机器人等场景。

## 功能特性

### 核心功能
- **TCP 服务器**：监听客户端连接，支持多客户端并发
- **自动应答规则**：基于字符串匹配的自动响应
- **延迟响应**：可配置延迟时间的定时响应
- **规则管理**：完整的增删改查（CRUD）操作
- **数据持久化**：使用 SQLite 数据库存储规则配置

### 规则类型
系统支持三种类型的自动应答规则：
- **DSC**：Discord 自动应答
- **ARC**：Arc 相关自动应答  
- **TG**：Telegram 自动应答

### 用户界面
- 侧边栏导航
- 规则管理界面
- 连接状态监控
- 响应日志查看

## 技术栈

- **编程语言**: C++17
- **框架**: Qt 6.x (Core, GUI, Network, SQL)
- **数据库**: SQLite 3
- **构建系统**: qmake
- **操作系统**: Windows/Linux/macOS (跨平台)

## 项目结构

```
AutomaticResponse/
├── AutomaticResponse.pro      # qmake 项目文件
├── src/                       # 源代码目录
│   ├── comm.h                # 公共数据结构定义
│   ├── localsqllite.h/cpp    # 数据库操作类
│   ├── widget.h/cpp/ui       # 主功能界面
│   ├── mainwindow.h/cpp      # 主窗口
│   ├── sidebar.h/cpp         # 侧边栏
│   ├── homepage.h/cpp        # 主页
│   ├── inputrulewidget.h/cpp # 规则输入界面
│   └── main.cpp              # 程序入口
├── resources/                # 资源文件
│   └── res.qrc              # Qt 资源文件
└── README.md                # 本文档
```

## 构建指南

### 环境要求
- Qt 6.x 开发环境
- C++17 兼容的编译器
- qmake 构建工具

### 构建步骤

#### Windows (MinGW/MSVC)
```bash
# 1. 打开 Qt Creator 或使用命令行
# 2. 生成 Makefile
qmake AutomaticResponse.pro

# 3. 编译
mingw32-make  # MinGW
# 或
nmake         # MSVC

# 4. 运行
./debug/AutomaticResponse.exe  # Debug 版本
./release/AutomaticResponse.exe # Release 版本
```

#### Linux/macOS
```bash
# 1. 生成 Makefile
qmake AutomaticResponse.pro

# 2. 编译
make

# 3. 运行
./AutomaticResponse
```

### 依赖项
项目依赖以下 Qt 模块：
- QtCore
- QtGui  
- QtWidgets
- QtNetwork
- QtSql

在 `.pro` 文件中已配置：
```qmake
QT += core gui network sql
CONFIG += c++17
```

## 使用说明

### 首次运行
1. 启动应用程序
2. 系统会自动创建 SQLite 数据库文件
3. 数据库文件位置：`用户目录/.automaticresponse/rules.db`

### 配置自动应答规则
1. 点击侧边栏的"规则管理"
2. 在规则输入界面配置：
   - **匹配指令**：客户端发送的触发指令
   - **响应模板**：自动回复的内容
   - **延迟时间**：响应延迟（毫秒）
   - **延迟应答**：延迟后发送的内容
   - **备注**：规则说明

3. 点击"设置"按钮保存规则

### 启动 TCP 服务器
1. 在主界面点击"连接"按钮
2. 服务器默认监听端口：`8080`（需查看代码确认）
3. 连接状态显示在界面底部

### 客户端连接
客户端可以通过 TCP 连接到服务器：
```bash
# 示例：使用 telnet 测试
telnet localhost 8080
```

## 数据库设计

### 表结构
系统使用以下数据库表存储规则：

#### auto_reply_rules 表
| 字段名 | 类型 | 说明 |
|--------|------|------|
| id | INTEGER | 主键，自增 |
| is_enabled | BOOLEAN | 是否启用 |
| match_command | TEXT | 匹配指令 |
| response_template | TEXT | 响应模板 |
| remarks | TEXT | 备注 |
| delayed_time | INTEGER | 延迟时间（毫秒） |
| is_delay_enabled | BOOLEAN | 是否启用延迟 |
| timeout_response | TEXT | 超时响应 |

### 数据访问
通过 `LocalSqlLite` 单例类访问数据库：
```cpp
// 读取 DSC 规则
QVector<AutoReplyRule> rules = LocalSqlLite::instance()->readDSCAutoReplyData();

// 写入规则
LocalSqlLite::instance()->writeAutoReplyData(
    LocalSqlLite::DataName::DSC, 
    rules
);
```

## 开发指南

### 代码架构
- **MVC 模式**：界面、业务逻辑、数据存储分离
- **单例模式**：数据库连接管理
- **信号槽机制**：Qt 事件驱动

### 添加新功能
1. **添加新规则类型**：
   - 在 `LocalSqlLite::DataName` 枚举中添加新类型
   - 实现对应的读写方法
   - 更新数据库表结构

2. **修改界面**：
   - 编辑 `.ui` 文件或代码中的界面组件
   - 使用 Qt Designer 可视化编辑

3. **扩展网络功能**：
   - 修改 `Widget` 类中的网络处理逻辑
   - 添加新的信号槽连接

### 调试技巧
1. **查看网络通信**：
   ```cpp
   // 在 onClientDataReady() 中添加调试输出
   qDebug() << "Received data:" << data;
   ```

2. **数据库调试**：
   ```cpp
   // 检查数据库连接
   if (!db.isOpen()) {
       qDebug() << "Database not open!";
   }
   ```

## 常见问题

### 编译问题
**Q1: 找不到 Qt 模块**
```
错误：QT += network sql 未找到
```
**解决方案**：
- 确认 Qt 安装包含 network 和 sql 模块
- 检查环境变量 `QTDIR` 设置

**Q2: C++17 特性不支持**
```
错误：constexpr if 需要 C++17
```
**解决方案**：
- 更新编译器版本
- 在 `.pro` 文件中确保 `CONFIG += c++17`

### 运行时问题
**Q1: 数据库无法创建**
```
错误：无法打开数据库
```
**解决方案**：
- 检查用户目录写入权限
- 确认 SQLite 驱动可用

**Q2: TCP 服务器无法启动**
```
错误：地址已被占用
```
**解决方案**：
- 更改监听端口
- 关闭占用端口的其他程序

## 性能优化建议

### 网络层优化
1. **连接池管理**：实现客户端连接池
2. **异步处理**：使用 QtConcurrent 处理大量请求
3. **心跳检测**：添加连接健康检查

### 数据库优化
1. **索引优化**：为常用查询字段添加索引
2. **批量操作**：实现规则的批量导入/导出
3. **缓存机制**：缓存常用规则到内存

### 界面优化
1. **懒加载**：大量数据时使用分页加载
2. **响应式设计**：适配不同分辨率
3. **主题支持**：添加深色/浅色主题

## 安全注意事项

### 网络安全
1. **输入验证**：验证客户端输入数据
2. **连接限制**：限制最大连接数
3. **IP 过滤**：实现 IP 白名单/黑名单

### 数据安全
1. **敏感信息**：避免在规则中存储密码等敏感信息
2. **数据备份**：定期备份数据库
3. **访问控制**：添加简单的身份验证

## 贡献指南

### 代码规范
1. **命名约定**：
   - 类名：大驼峰，如 `AutoReplyRule`
   - 变量名：小驼峰，如 `matchCommand`
   - 常量：全大写，如 `MAX_CONNECTIONS`

2. **代码格式**：
   - 使用 4 空格缩进
   - 大括号换行风格
   - 添加必要的注释

### 提交更改
1. Fork 本仓库
2. 创建功能分支
3. 提交更改并添加测试
4. 创建 Pull Request

## 许可证

本项目基于 [MIT License](LICENSE) 开源。

## 联系方式

- **项目仓库**: https://github.com/poluce/AutomaticResponse.git
- **问题反馈**: 请在 GitHub Issues 中提交

## 更新日志

### v1.0.0 (初始版本)
- 基本 TCP 服务器功能
- 自动应答规则管理
- SQLite 数据存储
- Qt 图形界面

---

*最后更新: 2024-01-01*