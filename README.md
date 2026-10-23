# Kanat（暗渠框架）

Kanat 是一个 Windows 原生的声明式界面框架。应用用一份布局文档描述界面，框架把文档解析为布局对象、解算像素几何、生成渲染路径，并把控件事件交回应用代码。框架内核不包含任何应用身份；仓库同时附带一个最小 Demo，演示如何用 Kanat 构建单窗口界面。

> GitHub 发布树只保留 `src/` 与 `docs/`（另含本 README 与 `docs/LICENSE.md`）；不附带 CMake、Visual Studio 工程、第三方依赖或预编译产物。用户按 [docs/BUILD.md](docs/BUILD.md) 自行建立工程并准备依赖。

## 特性

- **布局文档是唯一真源**：界面结构、控件、几何参考量与事件名都写在 JSON 中；运行期同时支持 JSON 与二进制 `.bin` 两种形态。
- **自适应几何**：每个控件的 `x` / `y` / `width` / `height` 由 8 个参考量系数加像素偏移解算；窗口尺寸变化时整体重算，不在代码里硬编码像素。
- **渲染后端解耦**：布局渲染只依赖 `RenderApi` 与 `CtrlApi` 两张能力表。当前实现是 Dear ImGui + D3D11；替换 UI 技术时提供另一套实现并注入即可，布局与调度代码不改，控件库可按同一 `CtrlApi` 另行实现。
- **声明式控件库**：22 种控件类型（20 种有绘制实现），父子链同时驱动绘制层次、坐标累加与事件委托。
- **事件三段式**：文档 `event` 字段 → 应用侧绑定表 → 处理函数。框架不反向依赖应用层。
- **依赖明确**：Dear ImGui 1.92.8（Win32 / DX11 后端）与 RapidJSON 1.1.0；均由用户自行准备，版本与配置见构建指南。

## 快速开始

### 环境要求

| 项 | 要求 |
| :--- | :--- |
| 系统 | Windows 10 / 11 x64 |
| 编译器 | Visual Studio 2022（17.10+）或 Visual Studio 2026，安装“使用 C++ 的桌面开发”工作负载 |
| 语言标准 | C++17（C++20 亦可编译，见 [docs/BUILD.md](docs/BUILD.md)） |
| 依赖 | Dear ImGui 1.92.8（Win32 + DX11 后端）、RapidJSON 1.1.0，由用户自备 |

### 构建

发布包不含构建工程，需自行建立 Visual Studio 项目或直接用 `cl` 编译。最小步骤：

1. 新建 x64 C++ 空项目，把 `src/` 下全部 `.cpp` 加入；
2. 加入 ImGui 源文件：`imgui.cpp`、`imgui_draw.cpp`、`imgui_tables.cpp`、`imgui_widgets.cpp`、`backends/imgui_impl_win32.cpp`、`backends/imgui_impl_dx11.cpp`；
3. 配置：C++17、`/MT`（Debug 用 `/MTd`）、Unicode、`/utf-8 /Zc:char8_t-`；
4. 包含目录：`src`、ImGui 根目录、ImGui `backends`、RapidJSON 的 `include` 目录（其下为 `rapidjson/`）；
5. 链接：`d3d11.lib`、`dxgi.lib`、`dwmapi.lib`、`imm32.lib`；
6. 生成后把 [docs/examples/Kanat.json](docs/examples/Kanat.json) 复制到 exe 同目录，命名为 `Kanat.json`。

完整的属性表、命令行 `cl` 示例与排障见 [docs/BUILD.md](docs/BUILD.md)。

### 运行

工作目录必须是可执行文件所在目录，布局文档按 exe 同目录解析：

```powershell
cd <exe 所在目录>
.\Kanat.exe
```

启动后按住标题栏可拖动窗口；`ESC` 或 `Alt+F4` 退出。`DEMO_AUTOCLOSE_MS` 可让程序在指定毫秒后自动退出，便于无人值守验证：

```powershell
$env:DEMO_AUTOCLOSE_MS = 1500
.\Kanat.exe
```

完整的构建选项、Debug 构建与排障见 [docs/BUILD.md](docs/BUILD.md)。

## 界面如何描述

一份布局文档是一个 `layouts` 数组。每个布局包含布局级元数据、参考尺寸与控件数组：

```json
{
  "layouts": [
    {
      "name": "DemoMain",
      "label": "Kanat Demo",
      "width":  { "coeff": [0, 0, 1, 0, 0, 0, 0, 0], "offset": 0 },
      "height": { "coeff": [0, 0, 0, 1, 0, 0, 0, 0], "offset": 0 },
      "autosize": {
        "mode": "AUTO_SIZE_BY_SCREEN_HEIGHT",
        "unit": 0,
        "width": 570,
        "height": 500,
        "controlHeight": 25,
        "fontSize": 13,
        "space": 10,
        "padding": 4
      },
      "ctrls": [
        {
          "id": 0,
          "type": "CTRL_TYPE_WND",
          "label": "Kanat Demo",
          "parent": -1,
          "x": { "coeff": [0,0,0,0,0,0,0,0], "offset": 180 },
          "y": { "coeff": [0,0,0,0,0,0,0,0], "offset": 120 },
          "width":  { "coeff": [0,0,1,0,0,0,0,0], "offset": 0 },
          "height": { "coeff": [0,0,0,1,0,0,0,0], "offset": 0 }
        }
      ]
    }
  ]
}
```

控件位置与尺寸的通用公式是：

```text
像素值 = Σ(coeff[i] × 参考量[i]) + offset
```

八个参考量依次为屏幕宽、屏幕高、窗口宽、窗口高、控件高、字号、间隔、内边距。字段含义、控件类型、二进制格式与校验规则见 [docs/LAYOUT-FORMAT.md](docs/LAYOUT-FORMAT.md)。

## 架构

```text
main.cpp
  └─ Dispatch（调度层：初始化 / 每帧流程 / 退出判据 / 会话内存）
       ├─ LayoutSolver     JSON / bin → 布局模型（FileMidState）
       ├─ LayoutBuilder    控件信息与控件缓冲
       ├─ GeometrySolver   参考量 → 像素几何
       ├─ Layout           布局对象（线性或链式存储）
       └─ LayoutRender     按 RenderApi 渲染一份布局
            ├─ CtrlApi     控件绘制能力（Controls / Widgets）
            └─ RenderApi   窗口 / 容器 / 光标 / 命中判定
                 └─ ImGuiRender → ImGui → D3D11 → 窗口（ImGuiHost）

DemoEvents（应用侧） ──绑定表──> UIEventHandler ──> CtrlInfo::pEventHandler
```

框架分为四层：

| 层 | 目录 | 职责 |
| :--- | :--- | :--- |
| 基础 | `src/framework/base/` | 设计参考量、控件类型契约、控件缓冲、文本宽度、内存分配器、文件校验 |
| 布局 | `src/framework/layout/` | 布局模型、JSON/bin 解析、几何解算、布局对象、渲染出口 |
| 渲染 | `src/framework/render/` | 渲染能力接口；`imgui/` 下的宿主、D3D11 设施、控件库与部件层 |
| 调度 | `src/framework/dispatch/` | 场景装配、事件绑定、每帧流程、退出判据、资源与路径 |

`src/demo/` 是唯一业务代码；`src/main.cpp` 只做初始化、启动、主循环与释放的胶水。各层依赖方向、数据流与扩展点见 [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)。

## 目录结构

```text
Kanat/
├─ src/
│  ├─ main.cpp                  进程入口
│  ├─ Identity.h                项目身份唯一真源
│  ├─ demo/                     Demo 事件层
│  └─ framework/                框架内核
│     ├─ base/                  基础设计量、控件契约、缓冲、文本宽度、内存、校验
│     ├─ layout/                布局模型、解析、几何、布局对象、渲染出口
│     ├─ render/                渲染能力接口与 ImGui / D3D11 实现
│     └─ dispatch/              调度层、场景装配、事件、资源路径
├─ docs/                        工程文档（架构、构建、格式、派生、体例）
│  └─ examples/Kanat.json       Demo 布局文档
└─ README.md
```

`src/` 下每个源文件模块都在同目录配一份接口文档（共 28 份）；契约细节以这些文档为唯一真源，代码只保留函数的 `函数 / 描述` 两行注。

## 文档

| 文档 | 内容 |
| :--- | :--- |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | 分层、数据流、能力表、事件模型、几何与内存模型、扩展点 |
| [docs/BUILD.md](docs/BUILD.md) | 构建、运行、Debug、CI 与排障 |
| [docs/LAYOUT-FORMAT.md](docs/LAYOUT-FORMAT.md) | 布局文档字段、控件类型、尺寸因子、bin 格式 |
| [docs/DERIVE.md](docs/DERIVE.md) | 从本仓库派生新应用 |
| [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md) | 编码风格、注释与提交约定 |
| [docs/INTERFACE-DOC-STYLE.md](docs/INTERFACE-DOC-STYLE.md) | 接口文档体例 |
| `src/**/<模块>.md` | 逐模块接口契约 |

## Demo 说明

- 一份最小单布局文档：窗口、标题栏与空内容容器，全部由 `Kanat.json` 声明。
- 标题栏按住拖动：事件把窗口期望坐标写入布局的窗口位置记录，渲染侧在下一次取得该窗口时结算几何。
- 除标题栏拖动外不绑定其它事件：界面结构完全由布局文档表达，增删控件只改 JSON。
- 布局来源：缺省加载链、内存缓冲、布局模型、布局对象数组、延迟装配五种入口，详见 `src/framework/dispatch/Dispatch.md`。

## 已知限制

- 没有编辑器能力：不能增删控件、修改类型或写回文档。
- Demo 只演示单窗口；框架支持多标准窗口与嵌套布局，但未提供完整范例。
- 交互观感（拖动、悬停、输入法）需在本机人工复核。
- 仅支持 Windows 与 MSVC；渲染后端目前只有 ImGui + D3D11。

## 许可

MIT License，见 [docs/LICENSE.md](docs/LICENSE.md)。Dear ImGui 与 RapidJSON 均为 MIT License，由用户自行获取。
