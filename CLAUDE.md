# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

这是一个名为"AutomaticResponse"（自动响应）的Qt C++桌面应用程序，实现了具有自动响应功能的TCP服务器。应用程序可以接收TCP连接，处理传入的十六进制数据，并根据可配置的指令匹配模式发送预定义的响应。

## 构建系统

- **框架**: Qt 5/6 配合 C++17
- **项目文件**: `AutomaticResponse.pro` (基于qmake)
- **必需的Qt模块**: core, gui, widgets, network

### 构建命令

```bash
# 生成Makefile
qmake AutomaticResponse.pro

# 构建项目
make
# 或在Windows MSVC环境下：
nmake
```

## 架构设计

### 核心组件

1. **Widget类** (`widget.h/cpp`): 主应用窗口和TCP服务器逻辑
   - 继承自QWidget
   - 管理QTcpServer处理客户端连接
   - 实现基于可配置指令匹配的自动响应系统

2. **主应用程序** (`main.cpp`): 标准Qt应用程序入口点

3. **UI定义** (`widget.ui`): Qt Designer UI文件，定义界面布局

### 主要功能

- **TCP服务器**: 在可配置端口(默认8088)监听客户端连接
- **自动响应系统**: 基于表格的指令匹配和响应配置
- **十六进制数据处理**: 所有通信都作为十六进制字符串处理，支持自动格式化
- **延时响应**: 可配置的延时响应，基于定时器执行
- **配置持久化**: 设置保存/加载到`config.json`文件

### 数据流程

1. TCP客户端连接到服务器
2. 传入数据转换为带空格的大写十六进制格式
3. 数据与表格中配置的响应模式进行匹配
4. 如果找到匹配项则发送自动响应
5. 通过QTimer触发可选的延时响应
6. 所有通信都带时间戳记录

### 配置说明

- **config.json**: 以JSON数组格式存储响应表配置
- 表格列：启用/禁用、指令匹配、响应模板、备注、延迟时间、延时应答
- 输入字段强制十六进制验证

## 开发注意事项

- 广泛使用Qt的信号-槽机制进行事件处理
- 实现自定义事件过滤器用于十六进制输入验证
- 表格操作的右键上下文菜单(增加/删除/清空)
- 实时十六进制格式化(每2个字符插入空格)
- 网络通信使用QTcpServer和QTcpSocket类