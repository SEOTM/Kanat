# Common 层归属评估(2026-10-23)

> 评估对象: `src\common\` 五件 —— `AllocatorHub` / `AppEnv` / `FileCodec` / `MemoryManager` / `ResourceLoader`。
> 触发问题(用户原话): 分析 Common 到底有哪些是平台、后端无关的真正的公共件, 哪些其实属于技术栈对应需要的,
> 哪些其实可以并入 framework 的 base 组。
> 本文只做评估与建议, 不改代码。体例沿用 `docs\CHARTER.md` 纲领十一的"评估 → 批复 → 落地"三段式。

## 1. 结论先行

五件里**只有 1 件**是名副其实的公共件(`FileCodec`), **2 件**属框架底座(可并入 `base`),
**2 件**属技术栈 / 平台宿主(应随技术栈迁出)。`common` 这一层当前的名实不符不是个例,
而是"平台设施 + 框架策略 + 技术栈能力 + 纯算法"四类东西共用了一个目录名。

| 模块 | 现落点 | 实际归属 | 平台无关 | 后端无关 | 框架无关 | 建议落点 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `FileCodec` | common | 纯算法(CRC32) | 算法是, 接口否(用 `DWORD`) | 是 | 是 | 留公共层(方案 A)/ `base\util`(方案 B) |
| `MemoryManager` | common | 平台设施(分配器) | 否(`VirtualAlloc`) | 是 | 是 | `base\mem`(建议同时拆出类型头) |
| `AllocatorHub` | common | 框架内存策略(会话分配器) | 否 | 是 | **否**(生命周期由调度层驱动) | `base\mem` |
| `ResourceLoader` | common | 平台 + 加载链(exe 目录 / PE 资源节) | 否 | 是 | **否**(只服务文档加载) | `dispatch` |
| `AppEnv` | common | 平台桌面栈(控制台 / 注册表主题 / DWM 圆角) | 否 | **否**(宿主外观) | 否 | 拆: 圆角 → `render\imgui`; 控制台 + 主题 → `dispatch` |

判据与逐件论证见 §3、§4, 落地批次与待拍板项见 §6、§7。

## 2. "平台无关"在本工程的可操作口径

必须先校正提问口径: **Kanat 是 Windows 专有框架**, 严格意义上的"平台无关"在本工程不存在——
`CMakeLists.txt:167` 只链接 `d3d11 dxgi dwmapi imm32`;`framework` 的公共头普遍直接 `#include <windows.h>`
(见 §3.4)。因此"平台无关"只能拆成四条判据 —— Q1 在本工程几乎没有区分度, 只作标注;

| 判据 | 问法 | 区分的是什么 |
| :--- | :--- | :--- |
| **Q1 OS** | 换操作系统是否要重写实现? | 平台件 vs 可移植件(本工程几乎全是"要", 故只作标注, 不作主判据) |
| **Q2 后端** | 换 UI 后端(ImGui → 其它)是否要改这个模块? | 技术栈件 vs 后端无关件(主判据之一) |
| **Q3 领域** | 删掉框架的文档 / 布局 / 调度领域后, 这个模块是否仍有意义? | 框架件 vs 公共件(主判据之二) |
| **Q4 共用面** | 是否被 ≥2 个互不相干的上层共用, 且不携带任何上层语义? | 公共件 vs 单消费者的实现细节 |

一条经验补充(Q3 的落地形态): **模块的语义若只能靠框架的一次具体决策来解释, 它就是框架件。**
`AllocatorHub` 的"池优先、超限回落、进程单例、随会话整体失效"正是这样的决策;
`FileCodec` 的 CRC32 则不需要任何框架决策来解释。

## 3. 现状事实

### 3.1 实际依赖图(按 `#include` 出处与调用点)

```
MemoryManager   ← base\BasicDesign.h:4 · base\ControlSpec.h:7 · layout\Layout.h:2
                  · layout\LayoutModel.cpp:2 · layout\LayoutSolver.h:4
AllocatorHub    ← base\CtrlBuffer.cpp:14 · layout\LayoutBuilder.cpp:8
                  · dispatch\DispatchScene.cpp:21 · common\ResourceLoader.cpp:10
ResourceLoader  ← dispatch\DispatchScene.cpp:22(调用点 131 / 140 / 578)
AppEnv          ← dispatch\Dispatch.cpp:21(调用点 86 / 109 / 495)
                  · render\imgui\ImGuiHost.cpp:25(调用点 472 / 1249)
FileCodec       ← layout\LayoutSolver.cpp:2(调用点 437–440)
```

### 3.2 消费者与调用面

| 模块 | 源行数(h+cpp) | 被引层数 | 被引文件 | 调用点 | 公共头依赖 |
| :--- | ---: | ---: | ---: | ---: | :--- |
| `MemoryManager` | 676 | 2(base / layout) | 5 | 多处 | 是(渗进 5 个框架公共头) |
| `AllocatorHub` | 147 | 3(base / layout / dispatch) | 4 | 20+ | 否(只在 `.cpp` 级被用) |
| `ResourceLoader` | 145 | 1(dispatch) | 1 | 4 | 否 |
| `AppEnv` | 98 | 2(dispatch / render) | 2 | 5 | 否 |
| `FileCodec` | 87 | 1(layout) | 1 | 4 | 否 |

> 行数 = `.h` + `.cpp` 的物理总行(不含 `.md` 接口文档)。

两个结构性事实:

1. `AllocatorHub` 的接口面**只出现在 `.cpp` 里** —— 没有任何框架公共头 include 它。
   这意味着它的搬迁/合并对框架公共契约零影响, 是五件里最好动的一件。
2. `MemoryManager` 恰恰相反: 它被五个框架公共头 include, 而其中三处只需要 `BufferInfo`
   这一个两字段结构(`ControlSpec.h:76`)。平台类型因此被带进了框架最底层的公共契约。

### 3.3 零消费者接口(死接口)

| 接口 | 位置 | 消费者 |
| :--- | :--- | :--- |
| `SecureBuffer` 整类(12 个公开成员) | `common\MemoryManager.h:36-92` / `.cpp:15-149` | **无** |
| `Arena::Mark` / `Rewind` | `MemoryManager.h:126-130` | **无**(仅文档注解提及) |
| `MemPool::SlabCount` | `MemoryManager.h:171` | **无** |
| `FileCodec::Crc32`(一次算完) | `FileCodec.h:34` | **无**(只用流式三件套) |
| `ResourceLoader::GetExeDir` | `ResourceLoader.h:20` | **无** |
| `ResourceLoader::LoadResource` | `ResourceLoader.h:32` | **无**(本工程无 `.rc` 资源链, 见 `CMakeLists.txt:14`) |

`SecureBuffer` 约占 `MemoryManager` 源行的三分之一、文档的 30 行, 是本层"过度设计"的最大一块;
`LoadResource` 则是 `EXAMPLES.md:233`(RG-A8)已登记的"取值真源落在应用层"的遗留件。
两者都不构成本轮搬迁的阻碍, 但应在评估后明确"保留待上游 / 就地标注未用"的口径。

### 3.4 平台类型的渗透面

`framework` 的公共面(头文件)中直接引 Windows 类型的共 **12 份**(另有 11 份 `.cpp` 同样直接 include),
其中含 base 五件中的四件:

| 公共头 | 引用 | 用途 |
| :--- | :--- | :--- |
| `base\BasicDesign.h:2-3` | `minwindef.h` + `windef.h` | `DWORD` / `RECT`(显示器工作区) |
| `base\ControlSpec.h:2` | `minwindef.h` | `BufferInfo` 里的 `VOID*` / `size_t` |
| `base\CtrlBuffer.h:2` | `minwindef.h` | `DWORD` / `LPCSTR` |
| `base\FuncPtr.h:2` | `minwindef.h` | `BOOL` / `LPCSTR` |
| `base\TextWidth.h` | 无 | 纯算法(唯一的零依赖头) |

结论: **"平台无关"在 Kanat 里已是既成事实的让步**, 本轮不应以"消除 Windows 类型"为目标
(那是跨平台改造, 不在收尾范围), 而应以"**谁的栈就归谁**"为目标。

### 3.5 `common` 内部其实是三级链, 目录却是平铺

`MemoryManager ← AllocatorHub ← ResourceLoader` 是明确的单向依赖链, 加上两件独立的(`AppEnv` / `FileCodec`),
实际是"底层一件 + 中间一件 + 上层一件 + 两件独立"。而 `CMakeLists.txt:139-145` 的 `KANAT_COMMON_SRC` 把五件平铺,
目录也无子分组 —— 这与 `framework` 已按 `base / render / layout / dispatch` 分层、`render` 再按技术名分目录的
现状**不对称**: 唯一没有内部结构的层, 恰好是唯一混装四类东西的层。

## 4. 逐件分析

### 4.1 `FileCodec` —— 真正的公共件(唯一)

- **语义**: 对"一段字节"做纯校验, 不感知文档结构, 不分配内存, 无状态。CRC32 = IEEE 802.3, 通用算法。
- **事实**: 87 行;唯一消费者 `layout\LayoutSolver.cpp:437-440`(bin 头校验);零 Win32 API 调用;
  但其接口用 `DWORD` / `LPCVOID` / `BYTE`(`FileCodec.h:2`), 所以"算法可移植、接口沾平台"。
- **判据**: Q1 否(算法可移植, 仅类型别名需换) · Q2 是(后端无关) · Q3 是(删掉布局领域它依然成立) · Q4 否(单消费者)。
- **结论**: 它是唯一**不需要任何框架决策来解释**的模块, 是公共件的正确形态:算法与"校验码写在格式的哪个字段"
  这一契约分离得很好(`FileCodec.h:7-11` 明文写了这层分离)。
- **建议**: 留公共层(A 方案), 或随 `common` 取消而并入 `base\util`(B 方案)。**不建议**迁进 `layout`——
  那会把"通用算法"降格成"文档格式的私有工具", 反向恶化它现在唯一的优点。
- **顺手项**: 若要让"公共件"名实相符, 把 `DWORD` / `LPCVOID` / `BYTE` 换成 `<stdint.h>` 的 `uint32_t` /
  `const void*` / `uint8_t` 即可(A 方案下 3 行改动, 零行为变更)。此事可做可不做, 不影响归类。

### 4.2 `MemoryManager` —— 平台设施, 语义属公共、实现属平台

- **语义**: `Arena` / `MemPool` / `SecureBuffer` / `AlignUp` / `BufferInfo`。前两者是任何 C/C++ 程序都会有的设施。
- **事实**: 676 行(五件里最大);实现全绑 `VirtualAlloc` / `VirtualFree` / `VirtualProtect` / `RtlSecureZeroMemory`
  (`MemoryManager.cpp:2-3`);被 base 与 layout 的**公共头**include(§3.4);`SecureBuffer` 整类零消费者(§3.3)。
- **判据**: Q1 是(换 OS 要重写) · Q2 是(与 UI 后端无关) · Q3 是(与文档/布局/调度无关) · Q4 是(base + layout 五处)。
- **结论**: 它是**平台设施**, 不是"公共件"(公共件应不含平台实现), 也不是"技术栈件"(与 UI 后端无关)。
  在只有三层的现实里, 它的正确位置是框架底座 —— 因为 base 与 layout 都必须用它, 而任何上层都不该自己再包一层。
- **建议**: 并入 `base\mem`。**关键增量动作**是把 `BufferInfo` + `AlignUp` 拆成零依赖的 `base\mem\BufferInfo.h`
  (仅 `<stddef.h>`), 让 `ControlSpec.h` / `CtrlBuffer.h` 这类"只需要一个两字段结构"的公共头不再拉进整个
  `MemoryManager.h` 与 Win32 类型。这一步收到的收益与搬迁本身相当。

### 4.3 `AllocatorHub` —— 框架内存策略(最明确应并入 base 的一件)

- **语义**: 进程级单例持有 `Arena` + `MemPool`, 提供"池优先、超限或失败回落线性分配器"的 `AllocSmall`
  与"字符串进 arena"的 `CopyString`。
- **事实**: 147 行;被 base(`CtrlBuffer.cpp:14`)、layout(`LayoutBuilder.cpp:8`)、dispatch(`DispatchScene.cpp:21`)
  与 common 内部(`ResourceLoader.cpp:10`)使用;`Init` 的调用点只有一处 —— `DispatchScene.cpp:529`
  (`AllocatorHub::GetArena() == NULL && !AllocatorHub::Init(DISPATCH_ARENA_SIZE)`), `Release` 只在
  `DispatchScene.cpp:849`。**它的生命周期完全由调度层驱动**, 消费方从不自己建立或销毁它。
- **判据**: Q1 否(自身仅用 CRT 的 `new` / `delete`; 平台性来自 `Arena`) · Q2 是 · Q3 **否**(会话分配器是框架对"一次布局会话"的内存决策) · Q4 是(三层共用)。
- **结论**: 归 base。它有 Q4 的"多层共用"但缺 Q3 的"领域无关", 因此既不是公共件, 也不该留在
  "与框架无关"的层里。它现在的落点是四类混装里最刺眼的一件: 名字带"会话(Hub)"、行为带框架策略,
  却与 `FileCodec` 并排。
- **并入后的确定性收益(实证)**: base 里已经存在一份**同构但独立演化的**策略 ——
  `BasicDesign.cpp:610-624` 的 `ProjCR::AllocLayers` 手写了"池优先、超限回落 arena"的判断与
  `AllocatorHub::AllocSmall`(`AllocatorHub.cpp:69-81`)一模一样, 并且为此在 base 里保留了
  `ProjCR::SetAllocators(Arena*, MemPool*)` 这一对注入接口(`BasicDesign.h:282` / `.cpp:491-494`)。
  原因是 base **不能**在语义上依赖 common 的"会话策略", 只能拿两个裸分配器自己拼。
  `AllocatorHub` 进 base 后, 这 15 行重复策略与一对注入接口可以直接删掉, `ProjCR` 改调 `AllocSmall`。
  **这是"并入 base"唯一有硬收益的一件, 其余都是归位。**

### 4.4 `ResourceLoader` —— 平台 + 加载链, 应迁 `dispatch`

- **语义**: 缓存 exe 目录(`GetModuleFileNameA`,`ResourceLoader.cpp:29`)、拼路径、判存在、读 PE 资源节。
- **事实**: 145 行;唯一消费者 `DispatchScene.cpp`(4 个调用点: `131` / `132` / `140` / `578`);`GetExeDir` 与
  `LoadResource` 零消费者(§3.3);`LoadResource` 走 `FindResourceA` / `LoadResource` / `LockResource` /
  `SizeofResource`(`ResourceLoader.cpp:88-99`), 是纯 PE 能力;拼接在 `MAX_PATH * 2` 栈缓冲内无边界检查
  (`lstrcpyA` / `lstrcatA`, 已登记 `EXAMPLES.md:265` RG-D2)。
- **判据**: Q1 是(exe 概念 + PE 资源节) · Q2 是(后端无关) · Q3 **否**(路径语义 = "布局文档按 exe 同目录解释",
  这正是调度层的加载链决策, 见 `README.md` 运行一节) · Q4 否(单消费者)。
- **结论**: 它是**加载链的路径解析器**, 不是公共件。文档读取其实不在它身上——真正读文件的是
  `LayoutSolver.cpp:61 / 370 / 652`(`CreateFileA` / `ReadFile`), `ResourceLoader` 只提供"路径 + 是否存在"。
  所以它迁 `dispatch` 比迁 `layout` 更准确: 它实现的是"缺省加载链"这一调度决策
  (`DispatchScene.cpp:131-140`), 而 `layout` 只接受"一份缓冲或一个路径"。
- **附注**: 若担心 `dispatch` 目录继续膨胀, 可把它与 `DispatchScene.cpp` 的加载链收在
  `dispatch\scene\` 子组; 但按 D5 目录惯例, 平铺亦可。

### 4.5 `AppEnv` —— 平台桌面栈, 应拆开随消费方走

- **语义**: 三件互不相关的能力, 只是共用了一个"环境件"的名字。

| 函数 | 实现 | 唯一消费者 | 实际归属 |
| :--- | :--- | :--- | :--- |
| `InitConsoleUtf8` | `SetConsoleOutputCP` / `SetConsoleCP`(`AppEnv.cpp:23-24`) | `Dispatch.cpp:86` | 进程诊断设施(调度层启动第一步) |
| `IsSystemDarkMode` | `RegOpenKeyExW` 读 `AppsUseLightTheme`(`AppEnv.cpp:37-42`) | `Dispatch.cpp:109 / 495` | 调度策略("亮暗默认跟随系统", 纲领十 D7) |
| `ApplyWindowCornerRounding` | `DwmSetWindowAttribute` → `CreateRoundRectRgn`(`AppEnv.cpp:56-78`) | `ImGuiHost.cpp:472 / 1249` | Win32 窗口宿主外观 |

- **判据**: Q1 是 · Q2 **是**(圆角与主题都属桌面宿主外观; 控制台属进程诊断) · Q3 否(不服务任何框架领域, 但也不通用) · Q4 否(各一至两个调用点)。
- **结论**: 它不是"公共件"也不是"框架件", 是**平台宿主能力**。而且它自带应用层痕
  (硬编码 `[MAIN]` 日志, `AppEnv.cpp:60 / 78`, 已登记 `EXAMPLES.md:234` RG-A9)。
- **迁法受分层纪律的硬约束**: 不能整件塞进 `render\imgui` —— `Dispatch` 是它的消费者之一, 而
  `CHARTER.md:425` 规定"`dispatch` 只经 `HostApi` 访问宿主, 不引用 `ImGuiHost.h`"。整件迁 `render\imgui`
  会造出 `dispatch → render\imgui` 的反向依赖。因此必须拆:
  - `ApplyWindowCornerRounding` → `render\imgui`(建议直接内联为 `ImGuiHost.cpp` 的静态函数: 只在建窗两处用,
    且实现体不足 40 行, 单独成件价值低);
  - `InitConsoleUtf8` + `IsSystemDarkMode` → `dispatch`(启动第一步与亮暗策略), 可合并为 `dispatch\HostEnv`
    或直接内联进 `Dispatch.cpp`(各 1 至 6 行)。
- **风险提示**: 内联会改动 `ImGuiHost` 与 `Dispatch` 两个"行为已锁定"的件, 需按例外条登记;
  若倾向零风险, 也可保留模块形态, 只分拆落点(`render\imgui\WndRounding` + `dispatch\PlatformEnv`)。

## 5. `common` 层本身怎么办

| 方案 | 内容 | common 剩余 | 代价 | 收益 |
| :--- | :--- | :--- | :--- | :--- |
| **A 保守重划** | 保留 common;`FileCodec` 留驻;其余四件按 §4 迁出 | 1 件(87 行) | `common` 为一棵草建花坛;目录数与层数不变 | 改动面最小;GenUI 回灌路径差异最小 |
| **B 取消 common**(推荐) | 五件全部归位: `base\mem`(两件) · `base\util`(`FileCodec`) · `dispatch`(`ResourceLoader`) · 拆分(`AppEnv`);目录只剩 `framework` + `demo` + `Identity` + `main` | 0 | 与 GenUI 侧路径分叉扩大, 回灌需按 `D3D11Manager` 先例处置 | 层级由"common + 四层"收敛为四层;`README` 的"框架内核 = framework + common"一句话简化;派生母本无歧义 |

判断依据: `README.md:100` 已写明 `common` 是"通用基础设施(GenUI 快照起点)", 而 GenUI 侧本就是扁平的;
`DERIVE.md:63-64` 也已把 common 计入"派生时整体带走"的 15 文件。
换言之, **common 的独立层身份从来只是历史遗留, 不是设计要求**。本工程已经把 `framework` 重排成四层并
按技术名分目录(纲领十), 唯一没有对齐的就是 common。

反面理由只有一条, 但很实在: `common` 是 GenUI 快照的目录名, 取消它会让"从 GenUI 重新复制 common"变成
一个需要人工映射的动作(先例 = `D3D11Manager` 归位后的三选一处置)。若本工程的定位是"派生母本优先、
回灌其次", 这条代价可接受;反之则取方案 A。

## 6. 落地批次(建议)

| 批 | 动作 | 性质 | 判定依据 |
| :--- | :--- | :--- | :--- |
| S1 | `ResourceLoader` → `dispatch`(纯路径搬迁 + include 改前缀 + `CMakeLists` 源表) | 纯搬迁 | 同 `D3D11Manager` 先例(`EXAMPLES.md:100-105`);零语义改动 |
| S2 | `AllocatorHub` → `base\mem`;删 `ProjCR::SetAllocators` 与 `AllocLayers` 内的重复策略(改调 `AllocSmall`) | 含语义改写(减少一次注入) | 依据 §4.3 的实证;需例外登记 |
| S3 | `MemoryManager` → `base\mem`;拆 `BufferInfo.h`;`ControlSpec.h` / `CtrlBuffer.h` 改引该头 | 含接口拆分 | 依据 §4.2;`ControlSpec.h` 只用到 `BufferInfo`(`:76`) |
| S4 | `AppEnv` 拆分: 圆角 → `render\imgui`;控制台与主题 → `dispatch` | 含行为件改动 | 依据 §4.5 与 `CHARTER.md:425` 的分层约束 |
| S5 | 文档同步: `README` 目录结构与两层结构表述 · `DERIVE` §5 计数 · `EXAMPLES` 例外登记 · `CHARTER` 纲领 · 五份接口文档的"要求"表与依赖行 | 文档 | 每批各自登记, 与 S1–S5 的既有做法一致 |

每批验收沿用现行口径: 两配置 0 警告 0 错误 + 无人值守运行退出码 0 + 启动日志的结构自证行
(场景载入 / 事件绑定 / 控件层次 / 控件坐标)与改前逐字符一致。S2 起须额外核对一条:
`[DEMO]` 开头的行数不变、`[MAIN]` 行消失(若 S4 同时清理 RG-A9)。

## 7. 待拍板

| 项 | 问题 | 选项 |
| :--- | :--- | :--- |
| D1 | `common` 层去留 | A 保留并缩为单件 / **B 取消, 全部归位(推荐)** |
| D2 | `MemoryManager` 是否拆出零依赖 `BufferInfo.h` | A 拆(推荐) / B 不拆(整体搬迁) |
| D3 | `AppEnv` 拆分方式 | A 拆成两件(`render\imgui\WndRounding` + `dispatch\PlatformEnv`)(推荐, 零行为风险) / B 内联进两个消费件(改动面大, 省两个文件) |
| D4 | 零消费者接口的处置 | A 只加"未使用"注记, 保留待上游(推荐) / B 就地删除 `SecureBuffer` 等 |
| D5 | 与 GenUI 的快照关系 | A Demo 先行, 登记漂移(与 `D3D11Manager` 同口径)(推荐) / B 等 GenUI 侧先改 |
| D6 | 是否顺带修正文档引用腐化 | A 是(推荐): `CHARTER.md:426` / `EXAMPLES.md:377` 引用的 `README.md` §2.1 层间依赖表与 §6 漂移处置**在当前 README 中已不存在**(现章节 = 两层结构 / 演示内容 / 构建 / 运行 / 目录结构 / 与 GenUI 的关系 / 已知限制) / B 留待下轮 |

## 8. 不做什么

- **不引入跨平台抽象层**: 不为"平台无关"造 `IPlatformMemory` 之类的接口。本工程是 Windows 专有框架,
  抽象层的成本远大于收益, 且会让 base 的公共契约多一层间接。
- **不改文档格式**: `FileCodec` 的 CRC32 覆盖口径、`BIN_VERSION` 均不动(纲领十 D8 同口径)。
- **不动快照行为**: 本轮只搬位置、拆依赖、删重复策略;除 S2 的策略合并外不产生任何运行期行为差异。
- **零消费者接口已删, 不是"只登记"**: 2026-10-23 批复"可以删的删除" ⇒ `SecureBuffer` 整类、`Arena::Mark` /
  `Rewind`、`MemPool::SlabCount`、一次算完的 `Crc32`、`GetExeDir`、`LoadResource` 一并删除(理由与清单见 §9)。
  若上游(GenUI)后续需要密钥缓冲或资源节读取, 按新版重写, 不从本工程回灌删除前的旧件。

## 9. 落地记录(2026-10-23)

用户批复原话:

> 不拆，纯搬迁，但是可以删的删除

| 项 | 处理 |
| :--- | :--- |
| D1 `common` 去留 | **B**: 取消 `src\common\`, 五件全部归位 `framework` |
| D2 `BufferInfo.h` 拆分 | **不拆**: `MemoryManager.h` 整体搬迁, `ControlSpec.h` / `CtrlBuffer.h` 仍引 `mem/MemoryManager.h` |
| D3 `AppEnv` 拆分方式 | **整件不拆**: 落 `framework\base`(平台查询件放底座; `dispatch` 与 `render` 均向下引用, 未新增分层例外) |
| D4 零消费者接口 | **B**: 删除 |
| D5 快照关系 | Demo 先行(本工程先落地, 登记漂移) |
| D6 文档引用腐化 | 修正: `README.md` §2.1 / §6 的失效引用改为指向 `CHARTER` 纲领十与本文件 |

落地清单:

**搬迁(纯路径, 接口零改动)**

| 原 | 现 |
| :--- | :--- |
| `src\common\MemoryManager.{h,cpp,md}` | `src\framework\base\mem\` |
| `src\common\AllocatorHub.{h,cpp,md}` | `src\framework\base\mem\` |
| `src\common\FileCodec.{h,cpp,md}` | `src\framework\base\util\` |
| `src\common\AppEnv.{h,cpp,md}` | `src\framework\base\` |
| `src\common\ResourceLoader.{h,cpp,md}` | `src\framework\dispatch\` |

**删除(零消费者)**: `SecureBuffer` 整类(12 个公开成员, 头 57 行 + 实现 141 行 + 文档 58 行) ·
`Arena::Mark` / `Rewind` · `MemPool::SlabCount` · `FileCodec::Crc32`(一次算完; 流式三件套保留) ·
`ResourceLoader::GetExeDir` · `ResourceLoader::LoadResource`。

**改动面**: include 前缀 17 处 · `CMakeLists.txt` 源表与分层注释 · 15 份接口文档(5 份随迁自身 + 10 份依赖表) ·
`README`(两层结构 / 目录树 / 注入脚本) · `DERIVE` §5 计数与归位处置 · `CHARTER` 纲领十四 · `EXAMPLES` 第八例例外 ·
`INTERFACE-DOC-STYLE` 体例示例路径。

**自证**

- 构建: Release / Debug 两配置 **0 警告 0 错误**(`cmake` 重新 configure + NuGet Restore + `msbuild -m`);
- 运行: `DEMO_AUTOCLOSE_MS=1500` 无人值守, 退出码 **0**;
- 日志: 14 行, 结构自证齐全(`场景载入` → `Kanat.bin` / `布局=DemoMain` / `控件=15`、`事件绑定 3 枚`、
  `控件层次`、`控件坐标`、`字号 28.0`、`建窗几何`), `[MAIN] 圆角实现: DWM` 一行仍在(行为未改)。

**构建踩坑(已修 README)**: 原注入脚本用 `[string]::Replace('</Project>', …)`, 会连带改坏 vcxproj 里
`ProjectReference` 的 `<Project>` 元数据(该标签同样以 `</Project>` 结尾), 表现为 MSB3107。现行 README 改为
`LastIndexOf('</Project>')` 定位文件末尾再插入。
