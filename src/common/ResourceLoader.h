// ================================
// 文件: ResourceLoader.h
// 描述: 资源与路径加载原语 —— exe 目录解析、路径拼接与"随 exe 内置的资源节"读取。
// ================================
#pragma once
#include <windows.h>

// ================================
// 命名空间: ResourceLoader
// 描述: 资源与路径加载原语(进程级单例语义; 与 GenUIEditor / SelectionOverlay 同风格)
// ================================
namespace ResourceLoader
{
	// 函数: Init
	// 描述: 缓存 exe 所在目录(含末尾 '\'; 应用启动期调用一次; 幂等)
	VOID Init();

	// 函数: GetExeDir
	// 描述: 取缓存的 exe 目录(含末尾 '\'; 未初始化/失败 ⇒ 空串)
	LPCSTR GetExeDir();

	// 函数: GetFilePath
	// 描述: 拼接 exe 目录与文件名(内存池字符串; 每次调用新分配)
	LPCSTR GetFilePath(LPCSTR filename);

	// 函数: FileExists
	// 描述: 普通文件是否存在(加载链的"外部文件"级别判据)
	BOOL FileExists(LPCSTR path);

	// 函数: LoadResource
	// 描述: 读取随 exe 内置的资源节字节并拷进内存池(末尾补一位 '\0')
	BOOL LoadResource(DWORD dwResId, LPCSTR szResType, LPVOID* ppData, DWORD* pcbData);
}
