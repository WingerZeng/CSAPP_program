#pragma once
#include "virtualheap.h"
#include <exception>

//CSAPP P603 利用隐式空闲链表管理内存分配
class MyMallocer {
public:
	MyMallocer();

	//O(n) n = 块数量
	void *malloc(size_t size);

	//O(1)
	void free(void* ptr);

private:
	//Block Operation
	void* extendHeap(size_t size);

	void* merge(void* blkptr);

	void* occupy(void* blkptr, size_t size);

	//Size definition
	constexpr static size_t WSIZE = 4;
	constexpr static size_t DSIZE = 8;
	constexpr static size_t MIN_EXTEND = 1<<12; //4k 初始空闲大小，和扩展堆时的单位大小

	//Basic pointer operation
	template<typename T>
	inline unsigned int* wptr(T* vptr);

	template<typename T>
	inline size_t getSize(T* blkptr);

	template<typename T>
	inline bool getAlloced(T* blkptr);

	template<typename T>
	inline void* nxtBlk(T* blkptr);

	template<typename T>
	inline void* pevBlk(T* blkptr);

	template<typename T>
	inline void* getHead(T* blkptr);

	template<typename T>
	inline void* getTail(T* blkptr);

	template<typename T>
	inline void set(T* ptr, size_t size, int alloced);

	void* head;

	VirtualHeap heap;
};

// Templates
template<typename T>
inline unsigned int* MyMallocer::wptr(T* ptr)
{
	return (unsigned int*)ptr;
}

template<typename T>
inline size_t MyMallocer::getSize(T* ptr)
{
	return (*wptr(ptr)) & (~0x7);
}

template<typename T>
inline bool MyMallocer::getAlloced(T* ptr)
{
	return (*wptr(ptr)) & (0x1);
}

template<typename T>
inline void* MyMallocer::nxtBlk(T* blkptr)
{
	return (void*)(wptr(blkptr) + getSize(wptr(blkptr) - WSIZE));
}

template<typename T>
inline void* MyMallocer::pevBlk(T* blkptr)
{
	return (void*)(wptr(blkptr) - getSize(wptr(blkptr) - DSIZE));
}

template<typename T>
void* MyMallocer::getHead(T* blkptr)
{
	return (void*)(wptr(blkptr) - WSIZE);
}

template<typename T>
void* MyMallocer::getTail(T* blkptr)
{
	return (void*)(wptr(blkptr) + getSize(blkptr) - DSIZE);
}

template<typename T>
inline void MyMallocer::set(T* ptr, size_t size, int alloced)
{
	*wptr(ptr) = unsigned int(size) | alloced;
}