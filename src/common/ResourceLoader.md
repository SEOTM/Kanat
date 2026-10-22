# ResourceLoader 模块 (src\common\ResourceLoader.h)

## 概述

资源与路径加载原语:缓存 exe 所在目录、按 exe 目录拼接文件路径、判断文件是否存在、读取随 exe
内置的资源节。目录与路径字符串一律落在会话分配器(`AllocatorHub`)的线性分配器里, 本模块不额外
持有堆内存。

本模块属 **common 层**, 依赖 `AllocatorHub` 与 Win32; 不解析文件内容, 不做文档格式分派。

本模块包含以下编程接口:

### 函数

| 函数 | 说明 |
| --- | --- |
| `Init` | 缓存 exe 所在目录(含末尾反斜杠); 幂等。 |
| `GetExeDir` | 取缓存的 exe 目录; 未初始化时返回空串。 |
| `GetFilePath` | 把 exe 目录与文件名拼成完整路径(每次新分配)。 |
| `FileExists` | 路径是否存在(加载链的"外部文件"判据)。 |
| `LoadResource` | 读取随 exe 内置的资源节字节, 拷进会话分配器并补结尾 `\0`。 |

本模块不定义公开类型与公开常量。

---

## Init 函数 (ResourceLoader)

缓存 exe 所在目录。

### 语法

```cpp
VOID Init();
```

### 参数

此函数无参数。

### 返回值

类型: **VOID**

此函数不返回值。

### 注解

幂等: 已缓存时直接返回。取 `GetModuleFileNameA(NULL)` 的目录部分(含末尾 `\`), 经
`AllocatorHub::CopyString` 存入会话分配器。取路径失败或长度异常时目录保持未建立状态,
此时 `GetFilePath` 返回 `NULL`、`GetExeDir` 返回空串。

必须在会话分配器建立之后调用(`AllocatorHub::Init` 先行), 否则字符串拷贝失败, 目录同样保持未建立。

---

## GetExeDir 函数 (ResourceLoader)

取缓存的 exe 目录。

### 语法

```cpp
LPCSTR GetExeDir();
```

### 参数

此函数无参数。

### 返回值

类型: **LPCSTR**

exe 目录(含末尾 `\`); **未初始化或初始化失败时返回空串 `""`, 不是 `NULL`**。
返回指针由会话分配器持有, 调用方不得释放。

### 注解

因失败返回空串而非空指针, 判空应看首字符是否为 `\0`。返回的指针在会话分配器释放后失效
(不单独失效, 随整体释放)。

---

## GetFilePath 函数 (ResourceLoader)

把 exe 目录与文件名拼成完整路径。

### 语法

```cpp
LPCSTR GetFilePath(LPCSTR filename);
```

### 参数

`[in] filename`

类型: **LPCSTR**

文件名(相对 exe 目录, 不含目录分隔符)。为 `NULL` 时返回 `NULL`。

### 返回值

类型: **LPCSTR**

完整路径(会话分配器内新串); 目录未就绪、`filename` 为 `NULL` 或分配失败时返回 `NULL`。

### 注解

每次调用都新分配, 不复用、不驻留; 返回指针随会话分配器整体失效。

拼接在 `MAX_PATH * 2` 的栈缓冲内完成, 目录与文件名的组合长度须小于该值, 否则结果被截断。

---

## FileExists 函数 (ResourceLoader)

判断路径是否存在。

### 语法

```cpp
BOOL FileExists(LPCSTR path);
```

### 参数

`[in] path`

类型: **LPCSTR**

待判断的完整路径。为 `NULL` 时返回 `FALSE`。

### 返回值

类型: **BOOL**

`TRUE` = 路径存在; `FALSE` = 不存在或 `path` 为 `NULL`。

### 注解

实现为 `GetFileAttributesA != INVALID_FILE_ATTRIBUTES`, 因此**目录也会返回 `TRUE`**;
需要区分文件与目录时由调用方自行检查属性。本函数只回答"存在性", 不判断可读性。

---

## LoadResource 函数 (ResourceLoader)

读取随 exe 内置的资源节字节。

### 语法

```cpp
BOOL LoadResource(
  [in]  DWORD  dwResId,
  [in]  LPCSTR szResType,
  [out] LPVOID* ppData,
  [out] DWORD* pcbData
);
```

### 参数

`[in] dwResId`

类型: **DWORD**

资源 ID。资源 ID 与类型串由资源脚本约定, 本模块不内置任何具体 ID。

`[in] szResType`

类型: **LPCSTR**

资源类型串(如 `"JSON"` / `"BIN"`)。为 `NULL` 时返回 `FALSE`。

`[out] ppData`

类型: **LPVOID\***

接收数据首址(资源节字节在会话分配器内的副本)。失败时置 `NULL`。

`[out] pcbData`

类型: **DWORD\***

接收字节数,**不含**本函数补的结尾 `\0`。失败时置 `0`。

### 返回值

类型: **BOOL**

`TRUE` = 命中且两个出参有效; `FALSE` = 会话分配器未就绪、出参为空、资源未找到、加载 / 锁定失败、
资源长度为 0 或分配失败。失败时两个出参均被置空。

### 注解

资源节字节不保证以 `\0` 结尾, 而按字符串消费的调用方(如 JSON 解析)需要 NUL ⇒ 统一在副本末尾
补一位 `\0`; 补的字节不计入 `pcbData`。按显式长度消费的调用方(bin 解析)不受影响。

读取顺序为 `FindResourceA` → `LoadResource` → `LockResource` → `SizeofResource`, 数据从模块资源节
复制到会话分配器, 返回后不依赖资源句柄。返回的指针随会话分配器整体失效。


---

## 要求

| 要求 | 值 |
| --- | --- |
| 标头 | `common/ResourceLoader.h`(`src\common\ResourceLoader.h`) |
| 实现 | `src\common\ResourceLoader.cpp` |
| 命名空间 | `ResourceLoader` |
| 依赖 | `kernel32.dll` · `AllocatorHub` |
| 平台 | Windows 全版本 |
| 线程安全 | 否(写会话分配器); 须由调用方串行 |
