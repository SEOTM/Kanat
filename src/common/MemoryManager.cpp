#include "common/MemoryManager.h"
#include <sysinfoapi.h>
#include <memoryapi.h>

// ================================
// 模块: MemoryManager 实现(接口契约见同目录 MemoryManager.md)
// ================================

// ================================
// 类名: SecureBuffer
// ================================

// 函数: SecureBuffer
// 描述: 构造安全缓冲区(容量向页对齐; 提交失败时容量归零)
SecureBuffer::SecureBuffer(size_t requestedSize)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	size_t pageSize = sysInfo.dwPageSize;
	capacity = AlignUp(requestedSize, pageSize);
	used = 0;
	buffer = (BYTE*)VirtualAlloc(nullptr, capacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	flagProtect = PAGE_READWRITE;
	// 计算默认分配对齐字节(这里使用系统指针大小作为对齐值)
	alignBytes = sizeof(void*);	// 一般为 4 或 8 字节
	// 底层分配失败时容量归零, 后续 Allocate 返回 { nullptr, 0 }
	if (buffer == nullptr)
	{
		capacity = 0;
	}
}

// 函数: ~SecureBuffer
// 描述: 释放缓冲区(释放前先整块清零)
SecureBuffer::~SecureBuffer()
{
	// 释放缓冲区内存
	if (buffer)
	{
		// 在释放前清零缓冲区
		SecureZeroAll();
		VirtualFree(buffer, 0, MEM_RELEASE);
		buffer = nullptr;
	}
}

// 函数: Allocate
// 描述: 从安全缓冲区分配一段内存(按指针大小对齐)
BufferInfo SecureBuffer::Allocate(size_t size)
{
	BufferInfo info = { nullptr, 0 };
	if (buffer == nullptr || size == 0)
	{
		return info;	// 底层未就绪或零大小请求, 返回空信息
	}
	size_t alignedSize = AlignUp(size, alignBytes);
	if (used + alignedSize > capacity)
	{
		return info;	// 超出容量, 返回空信息
	}
	// 返回分配信息(包括指针和大小, 但是大小为请求的大小)
	info.buffer = buffer + used;
	info.size = size;
	// used 按对齐后的占用累加
	used += alignedSize;
	return info;
}

// 函数: Contains
// 描述: 判断指针是否落在本缓冲区范围内
BOOL SecureBuffer::Contains(LPCVOID p) const
{
	if (buffer == nullptr || p == nullptr)
	{
		return FALSE;
	}
	CONST BYTE* b = (CONST BYTE*)p;
	return (b >= buffer) && (b < buffer + capacity) ? TRUE : FALSE;
}

// 函数: Capacity
// 描述: 取缓冲区容量
size_t SecureBuffer::Capacity() const
{
	return capacity;
}

// 函数: Used
// 描述: 取已使用的字节数
size_t SecureBuffer::Used() const
{
	return used;
}

// 函数: SecureZero
// 描述: 把区间内字节安全清零(越界或未就绪直接返回)
VOID SecureBuffer::SecureZero(size_t offset, size_t size)
{
	if (buffer == nullptr || offset + size > capacity)
	{
		return;	// 底层未就绪或超出容量, 直接返回
	}
	RtlSecureZeroMemory(buffer + offset, size);
}

// 函数: SecureZeroAll
// 描述: 把整个缓冲区安全清零
VOID SecureBuffer::SecureZeroAll()
{
	SecureZero(0, capacity);
}

// 函数: Protect
// 描述: 修改缓冲区的内存保护属性
BOOL SecureBuffer::Protect(DWORD protectFlag)
{
	if (!buffer)
	{
		return FALSE;	// 缓冲区未分配, 返回 FALSE
	}
	DWORD oldProtect;
	BOOL result = VirtualProtect(buffer, capacity, protectFlag, &oldProtect);
	if (result)
	{
		flagProtect = protectFlag;
	}
	return result;
}

// 函数: SetReadOnly
// 描述: 把缓冲区设为只读
BOOL SecureBuffer::SetReadOnly()
{
	return Protect(PAGE_READONLY);
}

// 函数: SetReadWrite
// 描述: 把缓冲区设为可读写
BOOL SecureBuffer::SetReadWrite()
{
	return Protect(PAGE_READWRITE);
}

// 函数: SetNoAccess
// 描述: 把缓冲区设为不可访问
BOOL SecureBuffer::SetNoAccess()
{
	return Protect(PAGE_NOACCESS);
}

// ================================
// 类名: Arena
// ================================

// 函数: Arena
// 描述: 构造线性分配器(容量向页对齐; 提交失败时容量归零)
Arena::Arena(size_t requestedSize)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	size_t pageSize = sysInfo.dwPageSize;
	capacity = AlignUp(requestedSize, pageSize);
	used = 0;
	buffer = (BYTE*)VirtualAlloc(nullptr, capacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	alignBytes = 16;
	// 底层分配失败时容量归零, 后续 Allocate 返回 { nullptr, 0 }
	if (buffer == nullptr)
	{
		capacity = 0;
	}
}

// 函数: ~Arena
// 描述: 释放线性分配器的底层内存
Arena::~Arena()
{
	if (buffer)
	{
		VirtualFree(buffer, 0, MEM_RELEASE);
		buffer = nullptr;
	}
}

// 函数: Allocate
// 描述: 从线性分配器分配一段内存(16 字节对齐)
BufferInfo Arena::Allocate(size_t size)
{
	BufferInfo info = { nullptr, 0 };
	if (buffer == nullptr || size == 0)
	{
		return info;	// 底层未就绪或零大小请求, 返回空信息
	}
	// used 按对齐后的占用累加
	size_t alignedSize = AlignUp(size, alignBytes);
	if (used + alignedSize > capacity)
	{
		return info;	// 超出容量, 返回空信息
	}
	info.buffer = buffer + used;
	info.size = size;
	used += alignedSize;
	return info;
}

// 函数: Contains
// 描述: 判断指针是否落在本分配器范围内
BOOL Arena::Contains(LPCVOID p) const
{
	if (buffer == nullptr || p == nullptr)
	{
		return FALSE;
	}
	CONST BYTE* b = (CONST BYTE*)p;
	return (b >= buffer) && (b < buffer + capacity) ? TRUE : FALSE;
}

// 函数: Capacity
// 描述: 取分配器容量
size_t Arena::Capacity() const
{
	return capacity;
}

// 函数: Used
// 描述: 取已使用的字节数
size_t Arena::Used() const
{
	return used;
}

// 函数: Reset
// 描述: 把游标复位到起点
VOID Arena::Reset()
{
	used = 0;
}

// 函数: Mark
// 描述: 取当前游标位置作为标记
size_t Arena::Mark() const
{
	return used;
}

// 函数: Rewind
// 描述: 把游标回退到标记位置
VOID Arena::Rewind(size_t mark)
{
	if (mark <= used)
	{
		used = mark;
	}
}

// ================================
// 类名: MemPool
// ================================

// 尺寸类表(块总大小, 升序)
const size_t MemPool::SIZE_CLASSES[MemPool::NUM_SIZE_CLASSES] = {
	32, 48, 64, 80, 96, 112, 128,
	160, 192, 224, 256, 320, 384, 448, 512,
	640, 768, 896, 1024, 1280, 1536, 1792, 2048,
	2560, 3072, 3584, 4096, 4112
};

// 函数: MemPool
// 描述: 构造小对象池(尺寸类就绪; 尚无 slab)
MemPool::MemPool()
	: alignBytes(16)
	, slabList(nullptr)
{
	InitSizeClasses();
}

// 函数: ~MemPool
// 描述: 释放全部 slab
MemPool::~MemPool()
{
	FreeSlabList();
}

// 函数: InitSizeClasses
// 描述: 初始化尺寸类数组
VOID MemPool::InitSizeClasses()
{
	for (size_t i = 0; i < NUM_SIZE_CLASSES; ++i)
	{
		sizeClasses[i].blockSize = SIZE_CLASSES[i];
		sizeClasses[i].freeList = nullptr;
	}
}

// 函数: FindSizeClass
// 描述: 依请求大小找到能容纳它的最小尺寸类
MemPool::SizeClass* MemPool::FindSizeClass(size_t size)
{
	size_t need = sizeof(BlockHeader) + size;
	for (size_t i = 0; i < NUM_SIZE_CLASSES; ++i)
	{
		if (sizeClasses[i].blockSize >= need)
		{
			return &sizeClasses[i];
		}
	}
	return nullptr;
}

// 函数: Allocate
// 描述: 从池中分配一段内存(请求不超过 MAX_SMALL_SIZE)
BufferInfo MemPool::Allocate(size_t size)
{
	BufferInfo info = { nullptr, 0 };

	if (size == 0)
	{
		return info;
	}
	if (size > MAX_SMALL_SIZE)
	{
		// 超过最大小对象尺寸, 返回空信息
		return info;
	}

	SizeClass* sc = FindSizeClass(size);
	if (!sc)
	{
		return info;
	}

	if (sc->freeList == nullptr)
	{
		// 空闲链表为空, 申请新的 slab
		if (!AllocateFromSlab(sc))
		{
			return info;
		}
	}

	// 从空闲链表取一个块
	FreeBlock* block = sc->freeList;
	sc->freeList = block->next;

	// 设置 BlockHeader
	BlockHeader* header = (BlockHeader*)block;
	header->sizeClass = sc;
	header->padding = 0;

	// 返回分配信息(跳过 BlockHeader, size 为请求的大小)
	info.buffer = (BYTE*)block + sizeof(BlockHeader);
	info.size = size;
	return info;
}

// 函数: AllocateFromSlab
// 描述: 申请新的 slab, 切分成块并挂入该尺寸类的空闲链表
VOID* MemPool::AllocateFromSlab(SizeClass* sc)
{
	VOID* mem = VirtualAlloc(nullptr, SLAB_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!mem)
	{
		return nullptr;
	}

	// 把 Slab 头直接放在 slab 内存开头, 用链表串起来
	Slab* slab = (Slab*)mem;
	slab->next = slabList;
	slabList = slab;

	// 从 Slab 头之后开始切分块(起始偏移按 16 对齐 ⇒ 用户指针保持 16 字节对齐)
	size_t startOffset = AlignUp(sizeof(Slab), alignBytes);
	size_t available = SLAB_SIZE - startOffset;
	size_t blockSize = sc->blockSize;
	size_t numBlocks = available / blockSize;

	BYTE* ptr = (BYTE*)mem + startOffset;
	for (size_t i = 0; i < numBlocks; ++i)
	{
		FreeBlock* block = (FreeBlock*)ptr;
		block->next = sc->freeList;
		sc->freeList = block;
		ptr += blockSize;
	}
	return mem;
}

// 函数: IsOwnedPointer
// 描述: 检查指针是否落在本池某个 slab 的用户数据区范围内
BOOL MemPool::IsOwnedPointer(VOID* ptr) const
{
	BYTE* p = (BYTE*)ptr;
	size_t startOffset = AlignUp(sizeof(Slab), alignBytes);

	for (Slab* s = slabList; s; s = s->next)
	{
		BYTE* begin = (BYTE*)s + startOffset;	// 跳过 Slab 头及对齐填充
		BYTE* end = (BYTE*)s + SLAB_SIZE;
		if (p >= begin && p < end)
		{
			return TRUE;
		}
	}
	return FALSE;
}

// 函数: Contains
// 描述: 判断指针是否属于本池
BOOL MemPool::Contains(LPCVOID p) const
{
	if (p == nullptr)
	{
		return FALSE;
	}
	return IsOwnedPointer((VOID*)p);
}

// 函数: SlabCount
// 描述: 取当前 slab 数量
size_t MemPool::SlabCount() const
{
	size_t count = 0;
	for (Slab* s = slabList; s; s = s->next)
	{
		count++;
	}
	return count;
}

// 函数: Free
// 描述: 回收一次 Allocate 的产物
VOID MemPool::Free(BufferInfo info)
{
	Free(info.buffer);
}

// 函数: Free
// 描述: 按指针回收本池分配的块
VOID MemPool::Free(VOID* ptr)
{
	if (!ptr)
	{
		return;
	}

	// 校验: 指针是否落在本池的某个 slab 范围内
	if (!IsOwnedPointer(ptr))
	{
		return;	// 非本池分配的指针, 直接忽略
	}

	// 获取 BlockHeader
	BlockHeader* header = (BlockHeader*)((BYTE*)ptr - sizeof(BlockHeader));
	SizeClass* sc = header->sizeClass;

	// 二次校验: sizeClass 是否指向本池的 sizeClasses 数组
	if (sc < sizeClasses || sc >= sizeClasses + NUM_SIZE_CLASSES)
	{
		return;	// 数据损坏, 拒绝释放
	}

	// 将块放回空闲链表
	FreeBlock* block = (FreeBlock*)header;
	block->next = sc->freeList;
	sc->freeList = block;
}

// 函数: FreeSlabList
// 描述: 释放全部 slab
VOID MemPool::FreeSlabList()
{
	Slab* current = slabList;
	while (current)
	{
		Slab* next = current->next;
		VirtualFree(current, 0, MEM_RELEASE);
		current = next;
	}
	slabList = nullptr;
}
