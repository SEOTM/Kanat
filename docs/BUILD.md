# 构建与运行

## 1. 环境要求

| 项 | 要求 |
| :--- | :--- |
| 系统 | Windows 10 / 11 x64 |
| Visual Studio | 2022（17.10 或更高）或 2026；安装“使用 C++ 的桌面开发”工作负载 |
| CMake | 3.20 或更高（`file(ARCHIVE_EXTRACT)` 需要 3.18+） |
| Python | 3.8+，可选；用于构建期生成 `Kanat.bin` |

框架使用 C++17、静态运行库 `/MT`、Unicode 与 `_AMD64_`，不依赖 vcpkg、NuGet 源或网络。

### 语言标准

当前源码按 **C++17** 编译，`CMakeLists.txt` 的 `CMAKE_CXX_STANDARD` 设为 17。

源码使用的现代特性均属 C++17：`inline constexpr` 变量、`if constexpr`、`template <auto>`；没有使用 C++20 专属特性。`RenderApi` 能力表按成员声明顺序初始化，因此 C++17 可直接编译。

兼容性结论（本机实测）：

| 标准 | 结果 |
| :--- | :--- |
| C++17 | 通过，0 警告 0 错误（当前配置） |
| C++20 | 通过，0 警告 0 错误（保留 `/Zc:char8_t-`，`u8"..."` 仍为 `char*`） |
| C++14 | 失败：`inline constexpr` 变量需要 C++17（C7525） |

C++14 需要同时改写 `inline` 变量、`if constexpr`、`template <auto>` 等多处特性，不作为支持目标。

## 2. 依赖

第三方依赖以离线归档形式放在 `third_party/`：

```text
third_party/
├─ Kanat.Deps.Native.1.0.0.nupkg    Dear ImGui 1.92.8 静态库 + RapidJSON 1.1.0 头文件
└─ README.md                        来源、版本与许可
```

CMake 在配置期把归档解包到 `${CMAKE_BINARY_DIR}/third_party/kanat-deps`，然后把：

- `build/native/include` 加入头文件搜索路径；
- `build/native/lib/debug/imguid.lib` 链接到 Debug 配置；
- `build/native/lib/release/imgui.lib` 链接到其他配置。

归档本身是 NuGet 包格式，但构建过程不调用 NuGet，也不访问包源。更新依赖时替换归档并清理构建目录即可。

## 3. 构建

### Visual Studio 2022

```powershell
cmake -S . -B out -G "Visual Studio 17 2022" -A x64
cmake --build out --config Release --parallel
```

### Visual Studio 2026

```powershell
cmake -S . -B out -G "Visual Studio 18 2026" -A x64
cmake --build out --config Release --parallel
```

生成器名称必须与本机安装的 Visual Studio 版本一致；`cmake --help` 会列出可用生成器。切换生成器前先删除 `out/`，否则 CMake 会保留旧缓存。

### Debug

```powershell
cmake --build out --config Debug --parallel
```

Debug 使用 `/MTd` 并链接 `imguid.lib`。

### 产物

```text
out/
├─ Release/
│  ├─ Kanat.exe
│  └─ Kanat.bin
├─ Debug/
│  ├─ Kanat.exe
│  └─ Kanat.bin
└─ third_party/kanat-deps/          配置期解包的依赖
```

`Kanat.bin` 由 `tools/json2bin.py` 从 `Kanat.json` 生成，并复制到可执行文件同目录。找不到 Python 时 CMake 给出警告，改为复制 `Kanat.json`；运行期两种形态都能加载。

### 清理重建

```powershell
Remove-Item -Recurse -Force out
cmake -S . -B out -G "Visual Studio 17 2022" -A x64
cmake --build out --config Release --parallel
```

不要删除 `third_party/`；它是构建输入。

## 4. 运行

工作目录必须是可执行文件所在目录：

```powershell
cd out\Release
.\Kanat.exe
```

布局文档按 exe 同目录解析。加载链优先 `Kanat.bin`，其次 `Kanat.json`；形态由内容嗅探判定。

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

退出方式：`ESC` 或 `Alt+F4`。启动时控制台会打印事件绑定数、文档路径、控件数、控件层次与坐标、建窗几何与周期渲染自证，可用于核对布局是否按预期展开。

## 5. 持续集成

`.github/workflows/build.yml` 在 `windows-latest` 上执行：

1. 检出仓库；
2. 安装 Python；
3. 用 Visual Studio 2022 生成器配置；
4. 构建 Release；
5. 校验 `out/Release/Kanat.exe` 与 `out/Release/Kanat.bin` 存在。

CI 只构建，不启动图形界面。

## 6. 排障

| 现象 | 原因与处置 |
| :--- | :--- |
| `Could not create named generator` | 生成器名称与本机 VS 不符；用 `cmake --help` 查看可用名称 |
| 配置报 `CMake 3.20 or higher is required` | 升级 CMake，或使用 VS 自带的 CMake |
| 配置报 `Missing dependency archive` | `third_party/Kanat.Deps.Native.1.0.0.nupkg` 缺失或被移动 |
| 编译报找不到 `imgui.h` | 依赖解包失败；删除 `out/` 后重新配置 |
| 构建警告“未找到 Python” | 安装 Python 3 并确保 `python` 在 `PATH`，或接受复制 JSON 的回退 |
| 启动弹窗“应用初始化失败” | 工作目录不对，或 `Kanat.bin` / `Kanat.json` 不在 exe 同目录；看控制台首行日志 |
| 窗口不出现 | 检查是否在无桌面会话中运行；本程序需要可交互的 Windows 桌面 |
| 链接报 `LNK1104` | 上一次的 `Kanat.exe` 仍在运行；关闭后重试 |
| 切换 VS 版本后构建异常 | 删除 `out/`，用新生成器重新配置 |

运行日志中的 `[DEMO]` 前缀来自 `Identity.h` 的 `IDENTITY_LOG_PREFIX`；派生新项目时可改。
