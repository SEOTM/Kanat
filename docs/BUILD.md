# 构建与运行

## 1. 发布包内容

发布树包含：

```text
Kanat/
├─ src/                       框架与 Demo 源码
├─ docs/                      工程文档
│  ├─ examples/Kanat.json     Demo 布局文档
│  └─ LICENSE.md              MIT 许可
└─ README.md
```

不包含 CMake、Visual Studio 工程、第三方依赖、预编译产物或构建脚本。用户按本文自行建立工程并准备依赖。

## 2. 环境要求

| 项 | 要求 |
| :--- | :--- |
| 系统 | Windows 10 / 11 x64 |
| Visual Studio | 2022（17.10 或更高）或 2026；安装“使用 C++ 的桌面开发”工作负载 |
| 平台 | x64 |
| 语言标准 | C++17（C++20 亦可编译） |
| 第三方依赖 | Dear ImGui 1.92.8（Win32 + DX11 后端）、RapidJSON 1.1.0 |

不需要 CMake、NuGet 或 Python。依赖获取方式见第 3 节。

### 语言标准

源码按 C++17 编译。使用的现代特性均属 C++17：`inline constexpr` 变量、`if constexpr`、`template <auto>`；没有使用 C++20 专属特性。`RenderApi` 能力表按成员声明顺序初始化，C++17 可直接编译。

| 标准 | 结果 |
| :--- | :--- |
| C++17 | 通过，0 警告 0 错误（推荐） |
| C++20 | 通过，0 警告 0 错误（保留 `/Zc:char8_t-`，`u8"..."` 仍为 `char*`） |
| C++14 | 失败：`inline constexpr` 变量需要 C++17 |

C++14 需要同时改写 `inline` 变量、`if constexpr`、`template <auto>` 等多处特性，不作为支持目标。

## 3. 准备依赖

### 方式一：直接使用 ImGui 源码（推荐）

1. 获取 Dear ImGui **1.92.8** 源码；
2. 获取 RapidJSON **1.1.0** 源码（header-only，只需要 `include/rapidjson`）；
3. 把 ImGui 的以下源文件加入你的工程：
   - `imgui.cpp`
   - `imgui_draw.cpp`
   - `imgui_tables.cpp`
   - `imgui_widgets.cpp`
   - `backends/imgui_impl_win32.cpp`
   - `backends/imgui_impl_dx11.cpp`

ImGui 与 RapidJSON 均以 MIT License 分发。

### 方式二：vcpkg（可选）

若已安装 vcpkg，可安装静态库版本：

```powershell
vcpkg install imgui[core,win32-binding,dx11-binding]:x64-windows-static
vcpkg install rapidjson:x64-windows-static
```

然后在工程中包含目录指向 `installed\x64-windows-static\include`，并链接 `imgui.lib`。此时不再把 ImGui 的 `.cpp` 加入工程。

## 4. 用 Visual Studio 构建

### 4.1 建立工程

1. 新建“空项目”（C++，Windows，控制台应用），平台选择 `x64`；
2. 把 `src/` 下全部 `.cpp` 加入项目；
3. 若使用 ImGui 源码，把第 3 节的 6 个 ImGui `.cpp` 也加入项目；
4. 不需要加入 `src/` 下的 `.md` 文件。

### 4.2 工程属性

| 位置 | 设置 |
| :--- | :--- |
| C/C++ → 常规 → 附加包含目录 | `src`；ImGui 根目录；ImGui `backends`；RapidJSON 的 `include` 目录（其下为 `rapidjson/`，或 vcpkg 的 `include`） |
| C/C++ → 语言 → C++ 语言标准 | ISO C++17（`/std:c++17`） |
| C/C++ → 代码生成 → 运行库 | Release：多线程 `/MT`；Debug：多线程调试 `/MTd` |
| C/C++ → 预处理器 → 预处理器定义 | `_AMD64_;UNICODE;_UNICODE` |
| C/C++ → 命令行 → 其他选项 | `/utf-8 /Zc:char8_t-` |
| 链接器 → 系统 → 子系统 | 控制台 |
| 链接器 → 输入 → 附加依赖项 | `d3d11.lib;dxgi.lib;dwmapi.lib;imm32.lib` |

`user32.lib`、`gdi32.lib` 等 Windows 默认库由 Visual Studio 工程自动带入；若手工配置链接器，需一并加入。

### 4.3 生成与运行

选择 `Release|x64` 生成，得到 `Kanat.exe`。把 `docs/examples/Kanat.json` 复制到 exe 同目录并命名为 `Kanat.json`，然后运行：

```powershell
.\Kanat.exe
```

## 5. 用 cl 命令行构建

在“Developer PowerShell for VS 2022/2026”中执行。先设置依赖路径和输出目录：

```powershell
$imgui = "C:\deps\imgui-1.92.8"
$rapidjson = "C:\deps\rapidjson-1.1.0\include"
New-Item -ItemType Directory -Force out | Out-Null
```

枚举本项目全部源文件，并加入 ImGui 源文件：

```powershell
$src = Get-ChildItem src -Recurse -Filter *.cpp | ForEach-Object { $_.FullName }
$imguiSrc = @(
    "$imgui\imgui.cpp",
    "$imgui\imgui_draw.cpp",
    "$imgui\imgui_tables.cpp",
    "$imgui\imgui_widgets.cpp",
    "$imgui\backends\imgui_impl_win32.cpp",
    "$imgui\backends\imgui_impl_dx11.cpp"
)
```

编译并链接：

```powershell
cl /nologo /std:c++17 /EHsc /MT /O2 /utf-8 /Zc:char8_t- `
   /D_AMD64_ /DUNICODE /D_UNICODE `
   /I src /I $imgui /I "$imgui\backends" /I $rapidjson `
   $src $imguiSrc `
   /Fe:out\Kanat.exe `
   /link d3d11.lib dxgi.lib dwmapi.lib imm32.lib user32.lib gdi32.lib
```

复制布局文档并运行：

```powershell
Copy-Item docs\examples\Kanat.json out\Kanat.json
.\out\Kanat.exe
```

Debug 版本把 `/MT /O2` 改为 `/MTd /Od /Zi`，并把输出改为 `out\KanatDebug.exe`。

如果使用 vcpkg 或已有预编译 ImGui 库，去掉 `$imguiSrc`，在 `/link` 后加入 `imgui.lib` 的完整路径，并把包含目录指向对应的 `include`。

## 6. 运行参数

工作目录必须是可执行文件所在目录，布局文档按 exe 同目录解析。加载链优先 `Kanat.bin`，其次 `Kanat.json`。

| 环境变量 | 作用 |
| :--- | :--- |
| `DEMO_DOC` | 覆盖布局文档文件名（缺省 `Kanat.json`） |
| `DEMO_AUTOCLOSE_MS` | 启动后 N 毫秒自动退出；不设则常驻 |

示例：

```powershell
$env:DEMO_DOC = "Another.json"
$env:DEMO_AUTOCLOSE_MS = 1500
.\Kanat.exe
```

退出方式：`ESC` 或 `Alt+F4`。启动时控制台会打印事件绑定数、文档路径、控件数、控件层次与坐标、建窗几何与周期渲染自证。

## 7. 布局文档

`docs/examples/Kanat.json` 是 Demo 的完整布局。字段、控件类型、尺寸因子与二进制格式见 [LAYOUT-FORMAT.md](LAYOUT-FORMAT.md)。

运行期同时支持 JSON 与 `.bin`。发布包不附带 JSON → bin 的生成工具；如需二进制形态，按格式文档自行生成。若 exe 同目录同时存在 `Kanat.bin` 与 `Kanat.json`，加载链优先 `.bin`。

## 8. 排障

| 现象 | 原因与处置 |
| :--- | :--- |
| 编译报找不到 `imgui.h` | 附加包含目录未包含 ImGui 根目录与 `backends` |
| 编译报找不到 `rapidjson/document.h` | 附加包含目录未包含 RapidJSON 的 `include` 目录 |
| 链接报 `LNK2019`（ImGui 符号） | 未加入 ImGui `.cpp`，也未链接 `imgui.lib` |
| 链接报 `LNK2038` / 运行库不匹配 | 工程与 ImGui 库的 `/MT`、`/MTd` 不一致；统一运行库 |
| 链接报 `LNK2019`（`CreateDXGIFactory` 等） | 未链接 `d3d11.lib`、`dxgi.lib`、`dwmapi.lib`、`imm32.lib` |
| 编译报 `C7555` 或 C++20 语法错误 | 语言标准未设为 C++17 或更高 |
| 启动弹窗“应用初始化失败” | 工作目录不对，或 `Kanat.json` 不在 exe 同目录 |
| 修改 JSON 后界面没变 | 同目录存在旧的 `Kanat.bin`，加载链优先 bin；删除或重新生成 |
| 窗口不出现 | 需要可交互的 Windows 桌面会话 |

运行日志中的 `[DEMO]` 前缀来自 `src/Identity.h` 的 `IDENTITY_LOG_PREFIX`。
