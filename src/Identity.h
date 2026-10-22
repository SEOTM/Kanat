// ================================
// 文件: Identity.h
// 描述: 本工程的项目身份常量(唯一真源) —— 从本工程派生新项目时, 改本文件即完成代码侧改名。
// 说明: 框架层(framework / common)不含任何项目身份, 身份只出现在场景层与构建定义里;
//       集中于此是为了让"派生新项目"的代码侧改动收敛到一处(构建侧见 CMakeLists 顶部两行,
//       tools\build.ps1 已与工程名解耦); 派生清单与步骤见 docs\DERIVE.md。
//       注: 日志前缀(形如 [DEMO])与 src\demo\ 目录名不属功能身份, 派生时可选改动。
// ================================

#pragma once
#include <minwindef.h>

// ================================
// 区域: 布局文档身份
// ================================
constexpr LPCSTR IDENTITY_DOC_NAME = "Demo.json";		// 缺省布局文档文件名(exe 同目录; 构建期生成同名 .bin 并投放)
constexpr LPCSTR IDENTITY_DOC_ENV = "DEMO_DOC";		// 覆盖布局文档文件名的环境变量(未设/取值过长 ⇒ 用缺省名)

// ================================
// 区域: 运行期开关(环境变量)
// ================================
constexpr LPCSTR IDENTITY_ENV_AUTOCLOSE = "DEMO_AUTOCLOSE_MS";	// 启动后 N 毫秒自动退出(无人值守验证)

// ================================
// 区域: 应用身份
// 说明: 应用名是窗口身份的派生源 —— 窗口类名 = <APPNAME>_IMGUI_CLASS, 窗口标题默认 = APPNAME
//   (由 framework\render\imgui\ImGuiHost::Start 按此口径构造); 窗口类名须在"进程外"也保持唯一(同名窗口类会与
//   同机其它程序冲突) ⇒ 派生新项目务必改本项。
// ================================
constexpr LPCSTR IDENTITY_APP_NAME = "Demo";		// 应用名(窗口类名与标题由它派生)
constexpr LPCWSTR IDENTITY_ERR_TITLE = L"GenUI Demo 错误";	// 启动期错误弹窗标题

// ================================
// 区域: 日志身份
// 说明: 日志前缀经 DispatchConfig.szLogPrefix 交给调度层, 每行打印为 "[DEMO] ";
//   前缀不属功能身份, 派生新项目时可选改动(改后日志行数不变, 仅前缀文本变)。
// ================================
constexpr LPCSTR IDENTITY_LOG_PREFIX = "DEMO";		// 调度层日志前缀(行首方括号内的文本)

// ================================
// 区域: 事件名
// 说明: 事件名是"布局文档 event 字段 ↔ 绑定表"的成对契约 ⇒ 改名须同时改布局文档, 只改一侧即静默失绑。
// ================================
constexpr LPCSTR IDENTITY_EVENT_DRAG_WINDOW = "DEMO_DRAG_WINDOW";		// 标题栏按住拖动窗口
constexpr LPCSTR IDENTITY_EVENT_ENTRUST_PROBE = "DEMO_ENTRUST_PROBE";	// 委托演示: 容器的父事件(子控件经委托到达)
