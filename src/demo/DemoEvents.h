#pragma once
#include <stddef.h>
#include "framework/dispatch/UIEventHandler.h"

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
