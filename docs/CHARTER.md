# Demo · 纲领登记

> 本文件登记用户对本项目下达的纲领, 逐条追加, 不改写历史条目。
> 纲领原话照录(含标点); 解读另列并注明是否经用户确认。
> 展开文档: 纲领一 ⇒ `docs\EXAMPLES.md`; 纲领二 ⇒ `docs\DERIVE.md`。
>
> **公开版说明**: 本仓库为公开版本, 已移除逐轮过程留痕(`docs\HISTORY\` 等)与内部工程管理文档。
> 文中指向这些目录的引用仅用于说明沿革, 对应文件不在本仓库内。

## 纲领一(2026-10-15)

原话:

> Demo 项目将会创建典范框架使用范例，用以反向治理 GenUI 项目

解读(经用户 2026-10-15 校正):

"典范"是修饰语, 修饰"框架使用范例" —— 本项目创建**典范的框架使用范例**(可作标杆的用法示范);
"反向治理 GenUI 项目"是这些范例的**用途**。

展开: `docs\EXAMPLES.md` —— 典范判据七条 · 范例清单 EX-00..EX-09 · 反向治理清单分级 · 布局文档硬约束与实测证据。

## 纲领二(2026-10-15)

原话:

> 第二份纲领是：后续其他本框架代码，将从本项目开始

解读(经用户 2026-10-15 确认, 选项 A):

**本项目是框架消费方项目的派生源(母本)** —— 后续其他使用本框架的项目, 以本项目为起点派生(复制)出去,
不再从 GenUI 复制。对工程的直接要求 = **可派生**: 身份集中、无历史包袱、派生清单与派生后自证口径明确。

展开: `docs\DERIVE.md` —— 派生五步 · 必改项 / 可选改动 · 派生后自证口径 · 无需改动的部分。

### 落地状态(2026-10-15)

| 措施 | 状态 |
| :--- | :--- |
| 代码侧身份集中到 `src\Identity.h`(文档名 / 环境开关 / 窗口类名与标题 / 事件名) | 已落地 |
| 构建侧身份收敛为 `CMakeLists.txt` 顶部两变量(`APP_PROJECT_NAME` / `APP_DOC_NAME`) | 已落地 |
| `tools\build.ps1` 与工程名解耦(解决方案与工程文件按 `out\` 通配发现) | 已落地 |
| 验证: 完整流程 `cfg → inject → restore → release` 全绿(0 警 0 错) + 缺省/覆盖两次运行与原行为逐行一致 | 已落地 |
| 布局有效性自检(把 `docs\EXAMPLES.md` §3.1 的静默失败变成显式诊断) | 待办(见 `docs\DERIVE.md` §4) |

## 纲领三(2026-10-15)

原话:

> Demo 项目，需要将 ImGui 初始化独立并加入框架层

追问口径(用户答复原话):

> 全套，但是需要提供一个接口表示本应用需要创建的标准 Wnd 数量，并且在布局激活 hWnd 数量不足时自动隐藏多余 Wnd
> 只加在 Demo 的 src\framework
> 配色可以通过相关接口初始化注入或运行时调整

解读(照录原话, 未另作确认):

1. **范围 = 全套**: ImGui 设施整体独立(上下文 + IO 配置 + 中文字体 + 样式 + Win32 / DX11 后端 +
   帧流程 + 窗口过程转发 + 成对释放), 不是只搬"初始化"一段。
2. **标准窗口数量是一等接口**: 应用声明"需要创建的标准窗口数", 运行时按"布局激活所需窗口数"
   自动隐藏多余窗口。
3. **落点 = 本工程 `src\framework` 首产**: GenUI 侧本轮不动 —— 本工程因此获得"首产框架层新增件"
   的授权; 这与 `docs\EXAMPLES.md` §2 补充纪律("不得就地改写快照语义")不冲突 ——
   **新增**允许, **回改既有快照语义**仍须走 GenUI 的 `LOCK.md` / `PLAN.md` 流程。
4. **配色的注入面**: 投影配色可经接口在初始化时注入, 也可在运行期调整。

落地: `src\framework\ImGuiHost.{h,cpp,md}`(框架层首产件) + 宿主侧改用框架接口;
证据与哈希见 `docs\HISTORY\2026-10-15-000004.md`, 接口文档见 `src\framework\ImGuiHost.md`。

## 纲领四(2026-10-16)

原话:

> Demo 项目，原有 UI 框架结构发生重大变更，第一件事
> Render.cpp 与 ImGui 彻底解耦，这一部分能力由其他文件承担，Render 仅根据上游文件提供的接口渲染布局

追问口径(用户答复原话):

> 由 ImguiHost 向 Render.cpp 注入，然后可以使用 Imgui 的能力，调用 Controls 绘制控件，未来使用其他 UI 实现时改为由其他文件注入即可

> 函数指针的形式，本项目如无其他外部库涉及，一律不使用抽象接口

解读(照录原话, 未另作确认):

1. **结构变更的第一件事 = 渲染侧与 ImGui 彻底解耦**: `Render.cpp` 不再直调 ImGui, 该部分能力由其他文件
   承担; `Render` 只根据上游文件提供的接口渲染布局。本工程按用户答复的命名落为: `Render.h` 定义接口,
   `ImGuiRender` 提供能力, `LayoutRender.h` 提供外部调用接口, `LayoutRender.cpp` 经 `Render.h` 的接口
   调用 `ImGuiRender` 的实现, 以响应 `LayoutRender.h` 接受的渲染需求。
2. **接口形式 = 函数指针**: 不用 C++ 抽象基类 —— 本工程在无外部库要求时不引入虚函数与 RTTI。
3. **注入方向 = 由 `ImGuiHost` 发起**: `ImGuiHost` 是"UI 实现 与 布局渲染"的装配点。
4. **未来换 UI 实现**: 改为由其他文件注入另一份同形态的能力表, 消费件零改动。

落地: `src\framework\Render.h`(渲染能力接口表) + `src\framework\ImGuiRender.{h,cpp}`(ImGui 侧能力提供者) +
`src\framework\layout\LayoutRender.{h,cpp}`(对外渲染出口, 不依赖任何 UI 库); 原 `src\framework\Render.cpp` 删除。
证据见 `docs\HISTORY\2026-10-16-012744.md`; 注入契约见 `src\framework\ImGuiHost.md` §3.6。

## 纲领五(2026-10-16)

原话:

> Demo 项目，原有 UI 框架结构发生重大变更，第二件事
> LayoutRender 现在不再接受 hWnd 作为参数，也不接受 WndIndex 作为参数，这是 ImGuiHost 和 ImGuiRender 之间需要协调的权责分离问题。
> 渲染时，数据端不关心到底渲染了谁，因为本框架模型本身时 hWnd 无关的，具体的事件也被 ImGui 代理了，布局对象是一个纯描述性的内容。ImGuiRender 拿到布局对象之后，由谁渲染时渲染器的事情

追问口径(用户答复原话):

> ImGuiRender 要反向调用 ImGuiHost 提供的接口。ImGuiHost 提供主消息循环，但是把实际业务全部外部委托出去，WndProc 保留在 ImGuiHost
> 整体流程时：每一帧，ImGuiHost 消费所有后端时间后，固定调用外部的处理流程（以指针形式传入），这个外部处理流程负责渲染层和数据层之间先后更新顺序或单纯调用 LayoutRender 进行渲染

> 更改一下 AcquireWnd 的签名，直接包含这个窗口的尺寸和位置，则不再需要 CheckWindowResize

> AcquireWnd 的参数是输入参数，如果当前 hWnd 的尺寸与传入不符，则需要 AcquireWnd 主动调整

补充拍板(同轮问答, 用户选定):

| 待定项 | 用户选定 |
| :--- | :--- |
| 帧驱动 | 外部驱动: ImGuiHost 每帧只调一次外部处理流程 |
| 窗口推进 | `RenderApi` 新增 `AcquireWnd`(第 17 项), 由 `RenderBase` 路径显式调用; `RenderFlex` 沿用当前窗口 |
| AcquireWnd 签名 | `BOOL AcquireWnd(CONST vec2& size)` —— 只传期望客户区尺寸(不含位置); 不符则由实现方主动调窗 |
| CheckWindowResize | 取消(窗口尺寸跟随并入 AcquireWnd) |
| 渲染清单 | `struct wnd{ layout* main; child*…; wnd* next; }` 外部私有, 框架层不引入 |
| 设施归属 | 呈现 / 交换链 / 遮挡 / IME / 窗口圆角由 ImGuiHost 创建并持有; 复用 `common\D3D11Manager`; 外部任何层面不持久持有 |
| 帧抓取 | 取消(`DEMO_FRAME_DUMP` 与后台缓冲回读整体删除) |
| 主循环 | ImGuiHost 持循环; 退出判据外露(外部持有的变量地址) |
| hWnd 传出 | `param` 为通用参数槽, 需要时携带窗口句柄(如标题栏拖拽) |
| IME | 两者合并: ImGuiHost 默认取 `io.WantTextInput`, 外部可每帧覆盖 |
| 验收 | 日志 + 绘制量自证(本工程不再产出 BMP, 无像素判据) |

解读(经用户 2026-10-16 目标对齐确认):

1. **渲染出口去窗口身份**: `LayoutRender` 与 `Render.h` 不再出现 hWnd / 窗口序号 —— 布局对象是纯描述,
   "渲染到哪个窗口"由渲染器按渲染顺序决定; 事件已由 ImGui 代理, 数据端不持句柄。
2. **权责三分**: 数据端(布局/文档/编排)只描述"有什么"; 布局渲染层(`LayoutRender` + 能力表)只描述
   "一份布局怎么画出来"; UI 宿主绑定层(`ImGuiHost` + `ImGuiRender`)独占窗口表 / 消息循环 / 上下文 /
   设备 / 呈现 / IME / 圆角。
3. **ImGuiHost 收回渲染链**: 主消息循环、`WndProc`(业务外部委托)、每窗口一套 ImGui 上下文与两端后端、
   D3D 设施、呈现、遮挡判定、IME、窗口圆角一并归 ImGuiHost; 每帧消费完消息后固定调用一次外部处理流程
   (函数指针传入)。
4. **外部处理流程**: 负责数据层与渲染层的先后更新顺序, 或单纯按外部私有的渲染清单顺序调 `LayoutRender`;
   宿主不再持 D3D / IME / 后台缓冲 / 持久句柄。
5. **窗口尺寸跟随**: 由新增能力 `AcquireWnd(size)` 在取得渲染目标时一并完成(尺寸不符即主动调整窗口),
   原 `CheckWindowResize`(渲染出口返回建议尺寸 + 宿主调窗)随之取消。

落地: `src\framework\Render.h`(第 17 项能力 `AcquireWnd`) + `src\framework\layout\LayoutRender.{h,cpp}`(删
`CheckWindowResize`, `RenderBase` 开窗前取窗口) + `src\framework\ImGuiHost.{h,cpp}`(持循环与设施) +
`src\framework\ImGuiRender.cpp`(反调宿主) + `src\demo\DemoHost.cpp`(宿主剥离设施, 注入处理流程与退出变量);
证据见 `docs\HISTORY\2026-10-16-*.md`(本轮)。

## 纲领六(2026-10-16)

原话:

> Demo 项目尝试不在 TitleBar 实现处利用 hWnd 进行窗口移动，而是装填窗口的 pos 值，最后由 AcquireWnd 时消费并产生实质移动。完成后需要保留 demo_old.exe 我对比尝试手感，如果手感不佳我会还原本次改动

追问口径(用户答复原话; 澄清"TitleBar 实现处"的所指):

> 不要臆测：
> TitleBar 实现处 指标题栏当前绑定的事件，现在 不再传递 hWnd，而是传递期望坐标
> AcquireWnd 发生在窗口实际绘制之前，所以有机会调整大小和位置

解读(照录原话, 未另作确认):

1. **装填点 = 标题栏当前绑定的事件**: "TitleBar 实现处"指标题栏绑定的事件处理器
   (`DemoEvents::OnDragWindow`) —— 其 `param` 通用槽由"当前渲染窗口句柄"改为"窗口期望坐标",
   该处理器不再接收也不使用 hWnd。
2. **移动动作移出事件处**: 事件处只把期望位置"装填"给渲染能力表(新增第 18 项能力 `SetWndPos`),
   不调用任何窗口 API; 拖动基线的读取(窗口当前桌面位置)仍由标题栏控件在装填前完成。
3. **消费点 = AcquireWnd**: 装填值由 `ImGuiHost::AcquireNextWnd` 在**下次取得该窗口渲染目标时**消费,
   并在该窗口绘制之前产生实质移动(AcquireWnd 先于窗口绘制 ⇒ 有机会先摆好几何) ⇒ 移动比"提出请求的
   那一帧"晚一帧。
4. **本轮为可回滚试验**: 保留回滚基线 `Demo\out\Release\demo_old.exe`(改动前 `Demo.exe` 的原件副本,
   与改动前逐字节一致); 手感不佳即整体还原本轮改动。

落地: `src\framework\Render.h`(第 18 项能力 `SetWndPos`) + `src\framework\ImGuiHost.{h,cpp}`
(`PostWndPos` / `GetWndPos` 与 `AcquireNextWnd` 的位置结算) + `src\framework\ImGuiRender.cpp`(反调宿主) +
`src\framework\ControlSpec.h`(标题栏参数槽的打包 / 解包口径) + `src\framework\Controls.cpp`(标题栏算期望位置、
`param` 改携期望坐标) + `src\demo\DemoEvents.{h,cpp}`(事件处理器改为装填); 证据见
`docs\HISTORY\2026-10-16-093925.md`。

### 口径修正(2026-10-16, 用户指正; 取代上文解读第 1–3 条)

用户指正原话:

> 我检查到，你没有在 AcquireWnd 处新增参数来消费 pos，而是以预期之外的方式，这不符合要求

> 你认为 WND 控件的 position 仍是 ImGui 内的布局坐标，但是实际上，我让 TitleBar 调用控件来消费这个 position 就是让他来记录这个坐标的，为什么不在 TitleBar 控件中调用，因为允许无法拖拽的窗口

追加拍板(用户答复):

| 待定项 | 用户选定 |
| :--- | :--- |
| 位置记录的初值 | 解析期三支: 布局文件中 pos = `{-1,-1}` ⇒ 随机位置; 非 `{-1,-1}` 且在屏内 ⇒ 直接绘制; 越屏 ⇒ 降级随机 |
| 位置声明字段 | WND 控件的 `x` / `y`(布局级 `wndX` / `wndY` 退位, 不再作位置来源) |
| 随机位置口径 | 主显示器工作区内随机(避开任务栏); 进程启动随机一次 |
| `OpenWnd` 摆放位置 | 布局原点 `(0,0)`(桌面位置与 ImGui 内坐标解耦) |
| 拖动基线来源 | 本轮先试"读 WND 件的 position"; 手感不佳则回退到"读真实窗口矩形" |

修正后的口径:

1. **位置记录 = WND 控件的 `position`**: 声明期给初值(解析期三支规则), 运行期由窗口级交互改写; 它同时是
   宿主建窗几何与渲染目标结算的真源。
2. **记录点 = 标题栏绑定的事件**: 标题栏只上报"窗口期望坐标"(经 `param` 通用槽, 不再传 hWnd), 由该事件
   把坐标记进 WND 件的 `position` ⇒ 未绑定事件的窗口不可拖动(不在 TitleBar 里直接改, 正是为了允许
   不可拖拽的窗口)。
3. **结算点 = `AcquireWnd` 的参数**: `RenderApi::AcquireWnd` 签名改为
   `BOOL AcquireWnd(CONST vec2& pos, CONST vec2& size)`(直接带期望位置与尺寸, 不用侧通道),
   `ImGuiHost::AcquireNextWnd(pos, size)` 在取得渲染目标时(该窗口绘制之前)一并结算位置与尺寸。
4. **手感判定(2026-10-16 用户本地复核, 原话"手感略差，但是可以接受")**: 上述口径采纳为现行口径; 备选口径
   (拖动基线读真实窗口矩形)保留为一行常量开关(`src\framework\Controls.cpp` 的 `GRAB_BASE_BY_WND_POS`)。
   对照结论与"略差"的可能机制见 `docs\HISTORY\2026-10-16-101915.md` §10。

落地: `src\framework\Render.h`(签名带 pos) + `src\framework\ImGuiHost.{h,cpp}`(`AcquireNextWnd(pos, size)`
结算几何; `GetWndPos` 保留为基线回退入口) + `src\framework\layout\LayoutRender.cpp`(取 WND 件的记录值; `OpenWnd`
改传布局原点) + `src\framework\Controls.{h,cpp}`(`UpdateContext` 注入位置记录槽; 标题栏读它建基线) +
`src\framework\GeometrySolver.{h,cpp}`(`ResolveWndPos` 三分支) + `src\framework\BasicDesign.{h,cpp}`
(`ScreenRef` 显示器工作区查询) + `src\demo\DemoEvents.{h,cpp}`(事件写位置记录) + `src\demo\DemoScene.{h,cpp}` /
`Demo.json`(位置声明迁到 WND 的 x/y; 建窗几何读位置记录); 证据见 `docs\HISTORY\2026-10-16-101915.md`。

## 纲领七(2026-10-16)

原话:

> Demo 项目推进，
> Controls 部件层正式提高身份，包括 Text、Button 、Combo 在内的已有控件，全部都需要新增部件形式。
> 控件层不再优先直接使用 ImGui 原生控件，而是改为优先使用基础部件
> 另基础部件在原有的三个固定参数的基础之上，新增第四个固定参数，表示绘制方式
> 绘制方式存在两种：
> 1. 提供一个配色方案，控件按照配色方案绘制（部件不直接解除层号）(该绘制方式主要服务于工具应用)
> 2. 提供一组 ImTextureRef，配色方案按照 ImTextureRef 绘制（该绘制方法主要服务于未来设计游戏时的应用）
> 当前，实际传递和消费的都是第一种

追问口径(用户答复原话):

> 分多阶段，当前阶段仍然保留在 Controls 中

> 只补直调 ImGui 原生控件的同名部件

> enum desc{PALETTE; IMAGE}; Widgets::Base::<name>(id,pos,size,{desc; unino {palette;void*}}...)

> 实际上，当使用 ImTextureRef 时，我会让他顺着 CtrlInfo 一起传过来

指正(同轮, 用户原话):

> TitleBar 是没有直调，这个没有对应控件层，他的最终形态是调用 Text 部件加 ExitSign 部件，你要自行区分，那些才是真正的基础部件，TitleBar 明显不是

补充拍板(同轮问答, 用户选定):

| 待定项 | 用户选定 |
| :--- | :--- |
| 契约作用面 | 所有基础部件统一改造(不止新增件) |
| 下沉边界 | 占位 Dummy / 命中项 InvisibleButton / 列表拖动辅助不下沉(不具顶层控件意义) |
| Combo 命名 | `ComboField` 改名为 `Widgets::Base::Combo` |
| 纹理载具 | 随控件缓冲传入; 当前尚未启用, 先预留能力 |

解读(照录原话, 未另作确认):

1. **部件层分阶段提高身份**: 第一阶段(本轮)不拆文件 —— 部件仍留在 `Controls.cpp` 内, 只完成"契约统一 +
   同名部件补齐 + 去层号"; 物理独立(独立 `Widgets.{h,cpp}` 与接口文档)留待后续阶段。
2. **基础部件的判据 = 最小绘制单元**: 一枚控件对应一枚真正的基础部件(Text / Button / CheckBox / Combo /
   RenderSurface 一类); **组合体不立部件** —— TitleBar 的最终形态是调用 `Text` 部件 + `ExitSign` 部件。
3. **第四固定参数 = 绘制方式**: 基础部件固定参数由三枚变四枚 —— `(id, pos, size, 绘制方式)`;
   绘制方式 = `desc` 判别位 + 载荷联合(`PALETTE` 取成品配色方案 / `IMAGE` 取纹理载荷)。
4. **部件不接触层号**: "层次 → 配色"的采样一律留在控件层(取色区), 部件只消费成品配色方案;
   `Command::ListSpec` / `Pipe::ListPipe` 的层次值字段随之改为成品配色与壳体底色。
5. **方式二服务未来游戏应用**: 纹理引用随控件缓冲传入(不在 `CtrlInfo` 上加字段, 也不把纹理类型带进
   `ControlSpec.h` 的零 UI 库边界); 当前实际传递与消费的都是方式一。

落地: `src\framework\Controls.cpp`(新增 `desc` / `PaintWay` / `MakePaletteWay` 契约; 13 件签名统一为四固定参数;
新增 `Button` / `Text` / `CheckBox` / `RenderSurface` 四枚同名部件; 取色上移至控件层 `MakeFramePalette` /
`MakeListPalette`; `ComboField` 改名 `Combo`) + `README.md` §1/§5/§6; 证据见 `docs\HISTORY\2026-10-16-120340.md`
(构建两配置 0 警告 0 错误 · 无人值守运行退出码 0 · 绘制量收尾行与基线逐字段一致)。

## 纲领七 · 第二阶段(2026-10-16 第 14 轮: 部件层物理独立)

用户指令原话:

> Demo 项目继续推进，部件与控件开始分离，部件单独放入 Widgets.cpp 文件，原本的上下文环境，也如同 Controls 一样，通过 UpdateContext 设置更新。

核准(本轮问答, 用户选定):

| 待定项 | 用户选定 |
| :--- | :--- |
| 拆分产物形态 | 新建 `Widgets.h` + `Widgets.cpp`; 命名空间**不变**(仍为 `Controls::Widgets`) |
| 部件上下文内容 | 参考尺寸 + 投影配色(两枚绝对档位, 只服务列表拖动辅助取色) |
| 跨层共用辅助归属 | 尽量留控件层, 部件层自留内部副本 |

落地: `src\framework\Widgets.{h,cpp}`(部件声明 + 实现; `Controls.cpp` 由 3862 行降至 1825 行)
+ `src\framework\Controls.{h,cpp}`(上下文转调 + 注释路径同步) + `CMakeLists.txt`(源表增 `Widgets.cpp`)
+ `README.md` §1/§2/§5/§6 + `docs\EXAMPLES.md`(行号引用重定位); 证据见
`docs\HISTORY\2026-10-16-144123.md`(构建两配置 0 警告 0 错误 · 无人值守运行退出码 0 ·
场景 / 控件 / 窗口 / 事件自证与基线一致)。

与第一阶段设想的两点偏差(按本轮用户口径执行): ① 命名空间**不提为顶层**(仍 `Controls::Widgets`);
② 本轮**不建** `Widgets.md` 接口文档 —— 部件声明自带完整中文契约注释, 接口文档留待后续阶段按需补。

## 纲领七 · 第二阶段后续(2026-10-17 第 17 轮: 部件层接口文档与注释治理)

用户指令原话:

> 部件层是我当下最希望推进的方向，但是当前部件层的注释太多了，影响我阅读代码，所以需要你将部件层的相当一部分注释移除。可以建立 Widgets.md，另外有相当一部分注释实际上在其他文档中已经描述过了，但是在注释中又被描述了一次，也需要移除

落地: 新建 `src\framework\Widgets.md`(部件层接口文档 —— 职责与边界 / 三种部件形态 / 固定参数与绘制方式 /
部件 → 调用方 返回约定 / 逐件语义 / 命令与管道部件 / 部件上下文 / 内部件 / 失败口径; 接口语义唯一真源) +
`Widgets.{h,cpp}` 注释按"文档已有的意图不进代码 + 其它现行文档已叙述过的不重复"瘦身
(净减 552 行, 零代码行改动; 两配置 0 警 0 错 + 无人值守退出码 0 + 收尾绘制量与第 13 轮基线逐字段一致)。
⇒ 上面《第二阶段》末条登记的偏差"**不建** `Widgets.md` 接口文档"由此消解;
另一条偏差(命名空间不提为顶层)仍按第 14 轮口径维持。证据见 `docs\HISTORY\2026-10-17-022005.md`。

## 纲领八(2026-10-18)

原话:

> Demo 项目中的部件自绘原本是为了解决控件高度与设计不符，但是在 GenUI 项目时就已经过拟合扩大了自绘面
> 现在请你检查Demo 项目中那些部件不需要自绘，调用原有设计即可，如存在，请先列出我确认

确认口径(用户答复原话):

> InputText、InputScalar、InputTextMulti、CheckBox、TextMulti 推进

解读(照录原话, 未另作确认):

1. **自绘的原始动机只有一个** = 控件高度与设计不符; 纲领七"控件层不再优先直调 ImGui 原生控件"把这个手段
   扩大到了所有控件, 属过拟合 —— 本纲领要求按"是否需要自绘"重新甄别。
2. **判据** = 若 ImGui 原生控件连同项目已注入的样式槽就能画出同一观感, 则该部件的自绘不必要;
   高度差可由原生帧的垂直内边距(帧高 = 设计字号 + 2 × 内边距)表达, 不必用自绘外框调和。
3. **本轮范围 = 五件**(用户点名): `InputText` / `InputScalar` / `InputTextMulti`(去外框自绘) ·
   `CheckBox`(回原生复选框) · `TextMulti`(回原生折行)。`Combo` 与承载专有观感的件(滑块 / 时码 / 进度底 /
   关闭钮 / 渲染占位面 / 两段件底框 / 列表拖动辅助)不在本轮范围。
4. **观感变化的接受范围**: 复选框方框与勾的原生形态、多行件框与文本内距、帧类件命中区高度 ——
   以用户本地观察为准(本工程无像素基线, 见 README §5)。

落地: `src\framework\Widgets.{h,cpp}`(三件删补画块并改用原生帧 + 垂直内边距; `CheckBox` 改原生复选框、
`IsChecked` 改引用形参并返回专属段; `TextMulti` 改 `PushTextWrapPos` + `TextUnformatted`) +
`src\framework\Controls.cpp`(CheckBox 去掉整矩形命中项, 改为消费部件结论并写回缓冲) +
`src\framework\Widgets.md` / `Controls.h` 注释同步; 证据见 `docs\HISTORY\2026-10-18-121500.md`
(两配置 0 警 0 错 · 无人值守运行退出码 0 · 控件 14 / 窗口可见 1/1)。

后续(2026-10-18 第 22 轮, 用户指令"Combo 部件参考 ComboAction 的实现方式，修改自绘"): 本纲领的判据同样适用于
`Combo` —— 其底改由 ImGui 原生帧绘制、边框改为与 `ComboAction` 同源的两段件补画口径(通道 1 的 1px `AddRect`、
色随项状态), 展开列表的项表抽成内部件 `ComboItems` 由 `Combo` / `ComboAction` 共用(行高 = `controlHeight`、
行距 0、文字行内垂直居中)。证据见 `docs\HISTORY\2026-10-18-114336.md`。

## 纲领九(2026-10-21)

用户方向(本轮任务书要点, 逐条照录):

> main.cpp 只做三件事 + 收尾：Application::Init → ImGuiHost::Start → ImGuiHost::Run → Release / DestroyWnds / Application::Release
> Application 是静态单例（全静态成员 + 私有删除构造析构 + AppData 成员块），形态照 VideoCut；
> DemoHost 整体删除；DemoScene 不再是公开类，实现收进 Application 的既定流程（demo/ApplicationScene.cpp）；
> DemoEvents 独立于 Application 之外，Application::Init 通过函数指针接收事件绑定表；
> WndProc 收进 ImGuiHost 内部（HostWndProc 自己处理 WM_DESTROY / ESC / DefWindowProcW），不再对外暴露、不再由宿主传；
> ImGuiHost::Start 聚合 SetStandardWndCount → CreateWnds → Acquire，首步 EnableDpiAwareness；
> 建窗只需要数量：默认客户区 800x600、屏幕工作区居中、字号 18、圆角 10；窗口类名 = <APPNAME>_IMGUI_CLASS，标题默认 = appName；
> AcquireNextWnd(pos, size, title) 增标题参数（UTF-8，变化才转 UTF-16 调 SetWindowTextW，NULL 不改）；
> RenderApi::pfnAcquireWnd 同步带标题；LayoutRender::RenderBase 传 pWnd->label；
> 每帧处理流程在 Application 实现一次（Application::Frame），main 取指针传给 ImGuiHost::Run；Run 签名不变，用户可自定义 FrameFunc；
> 退出判据 = Application 持 BOOL bExit，GetExitAddress 返回其地址；
> Identity.h 新增 IDENTITY_APP_NAME = "Demo"，删除 IDENTITY_WND_CLASS 与 IDENTITY_WND_TITLE，保留 IDENTITY_ERR_TITLE。

解读(照录原话, 未另作确认):

1. **main 重新成为胶水**: 进程入口不再承载宿主生命周期与业务编排, 只保留"初始化 → 宿主启动 → 主循环 →
   释放"五步与两条中文错误框。
2. **Application = 静态单例协调层**: 全静态成员 + 私有删除构造析构 + 会话分配器 arena 内的 `AppData` 成员块,
   形态照 VideoCut; 管辖控制台 UTF-8、事件绑定表注入、会话分配器注入、文档解析、布局展开、几何解算、
   投影配色 ProjCR、样式注入、每帧流程、无人值守参数、退出判据与清理。
3. **DemoHost 删除 / DemoScene 降为实现切片**: 场景装配的分配与释放收进 `demo\ApplicationScene.cpp` 的
   私有静态函数 `LoadScene` / `UnloadScene`, 不再暴露类; `DemoEvents` 独立于 `Application` 之外, `Init` 经
   函数指针取绑定表(不直接依赖 `DemoEvents`)。
4. **后端启动接口**: `ImGuiHost::Start(数量, 应用名, 段前回调, 用户指针)` 聚合"声明数量 → 建窗 → 建立渲染设施",
   首步 `EnableDpiAwareness`, 失败内部销毁已建窗口并回退; 建窗只需数量, 默认客户区 800x600、屏幕工作区居中、
   字号 18、圆角 10, 窗口类名 = `<APPNAME>_IMGUI_CLASS`, 标题默认 = 应用名。
5. **WndProc 内置**: 窗口过程收回 `ImGuiHost` 内部(自己处理 `WM_DESTROY` / `ESC`, 其余 `DefWindowProcW`),
   不再对外暴露、不再由宿主传入; `CreateWnds` 去掉 `WNDPROC` 参数, 全部窗口共用一个窗口类。
6. **标题进渲染能力**: `AcquireNextWnd(pos, size, title)` 与 `RenderApi::pfnAcquireWnd` 增标题参数(UTF-8;
   变化才转 UTF-16 调 `SetWindowTextW`; `NULL` 不改), `LayoutRender::RenderBase` 传 `pWnd->label`。
7. **帧流程集中在 Application**: 每帧处理流程在 `Application::Frame` 实现一次, `main` 取指针传给 `ImGuiHost::Run`;
   `Run` 签名不变, 用户仍可自行写 `FrameFunc` 传入。退出判据 = `Application` 持 `BOOL bExit`,
   `GetExitAddress()` 返回其地址。
8. **身份收敛**: `Identity.h` 增 `IDENTITY_APP_NAME = "Demo"`, 删除 `IDENTITY_WND_CLASS` 与 `IDENTITY_WND_TITLE`
   (已无使用者), 保留 `IDENTITY_ERR_TITLE`。

落地: 新增 `src\demo\Application.{h,cpp,md}` 与 `src\demo\ApplicationScene.cpp`; 删除 `DemoHost.{h,cpp,md}` 与
`DemoScene.{h,cpp,md}`; 重写 `src\main.{cpp,md}`; 改造 `src\framework\ImGuiHost.{h,cpp,md}`(`Start` / 共用窗口类 /
内置窗口过程 / 标题参数)、`src\framework\Render.{h,md}`、`ImGuiRender.{h,cpp,md}`、`layout\LayoutRender.{h,cpp,md}`、
`src\Identity.{h,md}`、`src\demo\DemoEvents.{h,cpp,md}` 与 `CMakeLists.txt`; 证据见本轮
`docs\HISTORY\2026-10-21-*.md`(两配置 0 警告 0 错误 · 无人值守运行退出码 0 · 收尾自证与基线同量级)。

## 纲领十(2026-10-21)

原话来源 = 本轮任务书 `docs\HANDOFF-RESTRUCTURE-20261021.md`: 第 1 节"任务目标"与第 3 节"已定决策(不可再议)",
以下逐字照录(未改字)。

任务目标(第 1 节):

> 把 Demo 从"一个应用"重组为"框架内核 + 项目壳": `Application` 改名 `Dispatch` 并入 framework,
> 业务层只保留事件表与身份; framework 按 `base` / `render` / `layout` / `dispatch` 分层,
> `render` 按技术名分目录(`render/imgui/{ImGuiHost, ability, control}`),
> 控件绘制与宿主访问改为接口注册制。

决策表(第 3 节, D1–D8):

> | D1 | 每种渲染技术的 Host 提供 `Register()`, 由 `main` 选择并取得 `HostApi*`; Dispatch 只消费接口 |
> | D2 | `ImGuiHost` 与 `ability/`、`control/` 平级, 放在 `render/imgui/` 下 |
> | D3 | `CtrlApi` 3 项(UpdateContext / DrawCtrl / HasActiveInput); 控件缓冲装配下沉 base |
> | D4 | base 内容: BasicDesign / ControlSpec / CtrlData / CtrlBuffer / FuncPtr / TextWidth |
> | D5 | 目录小写、文件名大驼峰、文档叙述用大驼峰; include 前缀随目录小写 |
> | D6 | 先提交一次纯搬迁(S1), 再分步改内容; 每一步单独提交 |
> | D7 | 亮暗默认跟随系统; `main` 可经 `Dispatch::SetDarkMode` 覆盖 |
> | D8 | `Version` 无关: 本任务不升 `BIN_VERSION`, 不动文档格式 |

> 后续变更(2026-10-22): base 的 `CtrlData` 已并入 `CtrlBuffer`(数据布局、容量常量、缺省文案与文本通道取址
> 随缓冲约定同置一处), 现行 base 内容 = BasicDesign / ControlSpec / CtrlBuffer / FuncPtr / TextWidth,
> 见 `README.md` §2。

解读(按分层语义; 依据 = 任务书第 1 节的任务目标与第 4.3 节的依赖表):

1. **框架内核 + 项目壳**: framework 承担"渲染 / 布局 / 调度"全部机制, 项目的可替换面收缩为事件表
   (`demo\DemoEvents`)与身份(`Identity.h` + `CMakeLists.txt` 两变量 + 布局文档); 派生新项目不再复制应用层协调代码。
2. **四层单向依赖**: `base`(基础数据与控件类型)→ `render`(渲染能力契约与实现)/ `layout`(布局解算与渲染出口)
   → `dispatch`(调度编排)。两条受控例外: `layout` 可引用 `dispatch` 的两个契约头(`UIEventHandler.h` /
   `FrameEvent.h`); `dispatch` 只经 `HostApi` 访问宿主, 不引用 `ImGuiHost.h` / `imgui.h`, 也不引用 `Identity.h`
   (身份经 `DispatchConfig` 注入)。依赖表见 `README.md` §2.1。
3. **按技术名分目录**: 渲染契约(与 UI 库无关)留在 `render\Render.h`; 具体 UI 技术的宿主与实现按技术名收进
   `render\imgui\`, 其中宿主 `ImGuiHost` 与能力提供者(`ability\`)、控件与部件(`control\`)平级 —— 增加第二种
   渲染技术时新增同级目录, 契约与上层零改动。
4. **接口注册制**(本纲领的核心判据): 渲染设施、控件绘制、宿主访问各自收敛为一张函数指针表(`RenderApi` /
   `CtrlApi` / `HostApi`), 提供方填表、宿主在初始化期注入、消费方只经表调用; 具体宿主的取表入口是幂等的
   `Register()`。表内只含框架自有语汇(`vec2` / `RECT` / GDI 色值), 不含任何 UI 库类型 ⇒ 样式由调度层算成
   `StyleDesc` 交宿主映射, 调度层无需认识 UI 库。
5. **Dispatch 归位**: `Application` 改名 `Dispatch` 并从演示层挪进 framework, 去掉项目身份与 UI 库依赖;
   场景装配切成 `DispatchScene.cpp`(实现 `LoadScene` / `UnloadScene`), 演示层只留事件表。
6. **行为零变更**: 唯一允许的变化是日志前缀由配置拼出(默认仍 `[DEMO] `); 结构自证行(场景 / 控件层次 / 控件坐标 /
   事件绑定)逐字段一致是每步的硬判据。

落地(2026-10-21, S1–S4 各自一次提交; S5 为文档同步):

| 步 | 提交 | 内容 |
| :--- | :--- | :--- |
| S1 | `d339606` | 纯搬迁: 建 `framework\{base,render,layout,dispatch}` 与 `render\imgui\{ability,control}`; `git mv` 全部源文件与接口文档; 全量改 include 前缀; 改 `CMakeLists.txt` 源表(逻辑零改动) |
| S2 | `b531d49` | 控件绘制接口化: 新增 `base\CtrlBuffer.{h,cpp,md}`(三件缓冲原语) · `BuildCtrlInfoFromSpec` 与 `FreeLayoutCtrlBuffers` 迁入 `layout\LayoutBuilder` · `Controls::GetCtrlApi()` 提供 `CtrlApi`(3 项) · `LayoutRender` 改表调用 |
| S3 | `7489634` | 宿主接口化与 Dispatch 归位: `Render.h` 增 `HostApi` / `StyleDesc` / `WndShowState` / `pfnFrame` / `pfnSegment` · `ImGuiHost::Register()` · `Application` → `Dispatch`(配置 `DispatchConfig` 六字段, 日志经内部 `Log` 包装) · `main` 六步接线 |
| S4 | `9dad4b4` | 清理: 删 `LayoutModel.h` 的 `FormatToUI` 死声明(文档同步删条目) · 注释与残留名义改 `Dispatch` |
| S5 | 本次 | 文档同步: `README` §1/§2/§5/§6/§7 · `DERIVE` · `CHARTER`(本纲领) · `EXAMPLES`(第六例例外 + 本轮登记) · 29 份接口文档 |

验收(每步): 两配置 0 警告 0 错误 · 无人值守运行退出码 0 · 日志的场景 / 控件层次 / 控件坐标行与 S0 基线逐字符一致。
过程与证据见 `docs\HISTORY\2026-10-21-*.md`(S1–S5 各一份); 回灌材料见 S6 的 `docs\REORG-CHANGES.md`。

## 纲领十一(2026-10-22)

原话(对 `docs\EVAL-DX11-RELOCATION.md` §8 待拍板项的批复):

> D1：A
> D2：Demo 先行
> D3：只加注记
> D4：保留
> D5：S1+S2

解读(照录, 未另作确认):

1. **落点 = 方案 A**: `D3D11Manager.{h,cpp,md}` 平铺到 `src\framework\render\imgui\`(与 `ImGuiHost` 平级;
   不建 `backend\` 子目录, 不另立 `render\dx11\`);
2. **推进 = Demo 先行**: 本工程先落地并登记快照漂移; GenUI 侧仍在 `src\common\`, 后续回灌按同一路径
   (GenUI 侧流程另计);
3. **依赖表 = 只加注记**: 不新增 `render\imgui` 宿主行, 只在 `README.md` §2.1 加"`D3D11Manager` 只被同目录
   `ImGuiHost` 包含"的注记; `ImGuiHost → layout\LayoutRender` 的注入反调留待另一轮;
4. **命名 = 保留 `D3D11Manager`**: 不与移动同轮改名;
5. **执行 = S1 + S2**: S1 纯搬迁(三件 + 2 处 include + CMake 源表/注释), S2 文档同步与漂移登记。

落地: S1 提交 `a3486ec`; S2 为文档同步(README / DERIVE / EXAMPLES / CHARTER / 接口文档)并新增 HISTORY 留档。
评估依据见 `docs\EVAL-DX11-RELOCATION.md`; 例外登记见 `docs\EXAMPLES.md` §2 第七例。本纲领不修改纲领五的
历史原话("复用 `common\D3D11Manager`" 属当轮记载)。

