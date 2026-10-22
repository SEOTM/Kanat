# DemoEvents 模块 (src\demo\DemoEvents.h)

## 概述

Demo 的事件层:事件名到处理函数的绑定表,以及本 Demo 的两条事件(标题栏拖动窗口 · 事件委托演示)。

绑定表的内容属应用侧,而框架层的 `UIEventHandler` 不得反向依赖应用侧,因此由 `Dispatch` 在启动期经
函数指针取得本表并注入框架侧解析器;本模块只提供表与处理器,不持有解析器状态。本层不认识窗口句柄:拖动只把
"窗口期望坐标"记进布局的窗口位置记录(WND 控件的 `position`),窗口的实质移动归框架层(渲染出口取得渲染目标时结算)。

本 Demo 保留两条绑定:标题栏拖动窗口,以及事件委托演示的父事件(容器绑定它,子控件以
`CTRL_EVENT_ENTRUST_PARENT` 委托到它);其余 `event` 名一律解析为空绑定(控件无响应)。

本模块包含以下编程接口:

### 函数

| 函数 | 说明 |
| --- | --- |
| `GetBindings` | 取本 Demo 的事件绑定表(两条:标题栏拖动窗口 · 事件委托演示的父事件)。 |
| `OnDragWindow` | 拖动窗口的事件处理器:标题栏按住拖动 = 把窗口期望坐标记进窗口位置记录。 |
| `OnEntrustProbe` | 事件委托演示的父事件处理器:子控件(委托父容器)的点击经父容器的事件到达这里。 |

本模块不定义公开类型与公开常量。

---

## GetBindings 函数 (DemoEvents)

取本 Demo 的事件绑定表。

### 语法

```cpp
CONST UIEventHandler::EventBinding* GetBindings(size_t* pnCount);
```

### 参数

`[out] pnCount`

类型: **size_t\***

接收绑定表条目数。可为 `NULL`(`NULL` 时不写回);写入值为编译期数组长度。

### 返回值

类型: **CONST UIEventHandler::EventBinding\***

绑定表首址,**恒非 `NULL`**。表为文件内静态数组,进程级常驻;框架侧只存指针、不复制,
因此调用方不得释放,也不得在注入后替换表内容。

### 注解

调用时机为宿主启动期,且必须早于场景载入——展开布局时即按 `event` 名解析处理器,晚于载入则控件
拿不到处理器。

绑定表条目按名字形态分两种匹配方式:完整名(条目的 `chDigitLo` 为 `'\0'`)按精确匹配;
前缀家族(`chDigitLo` 非 `'\0'`)按"前缀 + 一位数字"匹配,数字落在声明区间内才命中。

---

## OnDragWindow 函数 (DemoEvents)

标题栏按住拖动的事件处理器。

### 语法

```cpp
VOID OnDragWindow(VOID* obj, uint64_t oper, uint64_t param, void* data);
```

### 参数

`[in] obj`

类型: **VOID\***

触发的标题控件对象(`CtrlInfo*`)。本处理器不需要该对象,传入后忽略。

`[in] oper`

类型: **uint64_t**

规范操作码。本处理器只读公共段的 `OPER_COMMON_HOLD`(按住中);该位是电平位,条件成立期间
逐帧置位,因此处理器在按住期间每帧更新一次位置记录。

`[in] param`

类型: **uint64_t**

通用参数槽。本事件承载"窗口期望坐标"的打包值(桌面像素坐标),打包与解包口径见
`ControlSpec.h` 的 `PackWndPos` 与 `UnpackWndPos`;未拖动时该值为 `0`。

`[in] data`

类型: **void\***

该控件的缓冲(标题文本)。本处理器不需要,传入后忽略。

### 返回值

类型: **VOID**

此函数不返回值。

### 注解

处理器不做任何窗口 API 调用:它把解包后的坐标写进布局窗口件(WND 控件)的 `position`,
由渲染出口在下次取得该窗口渲染目标时结算成实质移动(渲染能力表的 `AcquireWnd`)。

未绑定本事件的窗口,标题栏上按住不会产生位置记录,窗口因此不可拖动——可拖动与否由事件绑定表达,
而不是在标题栏绘制代码里判断。

操作码不满足 `OPER_COMMON_HOLD` 时直接返回;布局缺失窗口件时同样直接返回。标题栏右上关闭钮的
专属码(`OPER_STRUCT_TITLE_CLOSE`)在本 Demo 下不接行为,退出窗口走 ESC 或系统关闭(Alt+F4)。

---

## OnEntrustProbe 函数 (DemoEvents)

事件委托演示的父事件处理器:子控件(委托父容器)的点击经父容器的事件到达这里。

### 语法

```cpp
VOID OnEntrustProbe(VOID* obj, uint64_t oper, uint64_t param, void* data);
```

### 参数

`[in] obj`

类型: **VOID\***

触发本事件的控件对象(`CtrlInfo*`)。对委托者是**委托者本身**(不是父容器)—— 处理函数来自父,
控件对象仍是子控件,本处理器用它打印到达自证。

`[in] oper`

类型: **uint64_t**

规范操作码。本处理器只认公共段的 `OPER_COMMON_CLICKED`(完整单击);其余操作码直接返回。

`[in] param`

类型: **uint64_t**

通用参数槽。本演示不使用(传入后忽略)。

`[in] data`

类型: **void\***

该控件的缓冲。本处理器不需要,传入后忽略。

### 返回值

类型: **VOID**

此函数不返回值。

### 注解

**演示链路**:`Demo.json` 的容器(`##CONTAINER`)绑定 `DEMO_ENTRUST_PROBE`;委托按钮
(`##DEMO_ENTRUST`,按钮类型)声明 `parent` = 容器的 `id` 且 `event` = `CTRL_EVENT_ENTRUST_PARENT`。
装配期 `LayoutBuilder::ResolveEntrustedEvents` 把容器的处理函数写给该按钮 ⇒ 点击该按钮时到达本处理器,
打印 `控件 id=14 label=##DEMO_ENTRUST`。

到达自证只打印、不改任何状态(无人值守运行不点击 ⇒ 无额外输出,日志基线不受影响);装配期"委托已解析"
的客观判据是场景自证行中的委托枚数,见 `Dispatch` 的场景载入日志(`DispatchScene.cpp`)。


---

## 要求

| 要求 | 值 |
| --- | --- |
| 标头 | `demo/DemoEvents.h`(`src\demo\DemoEvents.h`) |
| 实现 | `src\demo\DemoEvents.cpp` |
| 命名空间 | `DemoEvents` |
| 依赖 | `Identity.h`(事件名与日志前缀) · `framework/dispatch/UIEventHandler.h` · `framework/base/ControlSpec.h` · `framework/dispatch/Dispatch.h` · `windows.h` · `ucrt` |
| 平台 | Windows 全版本 |
| 线程安全 | 否;在框架层的帧流程内被调用,须与布局访问串行 |
