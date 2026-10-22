# main 模块 (src\main.cpp)

## 概述

Kanat(暗渠框架)的进程入口。本文件只做胶水: 应用初始化 → 渲染宿主启动(默认几何建窗 + 渲染设施) → 主循环 →
成对释放。启动失败时弹一条中文错误框并以非 `0` 退出码返回; 成功路径返回 `0`。

单窗口模式:调度层为 `framework\dispatch\Dispatch`(事件绑定注入 / 场景装配 / 投影配色 / 每帧流程 / 退出判据),
渲染宿主为 `framework\render\imgui\ImGuiHost`(窗口类与建窗 / ImGui 与 D3D11 设施 / 消息循环 / 窗口过程), 事件绑定表为
`demo\DemoEvents::GetBindings`。界面 = 一份布局对象的声明式渲染(结构件 `WND` / `TITLE` / `CNTR` + 内容控件);
事件 = 标题栏按住拖动窗口与事件委托演示; 无编辑器能力(不增删 / 不改类型 / 不写盘 / 无文件对话框)。

本模块包含以下编程接口:

### 函数

| 函数 | 说明 |
| --- | --- |
| `main` | 进程入口: 初始化应用、启动渲染宿主、运行主循环并按逆序释放。 |

---

## main 函数 (main)

进程入口。

### 语法

```cpp
int main(int, char**);
```

### 参数

`[in] argc`

类型: **int**

命令行参数个数。本工程不解析命令行, 该参数被忽略。

`[in] argv`

类型: **char\*\***

命令行参数数组。本工程不解析命令行, 该参数被忽略。

### 返回值

类型: **int**

`0` = 正常退出; `1` = 应用初始化失败或渲染宿主启动失败(失败原因见错误框与标准输出)。

### 注解

调用次序固定为七步:

1. `ImGuiHost::Register()` —— 取宿主接口表 `CONST HostApi*`(调度层只经该表访问宿主);
2. 组装 `DispatchConfig` —— 文档名 / 环境变量名 / 日志前缀取自 `Identity.h` 常量, 单窗口, 亮暗默认跟随系统;
3. `Dispatch::Init(pHost, cfg, DemoEvents::GetBindings)` —— 事件绑定注入、场景载入、投影配色与样式登记、无人值守参数;
4. `ImGuiHost::SetFontSize(Dispatch::GetFontSize())` —— 把布局参考字号交给具体宿主登记(建窗时作为字号);
5. `pHost->Start(Dispatch::GetWndCount(), IDENTITY_APP_NAME, Dispatch::OnSegment, NULL)` ——
   DPI 感知、按默认几何(客户区 800x600、屏幕工作区居中、已登记字号、圆角 10)建窗、建立 ImGui 与 D3D11 设施;
6. `pHost->Run(Dispatch::Frame, NULL, Dispatch::GetExitAddress())` —— 主消息循环, 每帧调一次调度层处理流程;
7. `pHost->Release()` + `pHost->DestroyWnds()` + `Dispatch::Release()` —— 关停渲染设施、销毁窗口与窗口类,
   再打印收尾自证并释放投影配色、场景与会话分配器。

第三步失败时弹错误框并返回 `1`(Dispatch 已在 `Init` 内部按阶段回退); 第五步失败时先补调
`Dispatch::Release()` 归还已建立的调度资源, 再弹错误框并返回 `1`。窗口类名由
`IDENTITY_APP_NAME` 派生(`<APPNAME>_IMGUI_CLASS`), 窗口标题默认取应用名, 首帧再按布局窗口件的标签更新。

进程的全部运行参数走环境变量, 不走命令行: 布局文档名由 `IDENTITY_DOC_ENV`(`DEMO_DOC`)覆盖, 自动退出时长由
`IDENTITY_ENV_AUTOCLOSE`(`DEMO_AUTOCLOSE_MS`)设置。常量定义见 `Identity.h`。

正常退出方式为 `ESC`、系统关闭(`Alt+F4` / 任务栏)或无人值守到点; 标题栏关闭钮由控件库绘制, 但本 Demo 不接
该操作码的行为。

---

## 要求

| 要求 | 值 |
| --- | --- |
| 标头 | 无(进程入口) |
| 实现 | `src\main.cpp` |
| 命名空间 | 全局作用域 |
| 依赖 | `Identity.h` · `framework/dispatch/Dispatch.h` · `demo/DemoEvents.h` · `framework/render/imgui/ImGuiHost.h` · `framework/render/Render.h` · `user32.dll` |
| 平台 | Windows(控制台子系统; 启动首件事为控制台代码页切 UTF-8) |
| 线程安全 | 不适用(进程入口, 单线程启动) |
