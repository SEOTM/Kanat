// ================================
// 文件: DemoEvents.cpp
// 描述: Demo 事件层实现 —— 绑定表定义与两条事件(标题栏拖动窗口 / 事件委托演示)的实现。
// 说明: 本层不持有也不读取任何窗口句柄: 标题栏控件把"窗口期望坐标"经事件回调的 param 通用槽带来,
//   本层把它记进布局的窗口位置记录(WND 控件的 position) —— 窗口的实质移动由框架层在下次取得该窗口
//   渲染目标时(渲染能力表的 AcquireWnd)以该记录为参数结算, 本层不调任何窗口 API。
//   可拖动与否由事件绑定表达: 未绑定本事件的窗口, 标题栏上按住不会产生位置记录 ⇒ 窗口不动。
// ================================

// ================================
// 区域: 文件包含区
// ================================
#include <windows.h>				// UNREFERENCED_PARAMETER 等基础宏
#include <stdio.h>					// printf(委托演示的到达自证)

#include "demo/DemoEvents.h"
#include "framework/dispatch/Dispatch.h"		// 本场景布局(窗口位置记录的宿主)
#include "Identity.h"				// 项目身份(事件名 / 日志前缀; 派生新项目时改此处)
#include "framework/base/ControlSpec.h"		// 操作码(OperCode)与公共码常量 + 窗口期望坐标打包/解包

// ================================
// 区域: 事件绑定表定义区
// 说明: 表内容 = 唯一真源(事件名 → 处理函数); 框架侧解析器只存本表指针。
//       命名规则: 完整名(chDigitLo == '\0')按精确匹配; 前缀家族(chDigitLo != '\0')按前缀 + 一位数字匹配。
// ================================
static CONST UIEventHandler::EventBinding Bindings[] =
{
	// 事件名					处理函数					数字下界	数字上界(完整名匹配 ⇒ 两者均为 '\0')
	{ IDENTITY_EVENT_DRAG_WINDOW,	DemoEvents::OnDragWindow,	'\0',		'\0' },
	{ IDENTITY_EVENT_ENTRUST_PROBE,	DemoEvents::OnEntrustProbe,	'\0',		'\0' },
};

// ================================
// 区域: 公开接口实现区
// ================================

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

	// 仅按住期间记录(按住 = 电平位): 未按住时参数槽无坐标, 本处理器无事可做
	if (OperCode::HasCommon(oper, OPER_COMMON_HOLD) == FALSE)
	{
		return;
	}
	// 更新窗口位置记录: 记录在布局数据上 ⇒ 未绑定本事件的窗口不会走到这里(不可拖动)
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

	// 只认完整单击: 本处理器是委托链的落点, 其余操作码(激活/按住等)一律不记录
	if (OperCode::HasCommon(oper, OPER_COMMON_CLICKED) == FALSE)
	{
		return;
	}
	// 到达自证: 打印触发本事件的控件(委托者)身份 —— 处理器被调用即证明"事件已绑至父控件的事件"
	{
		CtrlInfo* pCtrl = (CtrlInfo*)obj;
		printf(u8"[%s] 委托事件到达: 控件 id=%d label=%s(处理器来自父容器的事件)\n",
			IDENTITY_LOG_PREFIX,
			(pCtrl != NULL) ? pCtrl->id : -1,
			(pCtrl != NULL && pCtrl->label != NULL) ? pCtrl->label : "?");
	}
}
