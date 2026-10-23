#include <windows.h>
#include <stdio.h>

#include "demo/DemoEvents.h"
#include "framework/dispatch/Dispatch.h"
#include "Identity.h"
#include "framework/base/ControlSpec.h"

static CONST UIEventHandler::EventBinding Bindings[] =
{
	{ IDENTITY_EVENT_DRAG_WINDOW,	DemoEvents::OnDragWindow,	'\0',		'\0' },
	{ IDENTITY_EVENT_ENTRUST_PROBE,	DemoEvents::OnEntrustProbe,	'\0',		'\0' },
};

// 函数: GetBindings
// 描述: 取本 Demo 的事件绑定表(两条: 标题栏拖动窗口 · 事件委托演示的父事件)
CONST UIEventHandler::EventBinding* DemoEvents::GetBindings(size_t* pnCount)
{
	if (pnCount != NULL)
	{
		*pnCount = sizeof(Bindings) / sizeof(Bindings[0]);
	}
	return Bindings;
}

// 函数: OnDragWindow
// 描述: 拖动窗口的事件处理器 —— 标题栏按住拖动 = 把窗口期望坐标记进窗口位置记录
VOID DemoEvents::OnDragWindow(VOID* obj, uint64_t oper, uint64_t param, void* data)
{
	UNREFERENCED_PARAMETER(obj);
	UNREFERENCED_PARAMETER(data);

	if (OperCode::HasCommon(oper, OPER_COMMON_HOLD) == FALSE)
	{
		return;
	}
	CtrlInfo* pWnd = Dispatch::GetLayout().GetLayoutBase(CTRL_TYPE_WND);
	if (pWnd == NULL)
	{
		return;
	}
	pWnd->position = UnpackWndPos(param);
}

// 函数: OnEntrustProbe
// 描述: 事件委托演示的父事件处理器 —— 子控件(委托父容器)的点击经父容器的事件到达这里
VOID DemoEvents::OnEntrustProbe(VOID* obj, uint64_t oper, uint64_t param, void* data)
{
	UNREFERENCED_PARAMETER(param);
	UNREFERENCED_PARAMETER(data);

	if (OperCode::HasCommon(oper, OPER_COMMON_CLICKED) == FALSE)
	{
		return;
	}
	{
		CtrlInfo* pCtrl = (CtrlInfo*)obj;
		printf(u8"[%s] 委托事件到达: 控件 id=%d label=%s(处理器来自父容器的事件)\n",
			IDENTITY_LOG_PREFIX,
			(pCtrl != NULL) ? pCtrl->id : -1,
			(pCtrl != NULL && pCtrl->label != NULL) ? pCtrl->label : "?");
	}
}
