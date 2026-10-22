// ================================
// 文件: DemoEvents.h
// 描述: Demo 的事件层 —— 事件名到处理函数的绑定表, 以及本 Demo 的两条事件(拖拽窗口 / 事件委托演示)。
// 说明: 绑定表的内容属"应用侧", 而 framework\dispatch\UIEventHandler 不得反向依赖应用侧
//       (见 framework\dispatch\UIEventHandler.h 的文件头说明) ⇒ 由 Dispatch 在启动期经函数指针取得本表、
//       注入框架侧解析器; 本 Demo 保留两条绑定: 标题栏拖动窗口 与 事件委托演示的父事件(子控件经
//       CTRL_EVENT_ENTRUST_PARENT 委托到它), 其余 event 名一律解析为空绑定(控件无响应);
//       本层不认识窗口句柄: 拖动只把"窗口期望坐标"记进布局的窗口位置记录(WND 控件的 position),
//       窗口的实质移动归框架层(渲染出口取得渲染目标时以该记录为参数结算)。
// ================================

#pragma once
#include <stddef.h>			// size_t(绑定表条目数)
#include "framework/dispatch/UIEventHandler.h"

// ================================
// 命名空间: DemoEvents
// 描述: Demo 事件绑定表与处理器(纯静态; 无实例)
// ================================
namespace DemoEvents
{
	// 函数: GetBindings
	// 描述: 取本 Demo 的事件绑定表(两条: 标题栏拖动窗口 · 事件委托演示的父事件)
	CONST UIEventHandler::EventBinding* GetBindings(size_t* pnCount);

	// 函数: OnDragWindow
	// 描述: 拖动窗口的事件处理器 —— 标题栏按住拖动 = 把窗口期望坐标记进窗口位置记录
	VOID OnDragWindow(VOID* obj, uint64_t oper, uint64_t param, void* data);

	// 函数: OnEntrustProbe
	// 描述: 事件委托演示的父事件处理器 —— 子控件(委托父容器)的点击经父容器的事件到达这里
	VOID OnEntrustProbe(VOID* obj, uint64_t oper, uint64_t param, void* data);
}
