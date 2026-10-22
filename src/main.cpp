// ================================
// 文件: main.cpp (Kanat 暗渠框架 进程入口)
// 描述: 只做胶水: 取宿主接口表 → 调度层初始化 → 宿主启动(默认几何建窗 + 渲染设施) → 主循环 → 成对释放。
// 说明: 单窗口模式:
//   - 调度层 = framework\dispatch\Dispatch(事件绑定注入 / 场景装配 / 投影配色 / 每帧流程 / 退出判据);
//   - 渲染宿主 = framework\render\imgui\ImGuiHost(窗口类与建窗 / ImGui 与 D3D11 设施 / 消息循环 / 窗口过程);
//   - 事件绑定表 = demo\DemoEvents::GetBindings(经函数指针交给 Dispatch::Init)。
//   界面 = 一份布局对象的声明式渲染(结构件 WND/TITLE/CNTR + 内容控件); 事件 = 标题栏按住拖动窗口
//   与事件委托演示; 无编辑器能力(不增删 / 不改类型 / 不写盘 / 无文件对话框)。
// ================================

// ================================
// 区域: 文件包含区
// ================================
#include <windows.h>

#include "Identity.h"			// 项目身份(应用名 / 文档名 / 环境开关 / 日志前缀 / 错误标题)
#include "demo/DemoEvents.h"					// 事件绑定表
#include "framework/dispatch/Dispatch.h"		// 调度层
#include "framework/render/imgui/ImGuiHost.h"	// 渲染宿主

// ================================
// 区域: 进程入口实现区
// ================================

// 函数: main
// 描述: 暗渠框架 Kanat 程序入口(取宿主表 → 调度初始化 → 宿主启动 → 主循环 → 释放)
int main(int, char**)
{
	// 1. 取宿主接口表: 调度层只经 HostApi 访问宿主(具体宿主由本入口选择)
	CONST HostApi* pHost = ImGuiHost::Register();
	// 2. 组装调度配置: 身份取自 Identity.h 常量; 亮暗默认跟随系统, 单窗口模式
	DispatchConfig cfg{};
	cfg.szDocName = IDENTITY_DOC_NAME;
	cfg.szDocEnv = IDENTITY_DOC_ENV;
	cfg.szAutoCloseEnv = IDENTITY_ENV_AUTOCLOSE;
	cfg.szLogPrefix = IDENTITY_LOG_PREFIX;
	cfg.dwWndCount = 1;
	cfg.bDarkFollowSystem = TRUE;
	// 3. 调度层初始化: 失败即弹窗并返回(Dispatch 内不弹窗, 错误只以返回值上报)
	if (pHost == NULL || !Dispatch::Init(pHost, cfg, DemoEvents::GetBindings))
	{
		MessageBoxW(NULL, L"应用初始化失败(布局文档载入或事件绑定注入失败)!",
			IDENTITY_ERR_TITLE, MB_OK | MB_ICONERROR);
		return 1;
	}
	// 4. 字号: 建面字号由布局参考量决定(与场景几何同源); 调度层不引用具体宿主 ⇒ 由本入口交给宿主登记
	ImGuiHost::SetFontSize(Dispatch::GetFontSize());
	// 5. 渲染宿主启动: 默认几何建窗 + 建立渲染设施(失败时宿主内部已回退)
	if (!pHost->Start(Dispatch::GetWndCount(), IDENTITY_APP_NAME, Dispatch::OnSegment, NULL))
	{
		MessageBoxW(NULL, L"渲染宿主启动失败(窗口类注册、窗口创建或渲染设施初始化失败)!",
			IDENTITY_ERR_TITLE, MB_OK | MB_ICONERROR);
		Dispatch::Release();
		return 1;
	}
	// 6. 主消息循环: 每帧调一次 Dispatch::Frame; 退出判据地址由调度层持有
	pHost->Run(Dispatch::Frame, NULL, Dispatch::GetExitAddress());
	// 7. 成对释放(次序: 渲染设施 → 窗口与窗口类 → 调度场景与会话分配器)
	pHost->Release();
	pHost->DestroyWnds();
	Dispatch::Release();
	return 0;
}
