# Demo · 派生指南(纲领二的展开)

> 纲领二原文(2026-10-15): "后续其他本框架代码，将从本项目开始"。
> 解读(用户确认, 选项 A): 本项目 = **框架消费方项目的派生源(母本)** —— 后续其他使用本框架的项目,
> 以本项目为起点派生出去, 不再从 GenUI 复制。
> 因此本工程的第一要求是**可派生**: 身份集中、派生成本最低、派生后有明确自证口径。
>
> **公开版说明**: 本仓库为公开版本, 已移除逐轮过程留痕(`docs\HISTORY\` 等)与内部工程管理文档。
> 文中指向这些目录的引用仅用于说明沿革, 对应文件不在本仓库内。

## 1. 派生五步

| 步 | 动作 | 落点 |
| :--- | :--- | :--- |
| 1 | 复制整个工程目录并改名 | `<ws>\Demo` → `<ws>\<新项目>` |
| 2 | 改**代码侧身份**(唯一真源) | `src\Identity.h` |
| 3 | 改**构建侧身份** | `CMakeLists.txt` 顶部两行 `APP_PROJECT_NAME` / `APP_DOC_NAME` |
| 4 | 改**布局文档身份** | 布局文档的 `layouts[0].name` / `label`, 以及各控件 `event` 字段(须与 `IDENTITY_EVENT_*` 成对) |
| 5 | 重建与自证 | `tools\build.ps1` 四步 + exe 目录内启动一次(口径见 §4) |

`tools\build.ps1` **已与工程名解耦**(解决方案与生成的工程文件按 `out\` 通配发现) ⇒ 派生后无需修改。

**派生时不再复制应用层协调代码**(2026-10-21 结构调整): 协调层已归 `src\framework\dispatch\Dispatch.{h,cpp}` +
`DispatchScene.cpp`, 属框架侧、随目录整体带走 ⇒ 派生者**不需要**从 GenUI 或其它项目再抄一份应用层宿主/场景代码,
也**不需要改** `Dispatch.cpp`(它与 `DispatchScene.cpp` 不含任何项目身份; 身份经 `DispatchConfig` 注入。
`Identity.h` 现含 `IDENTITY_DOC_NAME` / `IDENTITY_DOC_ENV` / `IDENTITY_ENV_AUTOCLOSE` / `IDENTITY_APP_NAME` /
`IDENTITY_ERR_TITLE` / **`IDENTITY_LOG_PREFIX`** / 两条事件名共 7 个常量)。派生动作因此只有四处:
`src\Identity.h`(常量)、`CMakeLists.txt` 顶部两变量、布局文档身份字段, 以及 `src\demo\DemoEvents`(业务事件)。

## 2. 必改项(功能身份; 漏改会出错)

| 项 | 现值 | 落点 | 漏改后果 |
| :--- | :--- | :--- | :--- |
| 工程名 / 目标名 | `Demo` | CMakeLists.txt:`APP_PROJECT_NAME` | 与新项目名不符, 同机多工程易混 |
| 布局文档名 | `Demo.json` | CMakeLists.txt:`APP_DOC_NAME` + `Identity.h:IDENTITY_DOC_NAME` | 运行期找不到文档(退出码 1) |
| 应用名 | `Demo` | `Identity.h:IDENTITY_APP_NAME` | 窗口类名(`<APPNAME>_IMGUI_CLASS`)与窗口标题不随新项目变; 类名须进程外唯一, 不改可能与同机其它程序重名 |
| 错误标题 | `GenUI Demo 错误` | `Identity.h:IDENTITY_ERR_TITLE` | 启动失败弹窗仍显示旧项目名(无害, 但不专业) |
| 环境开关名 | `DEMO_DOC` / `DEMO_AUTOCLOSE_MS` | `Identity.h` | 与同机其它工程的自动化脚本约定冲突 |
| 事件名 | `DEMO_DRAG_WINDOW` | `Identity.h:IDENTITY_EVENT_DRAG_WINDOW` + 布局文档 `event` 字段 | **只改一侧 ⇒ 静默失绑**(控件无响应, 加载与运行均无报错) |
| 布局名 / 标签 | `DemoMain` / `GenUI Demo` | 布局文档 `layouts[0].name` / `label` | 仅影响日志与标识 |

## 3. 可选改动(不影响功能)

- 日志前缀 `[DEMO]`(`Identity.h` 的 `IDENTITY_LOG_PREFIX`; 经 `DispatchConfig.szLogPrefix` 交给调度层, 每行打印为
  `[<前缀>] `) —— 前缀**已配置化**, 派生时只改这一个常量, 不必改 `Dispatch.cpp`;
- 事件层目录名 `src\demo\` 与事件层类名 `DemoEvents`(场景装配切片 = `src\framework\dispatch\DispatchScene.cpp` 内的私有静态函数);
- `README.md` 与 `docs\*.md` 中的工程名、路径示例与文档名。

## 4. 派生后自证(建议口径)

1. **构建**: `cfg → inject → restore → release` 四步全绿(0 警告 0 错误);
2. **运行**: exe 目录内启动一次, 退出码 0;
3. **载入确认**: 日志"场景载入"行的路径与新文档名一致; `[DEMO] 建窗几何:` 行是 `ImGuiHost::Start` 的默认
   几何(客户区 800x600、工作区居中), 文档几何在首帧由 `AcquireNextWnd` 结算(见随后自证行的"窗口调整"列);
4. **布局有效性**: 结构件缺失或错序**不会**在加载期报错(退出码 0、启动段日志约 7 行, 见
   `docs\EXAMPLES.md` §3.1) ⇒ **本条目前无自动判据, 属待补**: 计划在场景层显式查询
   `Layout::IsInvalid` 并打印一行有效性诊断, 使派生者一眼可见;
5. **交互与外观**: 一律标注"待本地人工复核"(无人值守会话无法合成真实鼠标交互)。

## 5. 无需改动(随派生整体带走)

`src\framework\`(59 文件 = 源 39 件 + 接口文档 20 份; 分 `base` 14 / `render` 2 / `render\imgui` 15 / `layout` 18 / `dispatch` 10) ·
`src\common\`(15 文件 = 源 9 件 + 接口文档 5 份) ·
`src\Identity.{h,md}` / `src\main.{cpp,md}` ·
`src\framework\dispatch\Dispatch.{h,cpp,md}` / `src\framework\dispatch\DispatchScene.cpp`(调度层: 派生新项目时只经
`DispatchConfig` 注入身份与场景文档名, 结构本身随派生整体带走, 无需改实现) ·
`NuGet.config` · `nuget\GenUI.Deps.Native.1.0.0.nupkg` · `tools\build.ps1`。

> `src\demo\`(`DemoEvents.{h,cpp,md}`)不在本清单: 它是"框架使用范例"的事件层, 派生新项目时按新应用的业务事件改写
> (可作范本参考); 现行 `src\demo\` 共 3 文件 = 源 2 件 + 接口文档 1 份。

> 计数口径(2026-10-16 第 7 轮, 2026-10-18 第 27 轮按"源 + 文档"重列, 2026-10-21 结构调整 S5 按四层重列,
> 2026-10-22 按 `CtrlData` 并入 `CtrlBuffer` 与 `D3D11Manager` 归位复核):
> `framework` 源 39 件 = GenUI 快照 25 件(含自 `common` 归位的 `D3D11Manager` 2 件; 原 30 件去 `Render.cpp` 等上游裁剪,
> 并删 `CtrlData.h`) + 部件层拆分产物 2 件(`Widgets.{h,cpp}`) +
> 首产件 7 件(`ImGuiHost.{h,cpp}` + `ImGuiRender.{h,cpp}` + `Render.h` + `LayoutRender.{h,cpp}`) +
> 新增与归位件 5 件(`base\CtrlBuffer.{h,cpp}` + `dispatch\Dispatch.{h,cpp}` + `dispatch\DispatchScene.cpp`);
> 接口文档 = **每模块一份**共 28 份(`framework` 20 + `common` 5 + `Identity` / `main` 2 + `demo` 1), 体例见
> `docs\INTERFACE-DOC-STYLE.md`。全工程 81 文件 = 源 53 件 + 接口文档 28 份。
> `Render.h` 仍是快照件(文件名不变), 但其内容已就地改造为"渲染能力接口定义"(受控漂移, 见下)。

框架层原本是 GenUI 侧的**逐字节复制快照**; 上游修复不会自动进入派生项目, 同步方式见 `README.md` 第 6 节。
派生后接口文档随源目录整体带走, 文档内的模块名 / 路径已按 `src\` 相对路径书写, 改名只需替换工程身份
(`Identity.h` 与 `CMakeLists.txt` 顶部两变量), 无需逐份改文档。

**命名治理(2026-10-15 第 5 轮)**: `framework` / `common` 已就地执行数组命名治理(数组名去 `a` / `asz` 前缀、
改用复数; 规则见 `CodingPre\Habits_CPP.md` §4, 对照表见 `docs\HISTORY\2026-10-15-000005.md`), 故这两层与 GenUI 侧快照
**不再逐字节一致**。派生时随目录整体带走(派生者无需额外动作); 但从 GenUI 重新复制这两层会**覆盖**本次
治理, 处置口径见 `README.md` §6。

**渲染解耦(2026-10-16 第 7 轮)**: `Render.h` 已就地改造为渲染能力接口(函数指针表), `Render.cpp` 已删除,
新增 `ImGuiRender.{h,cpp}` 与 `LayoutRender.{h,cpp}` —— 详见 `docs\CHARTER.md` 纲领四与
`docs\EXAMPLES.md` §2 的第三例例外条。从 GenUI 重新复制 `framework` 层同样会覆盖本次改造。

**渲染链归属(2026-10-16 第 8 轮)**: `Render.h` 增第 17 项能力 `AcquireWnd`(输入期望客户区尺寸, 不符即
主动调窗); `LayoutRender` 删 `CheckWindowResize`(渲染出口零窗口身份); 首产件 `ImGuiHost` 收回主消息
循环 / 帧流程 / 每窗口 D3D 设施 / 呈现 / 遮挡 / IME / 窗口圆角(公开面新增 `FrameFunc` / `Run` /
`AcquireNextWnd` / `GetCurrentWnd` / `GetWndCount` / `SetIMEDisabled` / `GetRenderStats` / `GetResizeStats`,
`WndSpec` 增圆角半径字段, `InitUnit` / `NewFrame` / `RenderToTarget` 收进内部); 快照件 `Controls.cpp` 的
标题栏派发点把 `param` 通用槽改为携带当前渲染窗口句柄; 帧抓取(`DEMO_FRAME_DUMP` 与后台缓冲回读)取消。
详见 `docs\CHARTER.md` 纲领五与 `docs\EXAMPLES.md` §2 第四例例外条。从 GenUI 重新复制 `framework` 层
同样会覆盖本次改造。

**窗口位置请求(2026-10-16 第 10 轮)**: `Render.h` 增第 18 项能力 `SetWndPos`(装填目标窗口的期望位置);
首产件 `ImGuiHost` 增 `PostWndPos` / `GetWndPos`, 并由 `AcquireNextWnd` 在取得渲染目标时消费装填值
(实质移动发生在窗口绘制之前, 比请求帧晚一帧); `ControlSpec.h` 增标题栏参数槽的打包 / 解包口径
(`PackWndPos` / `UnpackWndPos`); 快照件 `Controls.cpp` 的标题栏派发点改为自行算出"窗口期望坐标"并放进
`param` 通用槽(不再携带窗口句柄); `DemoEvents.{h,cpp}` 只把位置请求装填给能力表、不调任何窗口 API。
详见 `docs\CHARTER.md` 纲领六与 `docs\HISTORY\2026-10-16-093925.md`。本轮为**可回滚试验**(回滚基线 =
`Demo\out\Release\demo_old.exe`, 手感不佳即整体还原)。从 GenUI 重新复制 `framework` 层会覆盖本次改造。

**窗口位置归属(2026-10-16 第 10 轮提出 / 第 11 轮按用户口径修正)**: 窗口件(WND 控件)的 `position` 即
**窗口位置记录** —— 解析期按三支规则给初值(声明 `(-1,-1)` 随机 / 在屏内直用 / 越屏降级随机, 见
`GeometrySolver::ResolveWndPos` 与 `ScreenRef` 的工作区查询), 运行期由标题栏绑定的事件改写(事件 `param`
携带期望坐标, 打包见 `ControlSpec.h` 的 `PackWndPos` / `UnpackWndPos`); 渲染出口以
`AcquireWnd(pos, size)`(**签名带参**)把记录交给实现方, 由 `AcquireNextWnd` 在该窗口绘制之前结算位置与尺寸;
`OpenWnd` 的摆放位置改为布局原点 `(0,0)`; 标题栏拖动基线现行读位置记录(`Controls.cpp` 的
`GRAB_BASE_BY_WND_POS`, 手感不佳即回退为读真实窗口矩形)。涉及快照件: `Render.h` / `layout\LayoutRender.cpp` /
`Controls.{h,cpp}` / `GeometrySolver.{h,cpp}` / `BasicDesign.{h,cpp}`。详见 `docs\CHARTER.md` 纲领六
(含口径修正)与 `docs\HISTORY\2026-10-16-101915.md`。

**调度层形态(2026-10-21 第 32 轮提出, 结构调整 S3 归位)**: `src\main.cpp` 只做胶水(初始化 → 宿主启动 → 主循环 → 释放); 调度逻辑集中在
`src\framework\dispatch\Dispatch.{h,cpp}`(静态单例: 事件绑定表经函数指针注入 / 会话分配器与成员块 / 文档解析与几何解算 /
投影配色与样式注入 / 每帧流程 / 无人值守 / 退出判据 / 清理), 场景装配的分配与释放收在同目录
`DispatchScene.cpp`(私有静态函数 `LoadScene` / `UnloadScene`)。派生新项目时通常只需替换场景文档与业务事件,
调度层结构可整体带走; 建窗与窗口身份(类名与默认标题)由 `framework\render\imgui\ImGuiHost::Start` 按 `IDENTITY_APP_NAME` 派生,
派生者不再需要自带窗口过程。

**例外**: 本工程首产的框架层文件 = `render\imgui\ImGuiHost.{h,cpp,md}`(纲领三) 与
`render\imgui\ability\ImGuiRender.{h,cpp}` / `render\Render.h` / `layout\LayoutRender.{h,cpp}`(纲领四);
另有部件层拆分产物 `render\imgui\control\Widgets.{h,cpp,md}`、自控件库拆出的 `base\CtrlBuffer.{h,cpp,md}` 与
归位的 `dispatch\Dispatch.{h,cpp,md}` + `DispatchScene.cpp` —— 2026-10-21 起 GenUI 侧已同名(第 43 轮反向
吸收其九件), 派生时仍随目录整体带走; 从 GenUI 重新复制 `framework` 层时**不得覆盖或删除**这些件(整目录替换会丢件),
且须先按 `README.md` §6 的"结构治理(2026-10-21)"重放分层(GenUI 侧仍是旧扁平路径)。

另有**快照件路径归位 2 件**(`render\imgui\D3D11Manager.{h,cpp}` + 随迁接口文档, 自 `common` 移至
`render\imgui`, 内容未改) —— 从 GenUI 重新复制 `common` 时按 `README.md` §6 的"漂移处置(2026-10-22
D3D11Manager 归位)"处置, 否则会写回一份不在源表内的同名件。
