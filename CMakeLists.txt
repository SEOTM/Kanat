cmake_minimum_required(VERSION 3.28)

# ================================
# 工程: Kanat(暗渠框架) —— 框架内核 + 最小 Demo 示例
# 描述: 暗渠框架(Kanat)的内核与最小 Demo 示例 —— framework 层(基础设计量 / 布局模型 /
#       控件库 / 渲染 / 事件机制 / 读方向解析 / 几何解算 / ImGui 宿主 / D3D11 后端; 源自 GenUI 快照并就地演进)
#       与内存 / 环境 / 校验 / 路径等基础设施(2026-10-23 起并入 framework: base / base\mem / base\util / dispatch)。
#
# 层级裁剪(相对 GenUI\src, app / doc / editor / tests 四层整体不引入):
#   1. 无编辑器能力  : 不载入编辑器外壳布局, 无布局增删 / 改类型 / 写盘, 无文件对话框与提示布局
#                      (源 = GenUI 的 editor 层, 整体不引入; 故不链接 comdlg32);
#   2. 无文档承载者  : 单布局场景, 不设条目表 / 中间态 / 脏标记(源 = GenUI 的 doc 层与 app\AppHost);
#   3. 无内嵌文档链  : 布局文档只从 exe 同目录的 Kanat.json 读取 —— GenUI 侧的 bin 生成
#                      (tools\json2bin.py)与 .rc 资源内嵌链在本工程整体裁掉(不启用 RC 语言);
#   4. 无探针        : 源 = GenUI 的 tests 层, 整体不引入。
#
# 源码来源: framework / common 两层为 GenUI\src 对应目录的逐字节复制快照(复制时刻已核验
#           SHA-256 一致); 演示层(src\demo)为 GenUI 侧 app\AppHost 的单窗口精简形态。
# 例外一: framework\ImGuiHost.{h,cpp} 为本工程首产的框架层新增件(GenUI 侧无对应文件) ——
#         它把原来散在演示层宿主的 ImGui 初始化与帧流程收成框架能力(窗口类注册与建窗 / 每标准窗口
#         一套上下文与两端后端 / 中文字体 / 亮暗基础样式 / NewFrame 与 Render / 后端消息转发 /
#         成对释放 / 激活窗口数不足时隐藏多余窗口)。
# 例外二(2026-10-16 第 7 轮, 用户指令): 渲染侧与 ImGui 彻底解耦 —— Render.h 改为"渲染能力接口定义"
#         (函数指针表 RenderApi + 窗口状态位), 新增 ImGuiRender.{h,cpp}(ImGui 侧能力提供者, 渲染侧
#         唯一 include imgui 的边界)与 LayoutRender.{h,cpp}(对外渲染出口, 不依赖任何 UI 库),
#         原 Render.cpp 删除; 注入点 = ImGuiHost::InitUnit(把 ImGuiRender 的能力表交给 LayoutRender)。
# 例外三(2026-10-16 第 8 轮, 用户指令): 渲染出口去窗口身份 + ImGuiHost 接管渲染链 ——
#         布局对象是纯描述: LayoutRender 与 Render.h 不再出现任何窗口句柄, 渲染目标由渲染器按顺序
#         决定(能力表新增 AcquireWnd; 基座布局路径显式调用, 内嵌(flex)路径沿用当前窗口);
#         ImGuiHost 独持主消息循环 / 窗口过程 / 每窗口 ImGui 面 / 每窗口 D3D11 设施 / 呈现与遮挡 /
#         IME / 窗口圆角, 每帧固定调用一次外部处理流程(函数指针), 退出判据由外部持变量、其地址交给
#         ImGuiHost; 取消帧抓取(DEMO_FRAME_DUMP 与 BMP 回读整段删除); 窗口级事件经事件回调的
#         param 通用槽携带 hWnd。
# 例外四(2026-10-16 第 10 轮提出 / 第 11 轮按用户口径修正, 用户指令): 窗口位置记录与几何结算 ——
#         窗口件(WND 控件)的 position 提为窗口位置记录: 解析期按三支规则给初值((-1,-1) 随机 /
#         在屏内直用 / 越屏降级随机; 见 GeometrySolver::ResolveWndPos 与 ScreenRef 的工作区查询),
#         运行期由标题栏绑定的事件改写; 能力表 AcquireWnd 的签名带上期望位置与尺寸
#         (BOOL AcquireWnd(CONST vec2& pos, CONST vec2& size)), 由 ImGuiHost::AcquireNextWnd 在取得
#         渲染目标时(该窗口绘制之前)结算几何; OpenWnd 的摆放位置改为布局原点 (0,0); 快照件
#         Controls.{h,cpp} 的标题栏派发点就地改动(第二次): param 通用槽由 hWnd 改为"窗口期望坐标"
#         (打包口径 = ControlSpec.h 的 PackWndPos / UnpackWndPos), 拖动基线读窗口位置记录。
#         除上述四处例外(以及 2026-10-15 第 5 轮的数组命名治理漂移)之外, 其余 framework / common
#         文件仍与 GenUI 快照一致。
# 例外五(2026-10-16 第 13 轮, 纲领七): 快照件 Controls.cpp 就地执行"部件契约统一 + 同名部件补齐" ——
#         基础部件新增第四固定参数(绘制方式 Widgets::Base::PaintWay: desc{PALETTE, IMAGE} + 载荷联合),
#         13 件既有部件签名统一为四固定参数并删净部件侧层号接触, 取色上移至控件层"控件取色区"
#         (MakeFramePalette / MakeListPalette), 新增 Button / Text / CheckBox / RenderSurface 四枚同名部件,
#         ComboField 改名 Combo; 控件层不再直调 ImGui 原生控件(占位 Dummy / 命中项 / 列表拖动辅助按用户口径
#         保留在控件层); 证据见 docs\HISTORY\2026-10-16-120340.md。
# 例外六(2026-10-16 第 14 轮, 用户指令): 部件与控件分离 —— 快照件 Controls.cpp 内的部件层(声明与实现)
#         整体迁出为新增件 Widgets.{h,cpp}(命名空间仍为 Controls::Widgets): 部件层自持上下文
#         (Widgets::UpdateContext / HasActiveInput, 由 Controls::UpdateContext 转调 ⇒ 每帧单一写点),
#         控件对外契约与 LayoutRender 调用点不变; 跨层共用辅助按用户口径"留控件层 + 部件层自留内部副本"
#         (部件层副本 = toImvec2 / DrawRectFace / 列表拖动把手与落点 / 行件返回约定折算); 随部件下移的
#         常量 = SLIDER_GRIP_RATIO / SLIDER_TRACK_RATIO。
#         除上述七处例外(以及 2026-10-15 第 5 轮的数组命名治理漂移)之外, 其余 framework / common
#         文件仍与 GenUI 快照一致。
# 例外七(2026-10-21 第 32 轮提出 / 结构调整 S3 归位, 纲领九): main 重新成为胶水 + Dispatch 调度层静态单例 ——
#         调度层归位 framework\dispatch\(Dispatch.{h,cpp,md}: 事件绑定注入 /
#         会话分配器与场景装配 / 投影配色与样式注入 / 每帧流程 / 无人值守 / 退出判据 / 清理)与
#         DispatchScene.cpp(场景实现切片: LoadScene / UnloadScene); 演示层只留 demo\DemoEvents(事件表);
#         首产件 ImGuiHost 增 Start(数量 + 应用名 → 默认几何建窗 → 渲染设施) 并收回窗口过程(内置 WM_DESTROY / ESC),
#         AcquireNextWnd 增窗口标题参数; Render.h / ImGuiRender / LayoutRender 的 AcquireWnd 同步带标题。
# 依赖: imgui(含 win32 / dx11 后端)与 rapidjson 由本地 NuGet 包 GenUI.Deps.Native 提供
#       (离线本地 feed = 本工程 nuget\ 目录)。
#
# 构建(经工作区 IPCHost 通道; 每次重新 configure 后必须重跑 NuGet 依赖注入):
#   cmake -S Kanat -B Kanat\out -G "Visual Studio 18 2026" -A x64
#   注入 PackageReference(片段见 README「构建」一节) → msbuild Kanat\out\Kanat.slnx -t:Restore
#   → msbuild Kanat\out\Kanat.slnx -p:Configuration=Release -p:Platform=x64 -m
# 注: 旧的四步封装脚本 tools\build.ps1 已不在本仓库, 现行步骤以 README 为准。
# ================================
# ================================
# 项目身份(派生新项目时改本节两行 + src\Identity.h)
# ================================
set(APP_PROJECT_NAME "Kanat")	# 工程名 / 目标名 / 解决方案名
set(APP_DOC_NAME "Kanat.json")	# 布局文档文件名(构建期复制到 exe 同目录)

project(${APP_PROJECT_NAME} LANGUAGES CXX)

if(MSVC)
	# 源码注释为 UTF-8(无 BOM), 需 /utf-8 以免按代码页 936 误读中文
	# /Zc:char8_t-: 沿用工程惯例 u8"..." 作为 char*(兼容 LPCSTR 传参)
	add_compile_options(/utf-8 /Zc:char8_t-)
endif()

# 静态运行库 /MT(/MTd), 自包含
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ================================
# 源码(本工程自持; framework = GenUI 的逐字节复制快照 + 本工程首产件)
#   framework\base           : 基础设计量 / 控件类型契约 / 控件缓冲数据与装配 / 函数指针 / 文本宽度
#                              / 平台环境件(AppEnv)
#   framework\base\mem       : 内存设施(MemoryManager 分配器 + AllocatorHub 会话分配器;
#                              2026-10-23 自 common 归位)
#   framework\base\util      : 与框架语义无关的纯算法件(FileCodec 校验; 2026-10-23 自 common 归位)
#   framework\render         : 渲染能力接口(Render.h)与具体渲染技术的实现(按技术名分目录)
#   framework\render\imgui         : ImGui 宿主(窗口 / 上下文 / 帧流程 / 消息循环)与 D3D11 图形后端设施
#                                   (设备 / 交换链 / 渲染目标 / 呈现; 2026-10-22 自 common 归位)
#   framework\render\imgui\ability : ImGui 侧渲染能力提供者(能力表的唯一填表处)
#   framework\render\imgui\control : 控件库与部件层(逐控件绘制)
#   framework\layout         : 布局子系统(布局对象 / 中间态模型 / 构建原语 / 读方向解算 / 渲染出口 / 几何解算)
#   framework\dispatch       : 调度层(帧间事件 / 事件绑定表 / Dispatch 与场景实现切片 / 资源与路径解析 *)
#   demo                     : 本 Demo 的业务层(事件绑定表)
# 注: 原 src\common 层已于 2026-10-23 取消(五件全部归位): 平台环境件 → framework\base;
#     内存设施 → framework\base\mem; 纯算法 → framework\base\util; exe 路径解析 → framework\dispatch。
# 注: Layout 前缀的 13 件(Layout / LayoutBuilder / LayoutModel / LayoutRender / LayoutSolver,
#     各含配套 .md 接口文档)收进 framework\layout 子目录 —— 源码内引用随之带上 layout 前缀。
# ================================
set(KANAT_BASE_SRC
	src/framework/base/AppEnv.cpp
	src/framework/base/BasicDesign.cpp
	src/framework/base/ControlSpec.cpp
	src/framework/base/CtrlBuffer.cpp
	src/framework/base/TextWidth.cpp
	src/framework/base/mem/AllocatorHub.cpp
	src/framework/base/mem/MemoryManager.cpp
	src/framework/base/util/FileCodec.cpp
)

set(KANAT_RENDER_SRC
	src/framework/render/imgui/D3D11Manager.cpp
	src/framework/render/imgui/ImGuiHost.cpp
	src/framework/render/imgui/ability/ImGuiRender.cpp
	src/framework/render/imgui/control/Controls.cpp
	src/framework/render/imgui/control/Widgets.cpp
)

set(KANAT_LAYOUT_SRC
	src/framework/layout/GeometrySolver.cpp
	src/framework/layout/Layout.cpp
	src/framework/layout/LayoutBuilder.cpp
	src/framework/layout/LayoutModel.cpp
	src/framework/layout/LayoutRender.cpp
	src/framework/layout/LayoutSolver.cpp
)

set(KANAT_DISPATCH_SRC
	src/framework/dispatch/Dispatch.cpp
	src/framework/dispatch/DispatchScene.cpp
	src/framework/dispatch/FrameEvent.cpp
	src/framework/dispatch/ResourceLoader.cpp
	src/framework/dispatch/UIEventHandler.cpp
)

set(KANAT_DEMO_SRC
	src/demo/DemoEvents.cpp
)

add_executable(${APP_PROJECT_NAME}
	src/main.cpp
	src/Identity.h
	${KANAT_BASE_SRC}
	${KANAT_RENDER_SRC}
	${KANAT_LAYOUT_SRC}
	${KANAT_DISPATCH_SRC}
	${KANAT_DEMO_SRC}
)

# 唯一包含根 = src\(源码内一律以"子目录/文件"形式引用, 与 GenUI 的引用口径一致)
target_include_directories(${APP_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Windows SDK 库; imgui/rapidjson 头与 imgui 库由 NuGet 包 GenUI.Deps.Native 注入
# 注: comdlg32(文件对话框)属编辑器侧能力, 本 Demo 裁剪后不链接
target_link_libraries(${APP_PROJECT_NAME} PRIVATE d3d11 dxgi dwmapi imm32)

# 平台宏: x64 架构 + Unicode(与 windows.h 窄宽 API 语义一致)
target_compile_definitions(${APP_PROJECT_NAME} PRIVATE _AMD64_ UNICODE _UNICODE)

# 布局文档: 构建期生成 bin(格式与脚本同 GenUI), 投放到 exe 同目录(加载链第 1 级 .\Kanat.bin)。
# 找不到 Python ⇒ 只复制工程源 JSON(加载链第 2 级; 警告, 不阻断构建)。
set(KANAT_DOC_BIN "${CMAKE_CURRENT_BINARY_DIR}/Kanat.bin")
find_program(KANAT_PYTHON NAMES python python3)
if(KANAT_PYTHON)
	add_custom_command(
		OUTPUT "${KANAT_DOC_BIN}"
		COMMAND "${KANAT_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/json2bin.py"
			"${CMAKE_CURRENT_SOURCE_DIR}/${APP_DOC_NAME}" "${KANAT_DOC_BIN}"
		DEPENDS
			"${CMAKE_CURRENT_SOURCE_DIR}/${APP_DOC_NAME}"
			"${CMAKE_CURRENT_SOURCE_DIR}/tools/json2bin.py"
		COMMENT "生成布局 bin 文档(Kanat.bin)"
		VERBATIM
	)
	add_custom_target(${APP_PROJECT_NAME}DocBin DEPENDS "${KANAT_DOC_BIN}")
	add_dependencies(${APP_PROJECT_NAME} ${APP_PROJECT_NAME}DocBin)
	add_custom_command(TARGET ${APP_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${KANAT_DOC_BIN}"
			"$<TARGET_FILE_DIR:${APP_PROJECT_NAME}>/Kanat.bin"
	)
else()
	message(WARNING "未找到 Python ⇒ 不生成 Kanat.bin, 改为复制工程源 JSON")
	add_custom_command(TARGET ${APP_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_CURRENT_SOURCE_DIR}/${APP_DOC_NAME}"
			"$<TARGET_FILE_DIR:${APP_PROJECT_NAME}>/${APP_DOC_NAME}"
	)
endif()
