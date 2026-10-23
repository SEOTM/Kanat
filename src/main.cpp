#include <windows.h>

#include "Identity.h"
#include "demo/DemoEvents.h"
#include "framework/dispatch/Dispatch.h"
#include "framework/render/imgui/ImGuiHost.h"

// 函数: main
// 描述: 暗渠框架 Kanat 程序入口(取宿主表 → 调度初始化 → 宿主启动 → 主循环 → 释放)
int main(int, char**)
{
	CONST HostApi* pHost = ImGuiHost::Register();
	DispatchConfig cfg{};
	cfg.szDocName = IDENTITY_DOC_NAME;
	cfg.szDocEnv = IDENTITY_DOC_ENV;
	cfg.szAutoCloseEnv = IDENTITY_ENV_AUTOCLOSE;
	cfg.szLogPrefix = IDENTITY_LOG_PREFIX;
	cfg.dwWndCount = 1;
	cfg.bDarkFollowSystem = TRUE;
	if (pHost == NULL || !Dispatch::Init(pHost, cfg, DemoEvents::GetBindings))
	{
		MessageBoxW(NULL, L"应用初始化失败(布局文档载入或事件绑定注入失败)!",
			IDENTITY_ERR_TITLE, MB_OK | MB_ICONERROR);
		return 1;
	}
	ImGuiHost::SetFontSize(Dispatch::GetFontSize());
	if (!pHost->Start(Dispatch::GetWndCount(), IDENTITY_APP_NAME, Dispatch::OnSegment, NULL))
	{
		MessageBoxW(NULL, L"渲染宿主启动失败(窗口类注册、窗口创建或渲染设施初始化失败)!",
			IDENTITY_ERR_TITLE, MB_OK | MB_ICONERROR);
		Dispatch::Release();
		return 1;
	}
	pHost->Run(Dispatch::Frame, NULL, Dispatch::GetExitAddress());
	pHost->Release();
	pHost->DestroyWnds();
	Dispatch::Release();
	return 0;
}
