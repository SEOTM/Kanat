#include <windows.h>

#include "demo/DemoEvents.h"
#include "framework/dispatch/Dispatch.h"
#include "Identity.h"
#include "framework/base/ControlSpec.h"

static CONST UIEventHandler::EventBinding Bindings[] =
{
	{ IDENTITY_EVENT_DRAG_WINDOW,	DemoEvents::OnDragWindow,	'\0',		'\0' },
};

// 函数: GetBindings
// 描述: 取本 Demo 的事件绑定表(一条: 标题栏拖动窗口)
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
