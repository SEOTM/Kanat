# Demo · 典范框架使用范例(草案)

> 状态: 草案 v0, 待用户确认。经确认后本文件成为本工程的范例纲领, 范例实物按本文件逐个落地。
> 上位口径: 工作区根 `AGENTS.md`(环境优先 / IPCHost 纪律) · `CodingPre\Habits_CPP.md`(编码习惯) ·
> GenUI 侧 `docs\HANDOFF.md` §1(设计原则) 与 §7-30(可迁移性判据)。
>
> **公开版说明**: 本仓库为公开版本, 已移除逐轮过程留痕(`docs\HISTORY\` 等)与内部工程管理文档
> (工作区根 `AGENTS.md`、GenUI 的 `docs\HANDOFF.md` 等)。文中指向这些文件的引用仅用于说明来源与沿革。

## 1. 本项目做什么

本工程只使用 GenUI 的 `framework`(24 件复制快照 + 本工程首产/新增的框架层件, 清单见 `README.md` §1)与
`common`(14 件逐字节复制快照), 外加自持的 `src\demo` 事件层(事件表); `app` / `doc` / `editor` / `tests`
四层整体不引入。首产件 = `framework\render\imgui\ImGuiHost.{h,cpp,md}`(2026-10-15 第 4 轮, 见 §3 的 EX-10),
协调层 = `framework\dispatch\Dispatch`(对应 GenUI 的 `app\Application`, 见 §2 第六例与 `docs\CHARTER.md` 纲领十)。

**两个用途**:

1. **给框架消费者照抄**: 一份"框架应当这样用"的范例集 —— 从最小可运行窗口到全控件、事件、数据流、
   布局源、几何自适应的完整用法示范, 每个范例都是可编译可运行的实物, 不是文档片段。
2. **反向治理 GenUI 的标杆**: 范例是消费方视角的"应当如此"。凡范例**做不到 / 必须绕过 / 必须复制
   应用侧代码**才能成立之处, 即框架的缺陷或非通用语义的实证 —— 逐条记入反向治理清单, 回灌 GenUI
   (路径与纪律见第 5 节)。

**为什么 Demo 能担当此任**: GenUI 侧 `HANDOFF.md` §7-30 的可迁移性判据问法是
"把它搬到另一个项目, `framework` 还能原样复用吗?" —— 过去这是**设问**(靠人工想象回答);
本工程是那个"另一个项目"的**真实实例**, 于是判据从设问变成可执行实验。

## 2. 典范判据(七条)

一个范例只有同时满足以下七条, 才可作为"典范"入库。任一条不满足即标为"非典范"并说明原因。

| 序号 | 判据 | 判据来源 |
| :--- | :--- | :--- |
| P1 | **只用 `framework` / `common` 的公开接口**, 不触碰 `app` / `doc` / `editor` 语义 | GenUI §7-30 可迁移性判据 |
| P2 | **界面全部声明在布局文档里**(唯一真源), 代码不硬编码界面元素 | 框架原则② |
| P3 | **数据经 `ctrl.buffer` 交互**: 类型 → 缓冲 → 渲染 = f(缓冲) → 事件 | 框架原则③ |
| P4 | **事件经"文档声明 `event` 名 → 绑定表 → 处理函数"三段式**; 绑定表属消费方, 框架不反向依赖 | 框架原则④ + GenUI `AP-12` 断环口径 |
| P5 | **几何量一律取自框架解算结果**(参考尺寸 / `sizeinfo`), 不硬编码像素 | GenUI §1.1-16 控件库绘制口径 |
| P6 | **资源成对管理**: `Init`/`Release`、分配器注入与解绑、缓冲建立与回收逐项配对 | `Habits_CPP.md` §6 |
| P7 | **可编译、可运行、有客观自证**: 构建零错零警 + 启动日志客观断言(+ 可选帧 dump 哈希); 外观与交互结论一律标"待本地人工复核" | GenUI §5 门禁口径 |

补充纪律: `framework` / `common` 两层是 GenUI 原文快照, **本工程不得就地改写其语义**; 需要框架没有的
能力时, 记入反向治理清单, 由 GenUI 侧按 `LOCK.md`(锁定面)与 `PLAN.md` 流程处置。

例外条(2026-10-15 第 4 轮, 用户明确要求): 经用户要求可在本工程**首产框架层新增件** —— 只新增独立成套
的文件(`.h` / `.cpp` / `.md`), **不回改**既有快照文件的语义; 首产件须在 `README.md` §1 / §6 与
`docs\DERIVE.md` §5 登记, 以免后续从 GenUI 重新复制 `framework` 层时被静默覆盖。首例 = `ImGuiHost`
(纲领三, 见 `docs\CHARTER.md`)。

第二例例外(2026-10-15 第 5 轮, 用户指令): 快照层**就地改名** —— 数组命名治理(数组名去 `a` / `asz` 前缀、
改用复数; 对照表见 `docs\HISTORY\2026-10-15-000005.md`)。该动作只改标识符名与相应注释, **不改语义、不改接口形态**, 属受控
漂移, 已在 `README.md` §1 / §6 与 `docs\DERIVE.md` §5 登记。除此之外, 快照层仍不得就地改写语义。

第三例例外(2026-10-16 第 7 轮, 用户指令): 快照件 `Render.h` **就地改造为渲染能力接口**(函数指针表
`RenderApi` + 窗口状态位), 快照件 `Render.cpp` **删除**, 并新增两套首产件 `ImGuiRender.{h,cpp}`(ImGui 侧
能力提供者, 渲染侧唯一 include imgui 的边界)与 `LayoutRender.{h,cpp}`(对外渲染出口, 不依赖任何 UI 库)。
这是本轮唯一改动快照文件语义的动作, 依据 = `docs\CHARTER.md` 纲领四; 公开面变更 = 原 `Render` 命名空间的
7 个函数(含渲染入口与探针读取)迁入 `LayoutRender` 命名空间(函数名与签名不变), 原 `Render.h` 不再对外提供
函数。登记处: `README.md` §1 / §6、`docs\DERIVE.md` §5、`docs\HISTORY\2026-10-16-012744.md`。除此之外,
快照层仍不得就地改写语义。

第四例例外(2026-10-16 第 8 轮, 用户指令): **渲染出口去窗口身份 + ImGuiHost 接管渲染链** ——
`Render.h` 增第 17 项能力 `AcquireWnd`(输入期望客户区尺寸, 不符则由实现方主动调窗);
`LayoutRender` 删 `CheckWindowResize`(`RenderBase` 在开窗前取渲染目标, 内嵌(flex)路径沿用当前窗口);
首产件 `ImGuiHost` 收回主消息循环、帧流程、每窗口 D3D 设施(`render\imgui\D3D11Manager`; 2026-10-22 前在
`common\`)、呈现、遮挡判定、IME 与窗口圆角, 每帧固定调用一次外部处理流程, 退出判据外露;
快照件 `Controls.cpp` 的标题栏事件派发点把 `param`(通用槽)改为携带当前渲染窗口句柄; 帧抓取
(`DEMO_FRAME_DUMP` 与后台缓冲回读)整体取消。
依据 = `docs\CHARTER.md` 纲领五; 登记处: `README.md` §1 / §5 / §6、`docs\DERIVE.md` §5、
`docs\HISTORY\2026-10-16-*.md`(本轮)。

第五例例外(2026-10-16 第 10 轮提出 / 第 11 轮按用户口径修正, 用户指令): **窗口位置记录与几何结算** ——
窗口件(WND 控件)的 `position` 提为窗口位置记录(解析期三支规则给初值: `(-1,-1)` 随机 / 在屏内直用 /
越屏降级随机); `RenderApi::AcquireWnd` 签名改为 `BOOL AcquireWnd(CONST vec2& pos, CONST vec2& size)`
(期望位置与尺寸都走参数, 不用侧通道), `ImGuiHost::AcquireNextWnd(pos, size)` 在该窗口绘制之前结算几何;
快照件 `Controls.{h,cpp}` 的标题栏派发点**再次**就地改动 —— `param` 通用槽由"当前渲染窗口句柄"改为
"窗口期望坐标"(打包口径见 `ControlSpec.h` 的 `PackWndPos` / `UnpackWndPos`), 拖动基线改读窗口位置记录;
快照件 `Render.h` / `layout\LayoutRender.cpp` / `GeometrySolver.{h,cpp}` / `BasicDesign.{h,cpp}` 亦按本轮口径调整;
事件处理器 `DemoEvents` 把坐标记进 WND 件的 `position`(未绑定事件的窗口因此不可拖动)。
依据 = `docs\CHARTER.md` 纲领六(含口径修正); 登记处: `README.md` §1 / §5 / §6、`docs\DERIVE.md` §5、
`docs\HISTORY\2026-10-16-101915.md`。回滚基线 = `Demo\out\Release\demo_old.exe`。

第六例例外(2026-10-21 第 33 轮, 用户指令): **Render 按技术名分目录 + 接口注册制** ——
① 渲染契约与具体 UI 技术分家: 与 UI 库无关的契约留在 `src\framework\render\Render.h`, 具体技术的宿主与实现按技术名
收进 `src\framework\render\imgui\`, 三级目录 = `imgui\ImGuiHost` · `imgui\ability\ImGuiRender` · `imgui\control\{Controls, Widgets}`
(宿主与 `ability` / `control` 平级 ⇒ 加第二种渲染技术只增同级目录);
② **两张能力表 + 注册制**: 控件库以 `Controls::GetCtrlApi()` 交出 `CtrlApi`(3 项: UpdateContext / DrawCtrl / HasActiveInput),
宿主以 `ImGuiHost::Register()`(返回 `CONST HostApi*`, 幂等)交出 `HostApi`(13 项); 宿主在 `Acquire` 期把两张表注入
`LayoutRender`(`SetRenderApi` / `SetCtrlApi`), 消费方只经表调用 —— `Dispatch` 只经 `HostApi` 访问宿主, 不引用
`ImGuiHost.h` / `imgui.h`, 也不引用 `Identity.h`;
③ **样式由调度层计算**: `StyleDesc`(4 圆角 + 17 配色槽, GDI 色值)由 `Dispatch` 按参考尺寸与投影配色填好,
宿主 `ApplyStyle` 负责映射到具体 UI 库的样式槽 ⇒ 表内与调度层均不含 UI 库类型;
④ 控件缓冲装配下沉 `base\CtrlBuffer`, 控件类型判定(`CtrlTypeIs*` 四件)归 `base\ControlSpec.h`,
`BuildCtrlInfoFromSpec` / `FreeLayoutCtrlBuffers` 归 `layout\LayoutBuilder` —— 控件库只剩"绘制"与"取表"。
这是本轮唯一的**快照语义改写**(快照件 `Render.h` / `Controls.*` / `ImGuiHost.*` 等只改结构不改行为);
依据 = `docs\CHARTER.md` 纲领十; 登记处: `README.md` §1 / §2 / §5 / §6(结构治理)、`docs\DERIVE.md` §1 / §5、
`docs\HISTORY\2026-10-21-*.md`(S1–S5 五份)。除此之外, 快照层仍不得就地改写语义。

第七例例外(2026-10-22, 用户指令): **快照件 `D3D11Manager` 路径层归位** —— 三件(`.h` / `.cpp` / `.md`)自
`src\common\` 移至 `src\framework\render\imgui\`, 与 `ImGuiHost` 平铺; 类的定义、成员布局、函数实现与链接库
零改动, 属**纯路径搬迁**(不属语义改写)。依据 = `docs\EVAL-DX11-RELOCATION.md` 的评估结论与用户批复
(D1=A / D2=Demo 先行 / D3=只加注记 / D4=保留 / D5=S1+S2); 落地 = `docs\CHARTER.md` 纲领十一。
登记处: `README.md` §1 / §2 / §2.1 / §5 / §6、`docs\DERIVE.md` §5、`docs\HISTORY\2026-10-22-*.md`。
从 GenUI 重新复制 `common` 时的三选一处置见 `README.md` §6"漂移处置(2026-10-22 D3D11Manager 归位)"。

## 3. 范例清单



状态词: 规划 / 进行中 / 已落地。当前 EX-00 与 EX-10..EX-17 已落地(EX-00 = 本工程首轮形态;
EX-10 = 2026-10-15 第 4 轮新增, 编号待用户确认; EX-14 = 2026-10-18 第 28 轮新增; EX-15 = 2026-10-18 第 29 轮新增;
EX-16 = 2026-10-18 第 30 轮新增; EX-17 = 2026-10-21 第 32 轮新增)。

| 编号 | 名称 | 覆盖的框架能力面 | 形态 | 状态 |
| :--- | :--- | :--- | :--- | :--- |
| EX-00 | 单窗口最小演示 | 结构件三件套 + 10 类内容控件 + 2 条事件(拖拽窗口 / 事件委托演示)+ 声明式渲染链 | 已落地(首轮) | 已落地 |
| EX-01 | 最小可运行窗口 | 从零到一屏的**最小装配集**(分配器注入 3 处 + exe 目录缓存 + 解析 → 中间态 → 展开 → 几何解算) | 布局文档 + 装配说明 | 规划 |
| EX-02 | 控件全集 | 类型表 22 类(3 结构件 + 19 内容控件)的声明与默认外观 —— 本工程现展示 13 类, 未展示 9 类(`RECT` / `INPUT_ACTION` / `LIST` / `LIST_INPUT` / `LIST_COMBO` / `LIST_TIMESPAN` / `TIMEBAR` / `RENDER` / `COMBO_ACTION`); 其中数类的实质内容须配处理器(见 §3.1) | 布局文档 + 处理器 | 规划 |
| EX-03 | 数据流与缓冲 | 类型 → `ctrl.buffer` 结构(`CtrlBuffer.h` 的 `Ctrl*Data`)→ 读写回读 | 布局文档 + 处理器 | 规划 |
| EX-04 | 事件与操作码 | 操作码族(公共段 / 结构件段 / 编辑族段)逐族的示范处理器 | 布局文档 + 处理器 | 规划 |
| EX-05 | 列表族与行数据 | `LIST` / `LIST_INPUT` / `LIST_COMBO` / `LIST_TIMESPAN` / `TIMEBAR` 的行数据块约定与拖拽搬移 | 布局文档 + 处理器 | 规划 |
| EX-06 | 几何与自适应 | `autosize` 各模式与 8 元系数语义 | 布局文档 + 对照日志 | 规划 |
| EX-07 | 布局源双格式 | 同一份布局的 JSON 与 bin 双形态(读方向内容嗅探) | 两份同源文档 | 规划 |
| EX-08 | 多布局文档 | 一份文档多布局与布局选择 | 布局文档 + 选择机制 | 规划 |
| EX-09 | 装修与配色 | `ProjCR` 投影配色、字体与 ImGui 样式槽 | 装配说明 | 规划 |
| EX-10 | ImGui 宿主与标准窗口 | 推荐入口 `Start(数量, 应用名, 段前回调, 用户指针)`(DPI 感知 → 声明数量 → 建窗 → 建立渲染设施; 失败内部回退) · 默认几何(客户区 800x600 / 工作区居中 / 圆角 10) + 已登记字号(`SetFontSize`; 未登记时 18) · 共用窗口类 `<APPNAME>_IMGUI_CLASS` · 内置窗口过程(`WM_DESTROY` / `ESC` 请求退出, 其余 `DefWindowProcW`) · 每标准窗口一套上下文与两端后端 · 中文字体 · 亮暗基础样式与配色注入 · 段前回调 · 激活数不足时自动隐藏多余窗口 | 框架层模块(`framework\render\imgui\ImGuiHost`, 接口文档同目录)+ 宿主装配 | 已落地(2026-10-15 第 4 轮; 2026-10-21 第 32 轮增 `Start` 与内置窗口过程) |
| EX-11 | 渲染侧解耦(能力表 + 注入) | 渲染能力接口(函数指针表 16 项 + 窗口状态位) · ImGui 侧能力提供者 · 对外渲染出口零 UI 库依赖 · 宿主初始化期注入 ⇒ 换 UI 实现只改注入方 | 框架层模块(`framework\render\Render.h` 接口 + `render\imgui\ability\ImGuiRender` + `layout\LayoutRender`)+ 宿主装配 | 已落地(2026-10-16 第 7 轮) |
| EX-12 | 渲染链归属(渲染出口去窗口身份 + ImGuiHost 自持设施) | 渲染出口零窗口身份(hWnd 与序号都不进) · 第 17 项能力 `AcquireWnd(期望位置, 期望客户区尺寸, 期望标题)` · ImGuiHost 持主消息循环 / 窗口过程 / 每窗口 D3D 设施 / 呈现 / 遮挡 / IME / 窗口圆角 · 每帧一次外部处理流程(由 `Application::Frame` 提供, 用户可自定义 `FrameFunc`) + 退出判据外露 · 帧抓取取消 | 框架层模块(`framework\render\Render.h` 能力表 + `layout\LayoutRender` + `framework\render\imgui\ImGuiHost` + `framework\render\imgui\ability\ImGuiRender`)+ 调度层帧流程 | 已落地(2026-10-16 第 8 轮; 2026-10-21 第 32 轮窗口过程内置、标题进能力表) |
| EX-13 | 窗口位置记录与几何结算 | 位置记录 = WND 件的 `position`(解析期三支规则: `(-1,-1)` 随机 / 在屏内直用 / 越屏降级随机) · 记录改写者 = 标题栏绑定的事件(`param` 携带期望坐标; 未绑定即不可拖动) · 结算 = `AcquireWnd(pos, size)`(签名带参, 在该窗口绘制之前) · `OpenWnd` 摆放位置 = 布局原点 | 框架层模块(`Render.h` 能力表 + `ImGuiHost` + `LayoutRender` + `GeometrySolver` + `BasicDesign` 工作区查询 + `ControlSpec.h` 参数槽约定)+ 事件层记录 | 已落地(2026-10-16 第 10 轮提出 / 第 11 轮修正) |
| EX-14 | 父容器与事件委托 | 控件声明 `parent`(父容器 ID, 缺省 `-1` = 无父)· 事件字段的特殊值 `CTRL_EVENT_ENTRUST_PARENT` = "把自己的事件绑定至父容器的事件"· 装配期按 `parentID` 上溯: 父自己也委托则继续上溯, 父无事件且无处理函数则中断, 其余继承其处理函数(只继承处理函数, 不继承缓冲)· 父缺失 / 父无事件且无处理函数 / 祖先未登记事件 / 成环 ⇒ 该控件无响应(静默降级)· 消费方可用 `Layout::GetCtrlInfoByID` 按父寻址 | 框架层模块(`ControlSpec.h` 的 `CtrlInfo` / 事件名约定 + `LayoutModel` 的 `ElementSpec` 与 bin 记录 + `LayoutSolver` 的 `parent` 解析 + `LayoutBuilder::ResolveEntrustedEvents` + `Layout::GetCtrlInfoByID`)+ 布局文档声明 + 事件处理器 | 已落地(2026-10-18 第 28 轮; 第 29 轮显式化上溯判据) |
| EX-15 | 控件层次按父链推导 | `CtrlInfo::nLayer` 由 `Layout::DeriveCtrlLayers` 按 `parentID` 链求值: 父层 + 自身类型步进(`BUTTON` / `CHECKBOX` `+1` · `RECT` / `INPUT_*` / `LIST_*` / `COMBO*` / `RENDER` / `SLIDER` `-1` · 其余 `0`); 无父 / 父缺失 / 成环 ⇒ 以窗口底档 `0` 为基; 结构件恒 `0`; **不做几何包含推导**, 层序只认声明 | 框架层模块(`ControlSpec.h` 的层次口径与 `CtrlLayerStep` + `Layout::DeriveCtrlLayers` / `GetCtrlInfoByID`)+ 布局文档的 `parent` 声明 + 运行日志的 `[DEMO] 控件层次:` 自证 | 已落地(2026-10-18 第 29 轮; 2026-10-22 移除选中标记特例) |
| EX-16 | 控件坐标按父链累加 | 内容控件的**有效坐标(窗口本地) = 声明坐标 + 父链声明坐标之和**: 声明坐标 `CtrlInfo::posDeclared` 由几何解算写入, 有效坐标 `CtrlInfo::position` 由 `Layout::DeriveCtrlPositions` 按 `parentID` 链重算(只读声明值 ⇒ 幂等 · 与链序无关); 自指 / 成环 ⇒ 视同无父; `WND` 是桌面位置 ⇒ 不作父基也不参与累加, `TITLE` / `CNTR` 计为窗口本地坐标; 内容容器同时承担裁剪与坐标系原点 ⇒ 绘制侧(`Controls::DrawCtrl`)与命中判定把有效坐标换算成容器本地坐标, 只扣一次; 嵌套几层就累加几层(探针 t10) | 框架层模块(`ControlSpec.h` 的 `posDeclared` + `Layout::DeriveCtrlPositions` + `GeometrySolver` + `Controls::DrawCtrl` / `UpdateContext` + `LayoutRender`)+ 布局文档的 `parent` 声明 + 运行日志的 `[DEMO] 控件坐标:` 自证 | 已落地(2026-10-18 第 30 轮) |
| EX-17 | main 胶水 + Dispatch 静态单例 | `main.cpp` 只做七步(取宿主表 `Register` → 组 `DispatchConfig` → `Dispatch::Init` → `ImGuiHost::SetFontSize(Dispatch::GetFontSize())` 登记建面字号 → `pHost->Start` → `pHost->Run` → 释放链) · `Dispatch` = 全静态单例(私有删除构造析构 + 会话分配器 arena 内的 `DispatchData` 成员块) · 事件绑定表经函数指针注入(调度层不反向依赖事件层) · 场景装配收在 `DispatchScene.cpp`(LoadScene / UnloadScene) · 每帧流程 = `Dispatch::Frame`, 退出判据 = `Dispatch::GetExitAddress()` · 投影配色与样式注入 / 无人值守参数 / 收尾自证同层 | 调度层模块(`framework\dispatch\Dispatch.{h,cpp,md}` + `framework\dispatch\DispatchScene.cpp`)+ 胶水入口(`src\main.cpp`) | 已落地(2026-10-21 第 32 轮; 结构调整 S3 归位并改名; 同日补建面字号登记) |

**承载机制**(已实现): 范例以"布局文档 + 处理器"为单位, 由本工程 exe 按 `DEMO_DOC` 环境变量选择文档
(缺省 `Demo.json`); 单一 exe 承载全部范例, 不为一范例建一个工程。

每个范例的交付物固定四项: ①布局文档(声明式, 唯一真源); ②必要的事件处理器; ③与范例同目录的用法说明
(讲"为什么这样写才是典范", 并逐条对应第 2 节判据); ④客观自证(构建日志 + 启动日志, 需要时加帧 dump 哈希)。

### 3.1 布局文档的硬约束与声明通道边界(造范例前必读)

依据 = 框架源码实测(`framework\layout\LayoutSolver.cpp` / `layout\LayoutModel.h` / `Controls.cpp` / `Render.h`)。

**硬约束与实测证据**(2026-10-15 本工程实测; 探针 = 故意违规的最小骨架, 用 `DEMO_DOC` 指向运行):

| 级别 | 约束 | 违反后果 | 实测结果 |
| :--- | :--- | :--- | :--- |
| 加载期**拒载** | 每枚控件的 `x` / `y` / `width` / `height` 四个因子对象全必需; 每因子的 `coeff` 须**至少 8 项数字**、`offset` 须为整数 | **整份文档拒绝加载**(不是该控件降级), 退出码 1, 日志 3 行, 末行"场景载入失败: 文档解析失败" | `width.coeff` 砍到 7 项 ⇒ 诊断"coeff 只有 7 项(需 8): 缺项按 0 处理", 随即拒载 |
| 加载期**拒载** | `layouts` 数组长度 1..1024; 每布局 `ctrls` 上限 8192 | 同上 | 未单独实测 |
| 加载期**容错** | 每枚控件 `type` 必需且须为类型表精确字符串(区分大小写) | 未命中 ⇒ `CTRL_TYPE_UNKNOWN`(画占位矩形), **不拒载** | 未单独实测 |
| 加载期**容错** | `ElementSpec::text` 上限 **128 B** | 超长**截断**, 不拒载 | 未单独实测 |
| **渲染期无效**(加载期完全不校验) | 结构件(`WND` / `TITLE` / `CNTR`)须落在 `ctrls` 前 3 项内, `WND` 与 `CNTR` 必需, 内容控件须排在其后 | 加载**成功**、退出码 **0**、日志照常报"渲染就绪"; 失效只在渲染期由 `Layout::IsInvalid` 触发(窗口空白, 周期自证行显示 绘制命令=0 且帧率极高) | 缺 `CNTR`、以及"内容控件排结构件之前"两例**均载入成功**(日志控件数 2 / 3), 与正常态的退出码无从区分 |

**两点必须写进典范用法**:

1. 因子违规的诊断文本是"缺项按 0 处理" —— **措辞像降级, 后果是整份拒载**。消费方读到该行不可当作"仅该控件降级"。
2. 结构件违规**没有任何加载期信号** ⇒ 这是**静默失败**, 消费方只看到空白窗口。范例须自行自证布局有效
   (例如当场查询 `Layout::IsInvalid` 并在日志中显式汇报), 不得把"启动成功"当作"布局正确"的证据。

探针文档与运行日志留档在 `docs\probes\`(`t1-badcoeff` / `t2-nocntr` / `t3-shellorder` / `t4-wndpos-random` /
`t5-wndpos-offscreen` / `t6-entrust-noparent` / `t7-entrust-cycle` / `t8-entrust-chain` / `t9-parent-layer` /
`t10-nested-parent` 各一份 `.json` 与 `.log`), 可用 `DEMO_DOC` 指向其副本重跑复现。其中 `t6` 实证"委托控件的
父容器不存在 ⇒ 该控件无响应"(绑定 0 枚 / 委托 1 枚已解析 0 枚)、`t7` 实证"两枚委托控件互为父容器(成环)⇒
上溯止步、均无响应"且**坐标累加同样按'视同无父'收口**(`4` / `5` 的有效坐标 = 声明坐标, 偏移 0 枚)、
`t8` 实证"多级委托沿父链上溯到最近一个持有处理器的祖先"(2 枚委托全部解析)、`t9` 实证"层序只认 `parent`
声明": 内容控件几何上落在容器内、但 `parent` 指向另一枚域控件(几何在别处)⇒ 层值随父链(`4=-1`),
与几何包含口径(该控件会取 `4=0`)可区分、`t10` 实证"坐标按父链累加": 三级嵌套(容器 → 域 → 文本 → 按钮)
的声明坐标 `(4,4)` / `(20,30)` / `(10,20)` 依次累加成 `3=(100,254)` / `4=(120,284)` / `5=(130,304)`
(每级都含容器原点 `(0,54)`), 与"只按自身声明绘制"的旧口径可区分。

`t2` 的留档日志是**摘录**: 缺 `CNTR` 使布局无效、渲染跳过, 周期自证行在 3000 ms 内放大到约 9800 行,
文件中以一行省略标记代替, 头部与收尾行保持原样。

**声明通道边界**(布局文档能声明什么、不能声明什么):

布局文档**能**声明的只有这些: 布局段的 `name` / `label` / `IsMaxiable` / `IsResizeable` / `wndX` / `wndY` /
`autosize`(六项参考量 + 四种 mode)/ `width` / `height`; 控件段的 `type` / `id` / `parent` / `label` / `text` /
`event` / `x` / `y` / `width` / `height`。解析器只读这些键, 没有别的。

**`parent` 的数据一致性口径**(2026-10-18 第 29 轮): 每枚控件(含 `WND` / `TITLE` / `CNTR` 三枚结构件)
都写 `parent`, 无父写 `-1`。框架对结构件的父值不作处理(结构件层次恒 `0`、坐标也不参与累加), 但声明面要求
字段齐备 —— 缺省不写时解析器按 `-1` 兜底, 因此"缺"不会报错, 只会让数据不一致(`parent` 同时是层序、
坐标累加与事件委托的父链)。

**`parent` 同时决定坐标基准**(2026-10-18 第 30 轮): 内容控件的 `x` / `y` 是**父控件坐标系内的声明坐标**,
有效坐标 = 声明坐标 + 父链声明坐标之和; 内容容器(`CNTR`)的原点因此必然计入(容器件本身是窗口本地坐标),
而 `WND`(桌面位置)不入累加。写布局文档时的实用口径: 以容器为父的控件, `x` / `y` 就是容器内坐标
(与改前一致); 以**另一枚内容控件**为父时, `x` / `y` 相对该控件的矩形原点 —— 这正是把"输入框 + 按钮"
一类复合件拆成多枚控件拼装的坐标基础。

因此下列**实质内容无法在布局文档里声明**, 必须由事件处理器或宿主在运行期填充:

| 控件类型 | 无法声明的实质内容 | 装配期初值 |
| :--- | :--- | :--- |
| `LIST_INPUT` / `LIST_COMBO` | 列表行数据(行内容由该列表绑定的处理器按帧填充) | 行数 0, 容量 64 |
| `COMBO` / `COMBO_ACTION` | 多选项表 | 恒 1 项(`text` 或 "选项 1") |
| `SLIDER` | 值域 | 恒 0..100(`text` 为手柄初值) |
| `PROG` | 进度值 | 恒 0 / 100 |
| `CHECKBOX` | 初始勾选态 | 恒 FALSE(`text` 为显示文字) |
| `INPUT_ACTION` | 按钮文案 | 恒 "浏览" |
| `RENDER` | 纹理与尺寸 | 全零(须宿主注入) |
| `TIMEBAR` | 起止时码 | 无缓冲装配点(恒 0..0) |

**点击类事件的路径差别**: 本工程走 `LayoutRender::RenderBase`(该渲染出口不派发点击) ⇒ `CLICKED` / `DOUBLECLICK` /
`RIGHTCLICK` 只在 `BUTTON` 与"已绑定事件的 `TEXT` / `RECT`"上由控件库自身派发; 其余类型只报各自的
"提交 / 变更 / 电平"码。

**结论**: 要展示上述控件的实质内容, 范例就必须自带处理器 ⇒ 范例形态是"布局文档 + 处理器", 不是单份 JSON。

## 4. 反向治理清单(种子, 待随范例扩)

下列各条是截至目前已确证的"消费方视角缺口"。**本清单只登记事实与实证, 不在本工程内自行修改框架或
GenUI**; 处置路径见第 5 节。

分级: A = 代码级夹带(编辑器/应用语义落在 framework / common, 违反 P1) · B = 消费方零调用的框架内建
(纯消费方为其付出编译与热路径代价) · C = 叙述与文档面(违反 P7) · D = 结构面(限制消费方形态) ·
E = 待用户口径。行号对 framework / common 而言即 GenUI 侧同一文件的行号(两层为逐字节快照)。

### A 级: 代码级夹带

| 编号 | 现象 | 实证 |
| :--- | :--- | :--- |
| RG-A1 | 编辑器选中标记的魔数 label 与其判据落在框架: `SEL_MARK_LABEL = "##SEL_MARK"` + `CtrlTypeIsSelectionMark`; `Layout.cpp` 另复制同一字面量以规避反向依赖(**已消解(Demo 侧, 2026-10-22)**: CtrlData 的标签与判据、Layout 的副本与两处跳过分支均已删除; 本工程无叠加件生产者, 层次推导不再有该特例。GenUI 侧仍待按"判据归 editor + 可注册谓词"处置) | framework\CtrlData.h(原):257-283 · framework\layout\Layout.cpp(原):279-282 · 唯一生产者 editor\SelectionOverlay.cpp:67 |
| RG-A2 | 框架头文件声明了一个自己无法提供的符号: `FormatToUI` 声明在 `LayoutModel.h`, 定义在 `app\Application.cpp`(**已消解(S4)**: 声明与文档条目均已删除) | framework\layout\LayoutModel.h:375-383 · app\Application.cpp:896-904 · 文档自认 LayoutModel.md:214 |
| RG-A3 | 框架回调契约直指编辑器命令: `pfnAddDocLayout` 的语义 / 注入者 / 降级口径全由编辑命令层定义 | framework\FuncPtr.h:81-91 |
| RG-A4 | 框架承载会话面回调四件套: 预览基座 / 预览同步 / 文档修订号 / 条目几何重算 | framework\FuncPtr.h:55-79 |
| RG-A5 | 框架内的编辑器专用渲染出口: `RenderPreviewBase` 把"编辑器点击记录器 + 选中态叠加件"合成进渲染入口 | framework\Render.h:44-72 · Render.cpp:272-292 · FuncPtr.h:45-53 · framework 外使用者仅 app\AppHost.cpp:503 |
| RG-A6 | 框架承载编辑器模态提示契约: `HintSpec` / `pfnHintSync` 在 framework 内零使用 | framework\FuncPtr.h:22-43 · 使用者全在 editor / app / tests |
| RG-A7 | 框架释放路径被编辑器行为塑形: 下拉控件选项数组"可能已被编辑器改指静态表"; 结构体字段注释写死三个编辑器 ImGui ID 与两个编辑器函数名(**已消解(Demo 侧, 2026-10-22)**: 字段注释改为"数组由装配期分配、字符串不归本结构释放", 编辑器专名与三个 ImGui ID 已移除) | framework\Controls.h(原):245-248 · framework\CtrlData.h(原):100-103 |
| RG-A8 | common 层公开接口的取值真源落在应用层头文件: `LoadResource` 的 `dwResId` 指向 `src\app\resource.h` | common\ResourceLoader.h:36-37 |
| RG-A9 | common 层硬编码应用层日志标签 `[MAIN]`, 纯消费方的运行日志里出现上游应用层身份 | common\AppEnv.cpp:62/80 · 实测痕迹 = 本工程运行日志第 5 行 `[MAIN] 圆角实现: DWM`(其余 8 行均为 `[DEMO]`) |

### B 级: 消费方零调用的框架内建

| 编号 | 现象 | 实证 |
| :--- | :--- | :--- |
| RG-B1 | 验证探针接口落在框架公开面, 且计数在正常渲染路径无条件累加(每击一次字符串拷贝), 消费方永不读取 | framework\Render.h:89-110 · Render.cpp:36-42 / 72-73 / 145-150 · 使用者仅 tests\HostProbes.cpp:519/529 |
| RG-B2 | 整件 `FrameEvent`(帧间事件队列)只为编辑器"帧内挂载、帧末执行"的时机需求存在, 消费方零调用却必须编译 | framework\FrameEvent.h:7-14 / 53-59 · 调用方 = app\AppHost.cpp:529 · app\PreviewSession.cpp:290-292 · editor\GenEditor.cpp 12 处 |
| RG-B3 | 消费方零调用但随快照进入产物的公开面: `RenderPreviewBase` / `RenderFlex` / `CheckWindowResize` / `GetClickProbeStats` / `GetCompoundClickStats` / `CtrlTypeIsSelectionMark` / `SEL_MARK_LABEL`(后两者已删除(Demo 侧, 2026-10-22)) / `LayoutBuilder::CopyLayout` | 本工程对上述符号 grep, 命中只落在 `src\framework\*` 自身 |

### C 级: 叙述与文档面

| 编号 | 现象 | 实证 |
| :--- | :--- | :--- |
| RG-C1 | 框架 / 公共层注释以编辑器模块名自我描述("与 GenUIEditor / SelectionOverlay 同风格", 4 处) | framework\GeometrySolver.h:13 · common\AppEnv.h:10 · common\AllocatorHub.h:11 · common\ResourceLoader.h:10 |
| RG-C2 | 框架注释残留应用 / 编辑器名义: "由 GenEditor 每帧维护" / `app\AppHost.cpp` 注入 / `Application::Init` 注入时机 / `GenEditor` 标签契约 / 双窗口架构 / 外壳快照 / 编辑器语境兜底 / 编辑器"打开/另存为"流程 | layout\Layout.cpp:280 · FuncPtr.h:85 · UIEventHandler.h:12 · layout\Layout.h:93 · FrameEvent.h:54 · layout\LayoutBuilder.h:20-21 · GeometrySolver.h:29 · layout\LayoutSolver.h:79-88 |
| RG-C3 | 接口文档的"使用分工"表把消费方一律写成 GenUI 应用层(`Application::` / `GenEditor::`), 对其它消费者不适用 | framework\layout\Layout.md:42-45 / 191-213 · layout\LayoutModel.md:294-333 · common\MemoryManager.md:242-243 |
| RG-C4 | 接口文档覆盖不足: framework 14 个模块仅 4 份 `.md`(缺 BasicDesign / Controls / CtrlData / FrameEvent / FuncPtr / GeometrySolver / LayoutSolver / Render / TextWidth / UIEventHandler); common 6 个模块仅 2 份(缺 AllocatorHub / D3D11Manager / FileCodec / ResourceLoader) | 各目录 `.md` 清单; 对照 GenUI LOCK.md:4"有接口文档 = 契约收敛可上锁" |
| RG-C5 | 文档路径腐化: 2026-10-14 目录治理后新旧两套路径并存(旧扁平 `src\X.h` 残留多处); 指向 Layout 前缀件的四处已随 2026-10-16 第 12 轮目录治理修正; `CtrlData.h` 的四处残留已随 2026-10-22 并入 `CtrlBuffer` 而消失 | 未修正残留: ControlSpec.md:3/7/8/224/225 · common\MemoryManager.md:3 · 已修正: `framework\layout\Layout.md:3` · `layout\LayoutModel.md:3` · `layout\LayoutSolver.h:9-10` · `common\FileCodec.h:9` |
| RG-C6 | 框架层注释把自身绑定到 GenUI 项目级文档(随层迁移即断链) | Render.cpp:63 · ControlSpec.md:6/223 · layout\Layout.md:15(`CtrlData.h:228` 一处已随 2026-10-22 并档消失) |
| RG-C7 | 框架未提供"文档 → 可用布局"的一步装配原语: 窗口像素与内部参考尺寸的算式必须由消费方复写 | 本工程 src\framework\dispatch\DispatchScene.cpp 的 `LoadScene`(窗口像素与参考尺寸段)对照 framework\layout\Layout.md 使用分工表 |
| RG-C8 | 消费方"必做装配步骤"无框架侧文档(三处分配器注入顺序、exe 目录缓存晚于分配器, 顺序不可颠倒) | 本工程 src\framework\dispatch\DispatchScene.cpp 的 `LoadScene`(分配器注入与文档路径段) |
| RG-C9 | **声明通道缺口**(与框架原则②"声明式布局, 布局文件是唯一真源"存在张力): 19 类内容控件中 8 类的**实质内容无法在布局文档里声明**, 只能由处理器或宿主在运行期填充; 且 `ElementSpec::text` 上限 128 B ⇒ `INPUT_MULTI` 的 4096 B 缓冲在纯 JSON 路径下无法填满 | framework\Controls.cpp:1619-1835(建缓冲矩阵与初值) · layout\LayoutModel.h:28 · Render.h:252-253 · 明细见本文件 §3.1 |
| RG-C10 | **诊断措辞与后果不符**: 因子违规时诊断写"缺项按 0 处理"(像降级), 实际是 `SolverFactor` 返回 FALSE ⇒ **整份文档拒载** | framework\layout\LayoutSolver.cpp:796/823-825/838-839/848-849(数值补 0 但 `bRet = FALSE`) · 调用方 :244-245 / :307-310 以 `goto END` 拒载 · 实测见 §3.1 |
| RG-C11 | **结构件违规静默失败**: 缺 `CNTR` 或内容控件排在结构件之前, **加载期零校验** —— 退出码 0、日志 7 行且报"渲染就绪", 失效只在渲染期由 `Layout::IsInvalid` 触发 | framework\layout\Layout.cpp:721-729(只扫前 3 槽认定结构件) / :753-770(IsInvalid) · Render.cpp:180-183(渲染期才退出) · 实测见 §3.1 |

### D 级: 结构面

| 编号 | 现象 | 实证 |
| :--- | :--- | :--- |
| RG-D1 | 框架 / 公共层以进程级文件静态承载分配器与帧上下文, 不存在"两份布局 / 两套控件上下文并存"的支持面(消费方被限制为单文档) | layout\Layout.h:50-53 · layout\Layout.cpp:146 · layout\LayoutModel.cpp:16-17 · BasicDesign.cpp:392-393 · Controls.cpp:97-99 · Widgets.cpp:39-41(部件层上下文; 第 14 轮分离后为两位上下文) · UIEventHandler.cpp:21-22 · AllocatorHub.cpp:17-18 · ResourceLoader.cpp:15 |
| RG-D2 | `ResourceLoader` 有 `MAX_PATH` 硬上限与无边界拼接(`lstrcpyA` / `lstrcatA`), 超长路径静默失败; 且依赖 `AllocatorHub::Init` 先行而自身不校验 | common\ResourceLoader.cpp:22-41 / 39 / 49-60 |
| RG-D3 | `AppEnv` 直读系统注册表与 DWM 属性(宿主外观策略落在 common 层), 每次调用都开键读值无缓存 | common\AppEnv.cpp:33-42 / 57-58 |

### E 级: 待用户口径

| 编号 | 现象 | 实证 |
| :--- | :--- | :--- |
| RG-E1 | 中文 UI 默认文案写死在框架控件缓冲初始化里(按钮默认"浏览"、组合动作默认"动作")(**已收束(Demo 侧, 2026-10-22)**: 三枚缺省文案集中为 `CTRL_DEFAULT_ACTION_BTN` / `CTRL_DEFAULT_BROWSE_BTN` / `CTRL_DEFAULT_COMBO_ITEM`, 四处字面量改为引用常量; "缺省值是否可由应用完全覆盖"仍待 GenUI 侧口径) | framework\base\CtrlBuffer.h 的"控件缺省文案"区 · 引用点 Controls.cpp / Widgets.cpp / CtrlBuffer.cpp |
| RG-E2 | 框架视觉 / 交互调参源自外部参考项目 VideoCut(时间轴滑块形态) | Widgets.cpp:940/977(单手柄滑块与几何比例常量) · Widgets.cpp:58 · Controls.cpp:1110 |
| RG-E3 | 进程级可写静态串作共享返回文案(`ERRBUFFER`) | Controls.cpp:79(被 423/458/986/1018/1057 复用) |
| RG-E4 | 消费方是否应裁剪快照中零调用的框架模块(如 `FrameEvent`) | 见 RG-B2 / RG-B3 |

**锁定面提示**: RG-A1 涉及 `Layout`(锁定模块, 冻结面含 `IsSelectionMarkCtrl` 与"选中标记不参与层次推导"),
RG-A2 涉及 `LayoutModel`(锁定模块) —— 处置前必须按 GenUI `LOCK.md` §1-4 先申请用户解锁, 不得先改后报。

### 本轮登记(2026-10-15 第 4 轮: ImGui 初始化独立到框架层)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 无新增条目 —— 本轮是"按用户指令为框架补能力", 不是从范例实践中发现缺口 |
| 已消解面 | RG-C7 / RG-C8 所述"消费方必做装配步骤无框架侧落点与文档"在 ImGui 面得到缓解: 原先内联在宿主 `DemoHost.cpp` 的建窗 / 上下文 / 中文字体 / 两端后端 / 帧流程 / 后端消息转发 / 成对释放迁入 `framework\ImGuiHost`, 并配齐接口文档(边界 / 接口表 / 契约 / 失败语义 / 消费方最小用法) |
| 回灌建议(待 GenUI 处置) | `GenUI\src\app\AppHost.cpp` 的编辑 / 预览双窗口 ImGui 初始化与帧流程(两套上下文与后端 + 每帧 `SetCurrentContext` 切换 + 后端 `WndProc` 转发)与本模块"每标准窗口一单元"的形态同构, 可整体迁入; 按 GenUI `LOCK.md` / `PLAN.md` 流程处置, 本工程不代改 |
| 快照纪律 | 本轮属 §2 例外条的首例(首产框架层新增件, 未回改任何既有快照文件); 三件首产件已在 `README.md` §1 / §6 与 `docs\DERIVE.md` §5 登记 |

### 本轮登记(2026-10-16 第 7 轮: 渲染侧与 ImGui 解耦)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 无新增条目 —— 本轮是"按用户指令为框架做结构变更"(纲领四), 不是从范例实践中发现缺口 |
| 快照纪律 | 本轮属 §2 例外条的**第三例**: 快照件 `Render.h` 就地改造为接口(渲染能力表), 快照件 `Render.cpp` 删除, 并新增两套首产件; 依据 = `docs\CHARTER.md` 纲领四, 登记处 = `README.md` §1 / §6 与 `docs\DERIVE.md` §5 |
| 涉及既有条目的行号 | 本节 A / B / C / D 级清单中凡以 `framework\Render.h` / `Render.cpp` 行号作实证的条目(`RG-A5` / `RG-B1` / `RG-C6` / `RG-C11`), 其行号**指向 GenUI 侧快照文件** —— GenUI 本轮未动, 故这些实证仍有效; 但本工程侧已无 `Render.cpp`, 若要在本工程复现这些实证, 须回 GenUI 侧或按本轮 HISTORY 的"实证点对照表"换算 |
| 公开面变更 | 原 `namespace Render` 的 7 个函数迁入 `namespace LayoutRender`(函数名与签名不变): `RenderBase` / `RenderPreviewBase` / `RenderFlex` / `CheckWindowResize` / `IsIMEDisabled` / `GetClickProbeStats` / `GetCompoundClickStats`; 新增注入面 `LayoutRender::SetRenderApi` / `GetRenderApi` 与接口件 `Render.h` 的 `RenderApi` 表 |
| 反向治理影响 | `RG-B1` 所述"验证探针接口落在框架公开面"在本工程**未消解**: 补项统计已随实现迁入 `ImGuiRender`, 但读取入口 `GetClickProbeStats` 仍在公开面(行为与口径不变); 是否裁撤留给后续轮次 |
| 本工程内部解耦面(新增观察) | 渲染出口的"窗口/容器/ID/命中判定"原语与布局语义原本混在 `Render.cpp` 一处(62 处 UI 库调用); 解耦后 `LayoutRender` 零 UI 库依赖, 未来换 UI 实现只需另写一份能力表 —— 本条可作"框架应提供的边界"实证候选, 待用户口径决定是否登记为反向治理条目 |
| 回灌建议(待 GenUI 处置) | `GenUI\src\framework\Render.{h,cpp}` 的解耦形态(接口表 + 能力提供者 + 对外出口 + 宿主注入)可按 GenUI `LOCK.md` / `PLAN.md` 流程整体迁入; 本工程不代改 GenUI |

### 本轮登记(2026-10-16 第 8 轮: 渲染出口去窗口身份 + ImGuiHost 接管渲染链)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 无新增条目 —— 本轮是按用户指令做结构变更(纲领五), 不是从范例实践中发现缺口 |
| 快照纪律 | 本轮属 §2 例外条的**第四例**(见该条); 快照件 `Controls.cpp` 另有一处就地改动(标题栏派发点 `param` 携带当前窗口句柄), 已在 `README.md` §6、`docs\DERIVE.md` §5 与本轮 HISTORY 登记 |
| 公开面变更 | `RenderApi` 16 → 17 项(新增 `AcquireWnd`); `LayoutRender` 删 `CheckWindowResize`(9 → 8 项); `ImGuiHost` 增 `FrameFunc` / `GetWndCount` / `Acquire` / `Run` / `AcquireNextWnd` / `GetCurrentWnd` / `SetIMEDisabled` / `GetRenderStats` / `GetResizeStats`, `WndSpec` 增 `dwRounding`, 移除 `InitUnit` / `NewFrame` / `RenderToTarget` 公开面 |
| 自证口径变更 | 帧抓取取消 ⇒ 本工程不再产出 BMP, 像素门禁失效; 门禁改为两配置零警零错 + 无人值守退出码 0 + 日志自证(场景 / 控件 / 事件 + 绘制量 + 窗口几何调整) |
| 反向治理影响 | `RG-B1`(探针面)与 `RG-A5`(编辑器专用出口)本轮仍未消解; 本轮新增观察 = "窗口身份不进渲染出口与能力表"可作框架边界的实证候选(待用户口径决定是否登记为条目) |
| 回灌建议(待 GenUI 处置) | `GenUI\src\framework\Render.{h,cpp}` 与 `app\AppHost` 的对应改造(能力表第 17 项 + ImGuiHost 形态 + 宿主剥离设施)可按 GenUI `LOCK.md` / `PLAN.md` 流程迁入; 本工程不代改 GenUI |

### 本轮登记(2026-10-16 第 10 轮提出 / 第 11 轮修正: 窗口位置记录与几何结算)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 无新增条目 —— 本轮是按用户指令做的结构变更(纲领六), 不是从范例实践中发现缺口 |
| 快照纪律 | 本轮属 §2 例外条的**第五例**(见该条); 快照件 `Controls.{h,cpp}` 的标题栏派发点第二次就地改动(`param` 由窗口句柄改为窗口期望坐标, 拖动基线改读位置记录), 另有 `Render.h` / `layout\LayoutRender.cpp` / `GeometrySolver.{h,cpp}` / `BasicDesign.{h,cpp}` 按本轮口径调整, 已在 `README.md` §1 / §6、`docs\DERIVE.md` §5 与本轮 HISTORY 登记 |
| 公开面变更 | `RenderApi::AcquireWnd` 签名 17 项不变但带参(`pos` + `size`); `ImGuiHost::AcquireNextWnd` 同步带参, `GetWndPos` 降为基线回退入口(无 `PostWndPos`); `ControlSpec.h` 增 `PackWndPos` / `UnpackWndPos`; `Controls::UpdateContext` 增"窗口位置记录槽"参数; `GeometrySolver` 增 `ResolveWndPos`; `ScreenRef` 增 `GetPrimaryWorkArea` / `IsRectOnAnyMonitor`; `DemoScene::GetWndRect` 改为读位置记录 |
| 自证口径变更 | 无(门禁不变); 本轮新增两支解析期探针(`docs\probes\t4-wndpos-random` / `t5-wndpos-offscreen`)验证位置声明的随机与越屏降级 |
| 反向治理影响 | 无新增条目; "窗口几何的记录与结算分离(记录在布局数据 · 结算在取得渲染目标处)"可作框架边界的实证候选 —— 手感已由用户判定"略差但可接受" ⇒ 口径采纳, 该候选待后续轮次决定是否登记 |
| 回灌建议(待 GenUI 处置) | `GenUI\src\framework` 的对应改造(位置记录槽 + `AcquireWnd(pos, size)` + 解析期三支规则)可按 GenUI `LOCK.md` / `PLAN.md` 流程迁入; 本工程不代改 GenUI |

### 本轮登记(2026-10-17 第 17 轮: 部件层注释治理与接口文档)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 无新增条目 —— 本轮是注释层治理 + 补接口文档, 不是从范例实践中发现缺口 |
| 反向治理影响 | `RG-C4`(接口文档覆盖不足)所列"framework 缺 `.md` 的模块"中 `Widgets` **本轮已消解**(新建 `Widgets.md`); 余者不变 |
| 注释治理面 | 治理范围由第 16 轮的"7 个有接口文档模块"扩为 **8 个**(`Widgets` 加入); 其余模块仍按"先补文档、再治注释"两步口径待办 |
| 快照纪律 | 未回改任何快照语义; 新建 `Widgets.md` 属"部件层拆分产物"第三件(GenUI 侧无对应件), 受 `README.md` §6 例外条保护 |

### GenUI 侧第一次反向治理(2026-10-18 · GenUI 第 38 轮)

| 事项 | 判定 |
| :--- | :--- |
| 台账落点 | GenUI 侧已立 `GenUI\docs\RG.md`(30 条逐条核账: 已消解 / 部分消解 / 未处置 / 待解锁)与 `GenUI\docs\PLAN.md` §2.14 的批次表(`RG-1`–`RG-9`); 本文件第 4 节仍是清单**源头与实证**, 条目现状以 GenUI 台账为准 |
| 本批消解 | `RG-A8` 已消解; `RG-C1` 已全消解(清单内 4 处 + GenUI 侧 `doc\DocConsistency.h` 同类 1 处); `RG-C2` / `RG-C5` / `RG-C6` 的**非锁定面**已消解 |
| 锁定面阻塞 | `RG-C2` 余 2 处(`layout\Layout.h` / `.cpp`) · `RG-C3` 三份接口文档 · `RG-C5` 余 4 份 · `RG-C6` 余 1 处 —— 均属 GenUI 锁定面, 须先解锁(GenUI 侧已按纪律登记, 未先改后报) |
| 未处置 | A 级 8 条 · B 级 3 条 · C 级余 `RG-C7`–`RG-C11` · D 级 3 条 · E 级 4 条; 另 GenUI 台账 §3 登记了 6 条**回灌候选件**(`RG-H1`–`RG-H6` = 本工程的 ImGuiHost · Render 接口化与几何结算 · 部件层独立 · 自绘收缩 · 目录治理) |
| 快照纪律 | GenUI 本轮未改任何接口与行为(22 文件仅注释与接口文档文本, 启动日志与基线逐字节相同) ⇒ 本工程快照**无需重取**, 两工程的 `framework` / `common` 分叉面**未扩大** |

### 本轮登记(2026-10-18 第 27 轮: 全量注释两行制 + 接口文档 29 份)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 无新增条目 —— 本轮是注释与文档层的规范性治理, 不是从范例实践中发现缺口 |
| 反向治理影响 | `RG-C4`(接口文档覆盖不足)所列缺失模块**本轮全部消解**: `src\` 下 29 个模块各一份 `.md`(新建 15 · 旧体例改写 6 · 复核 8)。`RG-C3`(旧文档"使用分工"表把消费方写成 GenUI 应用层)所述三份文档已按新体例改写, "使用方"表不再存在; `RG-C5` / `RG-C6` 的文档侧实证行号随整份改写失效, 需按新文档重定位(代码侧实证不受影响) |
| 注释治理面 | 由第 25 轮的两个样板模块扩为**全部 53 个源文件**: 695 处函数注释统一两行制, 补齐缺失描述 164 处, 移除函数块内的参数 / 返回 / 说明块 310 处(语义全部在文档承接); 结构骨架与实现性说明保留 |
| 快照纪律 | 未回改任何快照语义(仅注释与文档文本), 接口签名 / 行为 / 观感零变更(两配置 0 警 0 错 + 无人值守退出码 0); 但 `framework` / `common` 与 GenUI 快照的**文本差异面显著扩大**, 从 GenUI 重新复制时的处置见 `README.md` §6 的"漂移处置(2026-10-18 第 27 轮)" |

### 本轮登记(2026-10-18 第 30 轮: 控件坐标按父链累加 + InputAction/Combo 必要性评估)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 坐标口径原先靠"内容容器隐式偏移"实现 ⇒ 内容控件一旦以**另一枚控件**为父, 声明坐标就无处落基准(旧实现只按容器本地坐标绘制)。本轮把基准显式化(声明坐标 + 父链累加), 该缺口消解; 评估报告另行登记 `INPUT_ACTION` / `COMBO*` 三类的替代路径与未闭环项 |
| 反向治理影响 | `RG-A` / `RG-B` 类无新增条目。与 `RG-E`(待用户口径)相关的新增一项: 复合控件(`INPUT_ACTION` / `COMBO_ACTION`)在"事件委托 + 父链坐标"可用之后是否仍应保留为**单一控件**, 见 `docs\EVAL-INPUTACTION-COMBO.md` 的结论与候选方案 |
| 坐标口径 | 有效坐标(窗口本地) = 声明坐标 + 父链声明坐标之和; `WND` 不作父基(桌面位置), `TITLE` / `CNTR` 计入; 自指 / 成环 ⇒ 视同无父; 结构件不改写 |
| 绘制口径 | 容器已提供裁剪与偏移 ⇒ `Controls::DrawCtrl` 把有效坐标换算为容器本地坐标(只扣一次), 命中判定同一换算; Demo 收尾自证行与第 29 轮**逐字段一致** ⇒ 零像素变化 |
| 快照纪律 | 未回改既有快照语义, 但本轮改动面覆盖 `ControlSpec.h` / `Layout.{h,cpp}` / `LayoutBuilder.cpp` / `GeometrySolver.{h,cpp}` / `Controls.{h,cpp}` / `layout\LayoutRender.cpp` 等 GenUI 快照件 + 本工程首产的 `LayoutRender`, 处置见 `README.md` §6 的"漂移处置(2026-10-18 第 30 轮)" |

### 本轮登记(2026-10-21 第 32 轮: main 胶水 + Application 静态单例)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 无新增条目 —— 本轮按用户方向调整应用层形态(纲领九), 不是从范例实践中发现缺口 |
| 已消解面 | `RG-C7` / `RG-C8` 的"消费方必做装配步骤"进一步收口: 建窗 / 默认几何 / 窗口过程由 `ImGuiHost::Start` 承担, 场景装配的分配与释放收在 `ApplicationScene.cpp`(不再散在公开的场景类), 派生者只需替换文档与事件 |
| 反向治理影响 | `RG-B3`(消费方零调用的公开面)不变; 本轮新增观察 = "窗口过程与默认几何应归 UI 宿主"可作框架边界的实证候选, 待用户口径决定是否登记为条目 |
| 快照纪律 | 本轮改动 `ImGuiHost` / `Render.h` / `ImGuiRender` / `LayoutRender` 四组首产件(GenUI 尚未吸收); 应用层四件属本工程自持 ⇒ 处置见 `README.md` §6 的"漂移处置(2026-10-21 第 32 轮)" |

### 本轮登记(2026-10-21 第 33 轮: 结构调整 S1–S5 —— 分层归位 + 接口注册制 + 文档同步)

| 事项 | 判定 |
| :--- | :--- |
| 新增消费方缺口 | 无新增条目 —— 本轮按任务书做结构重组(纲领十), 不是从范例实践中发现缺口 |
| 已消解面 | `RG-A2`(`FormatToUI` 死声明)**已消解(S4)**: 声明与 `LayoutModel.md` 条目一并删除; `RG-C4`(接口文档覆盖不足)自第 27 轮起已全消解, 本轮新增 `base\CtrlBuffer.md` 后仍**每模块一份**(共 29 份) |
| 反向治理影响 | `RG-C7` / `RG-C8`(消费方必做装配步骤)再收一段: 调度层归 framework 后, 派生者不再复制应用层协调代码, 只改身份 + 事件表; `RG-B3`(消费方零调用的公开面)不变 |
| 结构口径 | 目录 = `framework\{base,render,layout,dispatch}` + `render\imgui\{ImGuiHost,ability,control}`; 层间依赖规则见 `README.md` §2.1; 例外见本文件 §2 的第六例例外 |
| 快照纪律 | 本轮只搬结构不改行为(日志的场景 / 控件层次 / 控件坐标行与 S0 基线逐字符一致); GenUI 侧仍是旧扁平路径 ⇒ 从 GenUI 复制 `framework` 的处置见 `README.md` §6 的"结构治理(2026-10-21)" |
| 口径变更 | 全工程 83 文件 = 源 54 + 接口文档 29; framework 58 = 源 38 + 文档 20; `Application` → `Dispatch`(类名与文件名), 场景切片 = `DispatchScene.cpp` |

## 5. 反向治理的机制与纪律

**路径**: 范例实践(本工程) → 反向治理清单(本文件第 4 节) → 回灌 GenUI。

**回灌纪律**(全部照 GenUI 侧现行制度, 不另立门户):

1. 每条清单项先判定**归属域**: 属"应用层与模块分离"⇒ GenUI `docs\PLAN.md` 的 `AP` 域; 属控件/渲染/
   序列化等 ⇒ 对应设计域; 属文档与工具 ⇒ `DT` 域。
2. 动手前**必查 `GenUI\docs\LOCK.md`**: 命中锁定面(MemoryManager / Layout / LayoutModel)⇒ 停手并按
   解锁制度申请, 不得先改后报。
3. 改动一律走 GenUI 标准门禁(两配置零错零警 → exe 目录内真实启动一次 → 数据指纹复核 → 证据归档)。
4. GenUI 侧改动完成后, 本工程的 `framework` / `common` 快照**重新复制并复核 SHA-256 一致性**, 再重跑
   `cfg → inject → restore → release`; 快照漂移必须显式登记, 不得静默。
5. 反向治理的**产出留痕在 GenUI 侧**(`PLAN.md` 状态、`HISTORY.md` 沿革、`docs\archive\recon-*` 证据);
   本文件只保留"当前清单"与指针, 不复制历史。

## 6. 待用户拍板项

1. 范例清单(第 3 节)的**编号、名称与粒度**是否照此; 是否需要先只做 EX-01 / EX-02 两个。
2. "典范判据"七条(第 2 节)是否完备; 是否增删。
3. 反向治理清单(第 4 节)的**存放位置**: 留在本文件, 还是同步在 GenUI 侧 `PLAN.md` 立一个域/条目。
4. 本工程是否纳入 Git 版本管理(当前无 `.git`; 工作区 `UIDemo\` 为 Demo 前身副本, 是否清除)。
5. 第 4 轮新增的框架层首产件(`framework\ImGuiHost` 三件)与范例编号 EX-10 是否照此登记;
   首产件是否要另立"框架层新增件"目录或命名前缀, 以便与 GenUI 快照件在目录上一眼可分。
