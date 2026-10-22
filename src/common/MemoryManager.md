# MemoryManager 模块 (src\common\MemoryManager.h)

## 概述

统一内存管理模块, 含三个按用途分工的分配器与一个对齐辅助:

| 类 | 定位 | 释放模型 |
| --- | --- | --- |
| `SecureBuffer` | 敏感数据专用缓冲区 | 整体析构, 安全清零 |
| `Arena` | 线性分配器 | 整体 `Reset` 或 `Mark` / `Rewind` |
| `MemPool` | 分级固定块内存池 | 任意顺序单独 `Free` |

三个类的共同契约: 底层一律 `VirtualAlloc` / `VirtualFree`, 模块内不使用 `new` / `delete`;
分配统一返回 `BufferInfo`, 其中 `size` 是**请求大小**而非对齐后的实际占用, 判空一律看 `buffer`;
分配失败统一返回 `{ nullptr, 0 }`(含底层提交失败与请求为 0 两种情形); 不提供隐式线程安全;
禁止拷贝构造与赋值。

本模块属 **common 层**, 不依赖工程内任何其它模块。

本模块包含以下编程接口:

### 类型

| 类型 | 说明 |
| --- | --- |
| `BufferInfo` | 分配结果: 内存首址 + 请求大小。 |
| `SecureBuffer` | 敏感数据缓冲区: 页级保护 + 释放前安全清零。 |
| `Arena` | 线性分配器: 指针推进式分配, 整体或按标记回退。 |
| `MemPool` | 分级固定块内存池: 小对象任意顺序分配与回收。 |

### 函数

| 函数 | 说明 |
| --- | --- |
| `AlignUp` | 把大小向上对齐到指定对齐值。 |

---

## AlignUp 函数 (MemoryManager)

把大小向上对齐到指定对齐值。

### 语法

```cpp
inline size_t AlignUp(size_t size, size_t align);
```

### 参数

`[in] size`

类型: **size_t**

原始大小(字节)。

`[in] align`

类型: **size_t**

对齐值(字节), 取 2 的幂。

### 返回值

类型: **size_t**

不小于 `size` 的最小对齐倍数。`size` 本身已对齐时原样返回。

### 注解

`align` 为 0 时行为无定义(除零性质), 调用方须传 2 的幂。三个分配器内部一律用本函数记账与换算,
调用方可复用同一口径。

---

## BufferInfo 结构 (MemoryManager)

分配结果描述。

### 语法

```cpp
struct BufferInfo
{
    VOID*  buffer;
    size_t size;
};
```

### 成员

| 成员 | 说明 |
| --- | --- |
| `buffer` | 内存首址; 分配失败为 `nullptr`。 |
| `size` | 请求大小; 分配失败为 `0`。不是对齐后的实际占用。 |

### 注解

判空只看 `buffer`。`size` 只用于回读请求量, 不参与 `MemPool::Free` 的回收判定,
可传 `{ buffer, 0 }`。

---

## SecureBuffer 类 (MemoryManager)

敏感数据专用缓冲区: 页级保护切换 + 释放前安全清零。

### 语法

```cpp
class SecureBuffer
{
public:
    SecureBuffer(size_t requestedSize);
    ~SecureBuffer();

    BufferInfo Allocate(size_t size);
    BOOL Contains(LPCVOID p) const;
    size_t Capacity() const;
    size_t Used() const;

    VOID SecureZero(size_t offset, size_t size);
    VOID SecureZeroAll();
    BOOL Protect(DWORD protectFlag);
    BOOL SetReadOnly();
    BOOL SetReadWrite();
    BOOL SetNoAccess();
};
```

### 成员

| 成员 | 说明 |
| --- | --- |
| `SecureBuffer(size_t)` | 按请求容量提交一块页对齐连续内存; 提交失败时容量归零。 |
| `~SecureBuffer()` | 先 `SecureZeroAll` 清零, 再 `VirtualFree` 释放。 |
| `Allocate(size_t)` | 线性分配一块; 超出容量或请求为 0 时返回空信息。 |
| `Contains(LPCVOID)` | 地址归属查询: 指针是否落在本缓冲区内(不解引用)。 |
| `Capacity()` | 容量(页对齐后的字节数)。 |
| `Used()` | 已用字节数(按对齐后占用累加)。 |
| `SecureZero(offset, size)` | 把指定区间安全清零; 越界时直接返回。 |
| `SecureZeroAll()` | 把整块缓冲区安全清零。 |
| `Protect(DWORD)` | 切换整块缓冲区的页保护; 成功时记录当前保护标志。 |
| `SetReadOnly()` | 切换为只读(`PAGE_READONLY`)。 |
| `SetReadWrite()` | 切换为读写(`PAGE_READWRITE`)。 |
| `SetNoAccess()` | 切换为不可访问(`PAGE_NOACCESS`)。 |

### 注解

内部分配是线性的: 每次 `Allocate` 只把已用偏移前推, 不支持单独释放。对齐粒度为
`sizeof(void*)`(x86 4 字节 / x64 8 字节), 不做 16 字节对齐; 容量与占用按对齐后大小记账。

安全清零经 `RtlSecureZeroMemory`, 不会被编译器优化掉。页保护切换是页级系统调用, 适合
"使用期间可读、闲置期间只读或不可访问"的节奏, 不适合高频切换。

适用: 密钥、口令、令牌、会话 ID 等敏感数据; 生命周期明确、整体释放的场景。
不适用: 普通业务数据与临时字符串(页对齐与清零的开销不值得)、需要单独释放、频繁小块分配、
需要 SSE / AVX 级对齐、多线程高频访问同一块。

禁止拷贝; 跨线程使用须外部加锁。

---

## Arena 类 (MemoryManager)

线性分配器: 指针推进式分配, 整体或按标记回退。

### 语法

```cpp
class Arena
{
public:
    Arena(size_t requestedSize);
    ~Arena();

    BufferInfo Allocate(size_t size);
    BOOL Contains(LPCVOID p) const;
    size_t Capacity() const;
    size_t Used() const;

    VOID Reset();
    size_t Mark() const;
    VOID Rewind(size_t mark);
};
```

### 成员

| 成员 | 说明 |
| --- | --- |
| `Arena(size_t)` | 按请求容量提交一块页对齐连续内存; 提交失败时容量归零。 |
| `~Arena()` | `VirtualFree` 释放整块内存。 |
| `Allocate(size_t)` | 线性分配一块; 超出容量或请求为 0 时返回空信息。 |
| `Contains(LPCVOID)` | 地址归属查询: 指针是否落在本分配器内(不解引用)。 |
| `Capacity()` | 容量(页对齐后的字节数)。 |
| `Used()` | 已用字节数(按 16 字节对齐后占用累加)。 |
| `Reset()` | 已用偏移归零, 此前分配的内存全部失效。 |
| `Mark()` | 记录当前已用偏移(回退标记)。 |
| `Rewind(size_t)` | 回退到给定标记; 标记大于当前已用偏移时不动。 |

### 注解

分配对齐粒度为 16 字节, 不初始化、不清零。不支持单独释放: 回收只有两种方式——整体 `Reset`,
或 `Mark` / `Rewind` 按 LIFO 顺序回退。

`Rewind` 必须按后进先出使用: 回退到某标记后, 该标记之后分配的所有对象失效; 不得回退到更早标记
再继续推进, 否则后续分配会与仍被引用的块重叠。

适用: 同生共死的一批对象、解析中间结果、帧内临时缓冲、反复重建整代的场景。
不适用: 生命周期不一致、需要单独释放、大小波动大且申请释放交替频繁、需要内存保护或安全清零。

禁止拷贝。

---

## MemPool 类 (MemoryManager)

分级固定块内存池: 小对象任意顺序分配与回收。

### 语法

```cpp
class MemPool
{
public:
    MemPool();
    ~MemPool();

    BufferInfo Allocate(size_t size);
    VOID Free(BufferInfo info);
    VOID Free(VOID* ptr);
    BOOL Contains(LPCVOID p) const;
    size_t SlabCount() const;

    static constexpr size_t MAX_SMALL_SIZE = 4096;
};
```

### 成员

| 成员 | 说明 |
| --- | --- |
| `MemPool()` | 初始化 28 个尺寸类的空闲链表; 不预分配 slab。 |
| `~MemPool()` | 释放全部 slab。 |
| `Allocate(size_t)` | 按尺寸类取一块; 请求为 0 或超过 `MAX_SMALL_SIZE` 时返回空信息。 |
| `Free(BufferInfo)` | 回收一块, 等价于 `Free(info.buffer)`。 |
| `Free(VOID*)` | 回收一块; 非本池指针静默忽略。 |
| `Contains(LPCVOID)` | 地址归属查询: 指针是否落在本池任一 slab 的用户数据区。 |
| `SlabCount()` | 已分配 slab 数(遍历链表, O(n))。 |
| `MAX_SMALL_SIZE` | 用户可请求的最大尺寸: 4096 字节。 |

### 注解

**结构**: 28 个尺寸类(块总大小含 16 字节块头):
`32, 48, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 640, 768, 896, 1024,
1280, 1536, 1792, 2048, 2560, 3072, 3584, 4096, 4112`。每个 slab 64 KB, 由 `VirtualAlloc` 提交,
Slab 头内嵌在 slab 开头; 数据区起始偏移按 16 字节对齐, 块大小均为 16 的倍数 ⇒ 用户指针保持
16 字节对齐。块头 `BlockHeader`(16 字节)记录所属尺寸类, 空闲块以链表串联 ⇒ 分配 / 回收 O(1)。

末档 `4112 = 16 + 4096` 是刻意的: 查找按"块头 + 请求"匹配尺寸类, 若末档仍是 4096,
则用户请求 4081..4096 无档可服务。

**归属校验**: `Free` 先判指针是否落在本池某个 slab 的数据区, 再校验块头的 `sizeClass` 是否指向本池
的尺寸类数组; 任一不满足即忽略。因此跨池误释放、外部指针、线性分配器指针都不会破坏池状态。
但**不要依赖它防错**: 同一块重复 `Free` 不会被拦住(会被挂进空闲链表两次), 调用方仍须保证语义正确。

**适用**: 不定生命周期的小对象(节点、事件、上下文), 生命周期互相独立、需要单独释放、
单对象不超过 `MAX_SMALL_SIZE`、频繁创建销毁的场景。
**不适用**: 单对象超过上限(返回空, 需上层改用其它分配器)、一次性批量分配且从不回收
(纯指针推进的线性分配器更省)、超大或变长缓冲、需要内存保护或安全清零、需要移动已分配块。

禁止拷贝; 内部无锁。


---

## 要求

| 要求 | 值 |
| --- | --- |
| 标头 | `common/MemoryManager.h`(`src\common\MemoryManager.h`) |
| 实现 | `src\common\MemoryManager.cpp`;`AlignUp` 与类内联成员在头文件内 |
| 命名空间 | 全局作用域 |
| 依赖 | `minwindef.h` · `kernel32.dll` · `RtlSecureZeroMemory` |
| 平台 | Windows 全版本 |
| 线程安全 | 否(无内部同步) |
