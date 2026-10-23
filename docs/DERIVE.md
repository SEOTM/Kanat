# 派生新应用

Kanat 是“框架 + 最小 Demo”的仓库。派生新应用时，把整个仓库复制出去，替换应用身份与业务事件即可；框架层不需要改动。

## 1. 复制与改名

```powershell
Copy-Item -Recurse Kanat MyApp
Remove-Item -Recurse -Force MyApp\.git
```

在新目录初始化自己的 Git 仓库：

```powershell
cd MyApp
git init
```

## 2. 必改项

| 项 | 位置 | 现值 | 说明 |
| :--- | :--- | :--- | :--- |
| 项目名 / 目标名 | `CMakeLists.txt` 的 `APP_PROJECT_NAME` | `Kanat` | 决定解决方案、工程与可执行文件名 |
| 布局文档名 | `CMakeLists.txt` 的 `APP_DOC_NAME` + `src/Identity.h` 的 `IDENTITY_DOC_NAME` | `Kanat.json` | 两处必须一致；构建期按此名生成 `.bin` |
| 应用名 | `src/Identity.h` 的 `IDENTITY_APP_NAME` | `Kanat` | 派生窗口类名 `<APPNAME>_IMGUI_CLASS` 与默认窗口标题；窗口类名须进程外唯一 |
| 错误弹窗标题 | `IDENTITY_ERR_TITLE` | `暗渠框架 Kanat 错误` | 启动失败时显示 |
| 文档覆盖环境变量 | `IDENTITY_DOC_ENV` | `DEMO_DOC` | 与同机其他工具的环境变量约定冲突时改 |
| 自动退出环境变量 | `IDENTITY_ENV_AUTOCLOSE` | `DEMO_AUTOCLOSE_MS` | 同上 |
| 日志前缀 | `IDENTITY_LOG_PREFIX` | `DEMO` | 日志行首 `[DEMO]` |
| 拖动窗口事件名 | `IDENTITY_EVENT_DRAG_WINDOW` + 布局文档 `event` | `DEMO_DRAG_WINDOW` | 只改一侧会静默失绑 |
| 委托演示事件名 | `IDENTITY_EVENT_ENTRUST_PROBE` + 布局文档 `event` | `DEMO_ENTRUST_PROBE` | 同上 |
| 布局名 / 标签 | 布局文档的 `layouts[0].name` / `label` | `DemoMain` / `暗渠框架 Demo` | 只影响日志与标识 |
| 业务事件表 | `src/demo/DemoEvents.cpp` | 两条演示事件 | 换成自己的处理函数 |

`src/Identity.h` 共 8 个常量：3 个文档 / 运行开关，3 个应用与日志身份，2 个事件名。

## 3. 建议改动

- 把 `src/demo/` 改名为业务名（如 `src/app/`），并同步 `#include` 与 `Dispatch::Init` 传入的绑定表函数；
- 把 `Kanat.json` 改名为 `App.json`，重写布局与控件；
- 更新 `README.md` 与 `docs/` 中的项目名、示例路径；
- 如果不再需要事件委托演示，删除 `CTRL_EVENT_ENTRUST_PARENT` 相关控件与处理器；
- 如果产品只需要单窗口，保留 `DispatchConfig::dwWndCount = 1`；多窗口时按 `ImGuiHost::MAX_WND`（8）声明。

## 4. 派生后自证

1. **配置与构建**：零警告、零错误；
2. **启动**：在 exe 目录运行，退出码 0；
3. **文档**：控制台“场景载入”行的路径与新文档名一致；
4. **控件**：日志中的控件数、层次与坐标符合预期；
5. **事件**：点击 / 拖动绑定了事件的控件，处理函数被调用；
6. **交互与外观**：拖动、悬停、输入法需本机人工复核。

启动日志示例：

```text
[DEMO] 场景载入: ...\Kanat.bin (布局=DemoMain 控件=15 窗口=...)
[DEMO] 事件绑定: 3 枚控件已绑定处理器(其中委托父容器 1 枚, 已解析 1 枚)
[DEMO] 控件层次: 0=0 1=0 ...
[DEMO] 控件坐标: 0=(180,120) ...
```

## 5. 不要改的部分

- `src/framework/` 的分层与路径：派生时整体带走；
- `third_party/Kanat.Deps.Native.1.0.0.nupkg` 与 `CMakeLists.txt` 的依赖解包逻辑：除非升级依赖版本；
- `tools/json2bin.py`：除非同步修改 C++ 侧的 bin 格式契约；
- 28 份模块接口文档：路径与模块名按 `src/` 相对路径书写，改名工程不需要逐份修改。

## 6. 常见错误

| 现象 | 原因 |
| :--- | :--- |
| 启动弹窗“应用初始化失败” | `APP_DOC_NAME` 与 `IDENTITY_DOC_NAME` 不一致，或文档不在 exe 同目录 |
| 控件点击无反应 | 布局 `event` 与绑定表事件名不一致；检查大小写与拼写 |
| 拖动窗口无反应 | 标题栏 `event` 未绑定 `IDENTITY_EVENT_DRAG_WINDOW`，或布局中没有 `CTRL_TYPE_WND` |
| 窗口类注册失败 | `IDENTITY_APP_NAME` 与同机其他程序重名；窗口类名须进程外唯一 |
| 日志前缀仍是 `[DEMO]` | 只改了 `Identity.h` 但未重新构建，或改的是错误副本 |
