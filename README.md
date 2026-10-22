# Kanat（暗渠框架）

暗渠框架（Kanat）：声明式界面框架 —— 一份布局文档 → 一张布局对象 → 一条渲染路径 → 事件处理。
本仓库同时承载**框架内核**与**随框架附带的最小 Demo 示例**：内核是暗渠框架本体，示例是"如何用暗渠框架写一个界面"的最小样本。

> 沿革：本工程原名 `Demo`（GenUI 的最小演示程序），2026-10-22 正式改名 `Kanat` 并定为框架名"暗渠框架"。
> 示例层的 Demo 语义（事件表 `DemoEvents`、`DEMO_*` 环境变量与事件名、`src\demo\` 目录）按用户口径保留不变。

## 两层结构

| 层 | 目录 | 说明 |
| :--- | :--- | :--- |
| 框架内核 | `src\framework\` + `src\common\` | 暗渠框架本体：基础设计量、控件类型契约、控件缓冲、文本宽度、渲染能力表与 ImGui / D3D11 实现、布局对象与解算、调度层、内存管理与会话分配器、环境件、文件校验、资源与路径 |
| 最小 Demo 示例 | `src\demo\` + `src\main.cpp` + `Kanat.json` | 随框架附带的参考实现：两条业务事件（拖动窗口 / 事件委托）与一份单窗口布局文档 |

框架内核不含任何项目身份：应用名、文档名、事件名等经 `src\Identity.h` 与 `CMakeLists.txt` 顶部两个变量注入（派生新项目见 `docs\DERIVE.md`）。

## 演示内容

- 一份单布局文档 `Kanat.json`：结构件（窗口 / 标题栏 / 内容容器）+ 12 枚内容控件。
- 标题栏按住拖动窗口：拖动事件把窗口期望坐标写进布局的窗口位置记录，渲染侧按记录结算窗口几何。
- 事件委托：容器绑定 `DEMO_ENTRUST_PROBE`，其子按钮以 `CTRL_EVENT_ENTRUST_PARENT` 把自己的事件委托给父容器，点击到达父容器的处理函数。
- 父子链：`parent` 字段驱动层序与坐标累加，运行日志逐枚打印层值与坐标供核对。
- 自适应几何：所有控件位置与尺寸来自布局参考量（字号 / 控件高 / 间隔 / 内边距），窗口缩放时整体等比重算。

## 构建

### 前置

| 项 | 要求 |
| :--- | :--- |
| 系统 | Windows 10 / 11 x64 |
| 工具链 | Visual Studio 2026（"使用 C++ 的桌面开发"工作负载），CMake 3.28+ |
| Python | Python 3（构建期把 `Kanat.json` 转成 `Kanat.bin`；缺失时退化为只复制 JSON） |
| 依赖 | 仓库自带 `nuget\GenUI.Deps.Native.1.0.0.nupkg`（ImGui 1.92.x + DX11 / Win32 后端、rapidjson），无需联网 |

### 步骤

```powershell
# 1. 生成解决方案
cmake -S . -B out -G "Visual Studio 18 2026" -A x64

# 2. 注入本地 NuGet 依赖（每次 configure 后都要重跑）
$vcx = "$PWD\out\Kanat.vcxproj"
$xml = [IO.File]::ReadAllText($vcx)
$xml = $xml.Replace('</Project>', @"
  <ItemGroup>
    <PackageReference Include="GenUI.Deps.Native"><Version>1.0.0</Version></PackageReference>
  </ItemGroup>
</Project>
"@)
[IO.File]::WriteAllText($vcx, $xml, (New-Object Text.UTF8Encoding($false)))

# 3. 还原并构建
msbuild out\Kanat.slnx -t:Restore -p:Configuration=Release -p:Platform=x64
msbuild out\Kanat.slnx -p:Configuration=Release -p:Platform=x64 -m
```

Visual Studio 2022 把生成器名改为 `"Visual Studio 17 2022"` 即可；工程只要求 C++20。

### 产物

```
out\Release\
├─ Kanat.exe      暗渠框架的最小 Demo（单窗口）
└─ Kanat.bin      构建期生成的 bin 文档
```

## 运行

工作目录必须是 exe 所在目录（布局文档按 exe 同目录解释）：

```powershell
cd out\Release
.\Kanat.exe
```

加载链：`.\Kanat.bin` 优先，其次 `.\Kanat.json`；形态由内容嗅探判定。

| 环境变量 | 作用 |
| :--- | :--- |
| `DEMO_DOC` | 覆盖布局文档文件名（缺省 `Kanat.json`）；可让同一 exe 承载多份范例 |
| `DEMO_AUTOCLOSE_MS` | 启动后 N 毫秒自动退出（无人值守运行用）；不设则常驻 |

退出：`ESC` 或 `Alt+F4`。

启动时控制台打印场景载入、控件数、事件绑定、逐控件层与坐标等自证日志，便于核对布局是否按预期展开。

## 目录结构

```
Kanat\
├─ src\
│  ├─ main.cpp                 进程入口（取宿主表 → 调度初始化 → 宿主启动 → 主循环 → 释放）
│  ├─ Identity.h / .md         项目身份唯一真源（应用名 / 文档名 / 环境开关）
│  ├─ demo\                    示例层：事件绑定表 DemoEvents
│  ├─ common\                  通用基础设施（GenUI 快照起点）
│  └─ framework\               框架内核（GenUI 快照起点 + 本工程首产件）
│     ├─ base\                 基础设计量、控件类型契约、控件缓冲、文本宽度
│     ├─ render\               渲染能力表 + ImGui 宿主 / D3D11 / 控件库 / 部件层
│     ├─ layout\               布局对象、中间态、解析、渲染出口、几何解算
│     └─ dispatch\             调度层：事件注入、场景装配、每帧流程、退出判据
├─ tools\json2bin.py           构建期 bin 生成（与 GenUI 同一格式与脚本）
├─ Kanat.json                  布局文档（工程源）
├─ CMakeLists.txt              工程定义
└─ docs\                       工程纲领、范例判据、派生清单、接口文档体例
```

`src` 下每个源文件模块配一份接口文档（与源码同目录），共 28 份；framework 层的代码源自 GenUI 快照（已就地演进），本仓库保留单文档场景需要的最小子集。

## 与 GenUI 的关系

| 层 | 本工程 | 说明 |
| :--- | :--- | :--- |
| 框架内核 / 基础设施 | 起点为 GenUI 快照 | 逐字节复制起点，随后就地演进（漂移登记见 `docs\DERIVE.md`），可随时回灌上游 |
| 文档层 / 编辑器层 / 应用层 / 宿主层 | 不引入 | 单布局场景不需要条目表、编辑命令、文件对话框与多文档协调 |
| 调度层 | 本工程形态 | `Dispatch` 是 GenUI `AppHost` 的单窗口精简形态，建窗与消息循环归 `ImGuiHost` |
| 示例层 | `demo\DemoEvents` | 唯一业务代码：两张事件处理函数 |

派生新项目的清单与步骤见 `docs\DERIVE.md`；工程纲领与范例判据见 `docs\CHARTER.md`、`docs\EXAMPLES.md`。

## 已知限制

- 无编辑器能力：不能增删控件、改类型或写盘。
- 交互观感（拖动、悬停、输入法）需在本机人工复核。
- 控件绘制与几何口径以框架内核为准，本工程不另做特例。
