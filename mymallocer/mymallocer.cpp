#include "mymallocer.h"
#include <assert.h>
#include <algorithm>

/************************************************/
//	内存结构为
//
//      对齐块 头守卫块     普通块1                普通块N   尾守卫块               
// 		|    |         |              |       |              |    |
//		|----|----|----|----|####|----|.......|----|####|----|----|
//	    ^    	            ^                      ^
//	双字对齐             双字对齐               双字对齐
//	
//	其中头尾守卫块用于处理代码中的边界情况
//
/************************************************/
MyMallocer::MyMallocer()
{
	unsigned int* pheap = wptr(heap.sbrk(WSIZE * 4));
	if (!pheap)
		throw std::bad_alloc();
	*pheap = 0x0;	//由于heap头是确保双字对齐的，所以在此处加一个空位，保证之后的每一个块内容都是双字对齐的
	set(pheap + WSIZE, DSIZE, 1); 
	set(pheap + WSIZE * 2, DSIZE, 1);
	set(pheap + WSIZE * 3, 0, 1);
	pheap += WSIZE * 2;
	if (!extendHeap(MIN_EXTEND))
		throw std::bad_alloc();
}

void* MyMallocer::malloc(size_t size)
{
	if (size <= 0)
		return nullptr;
	size = ((size - 1) / DSIZE + 2) * DSIZE; //加上首尾守卫大小，并向上对齐到双字

	void* blkptr = head;
	size_t tailSpace = 0;
	while (true) {
		void* head = getHead(blkptr);
		if (getAlloced(head)) {
			if (getSize(head) == 0) // meet tail
				break;
			else {
				tailSpace = 0;
			}
		}
		else {
			if (getSize(head) >= size) { // find sufficient space
				return occupy(blkptr, size);
			}
			else {
				tailSpace = getSize(head);
			}
		}
		blkptr = nxtBlk(blkptr);
	}
	assert(size > tailSpace);
	size_t spaceToExtend = std::max(size_t(size - tailSpace), MIN_EXTEND);
	blkptr = extendHeap(spaceToExtend);
	if (!blkptr)
		throw std::bad_alloc();
	return occupy(blkptr, size);
}

void MyMallocer::free(void* ptr)
{
	size_t size = getSize(getHead(ptr));
	set(getHead(ptr), size, 0);
	set(getTail(ptr), size, 0);
	merge(ptr);
}

void* MyMallocer::extendHeap(size_t size)
{
	if (size <= 0)
		return nullptr;
	//确保双字对齐
	size = (((size - 1) / DSIZE) + 1) * DSIZE;
	void* ptr = heap.sbrk(size);
	if (!ptr)
		return nullptr;
	set(getHead(ptr), size, 0);
	set(getTail(ptr), size, 0);
	set(getHead(nxtBlk(ptr)), 0, 1);

	return merge(ptr);
}

void* MyMallocer::merge(void* blkptr)
{
	bool nxtAlloced = getAlloced(getTail(pevBlk(blkptr)));
	bool pevAlloced	= getAlloced(getHead(nxtBlk(blkptr)));
	size_t size = getSize(getHead(blkptr));
	void* ret;

	if (!nxtAlloced && !pevAlloced) {
		size = size + getSize(getTail(pevBlk(blkptr))) + getSize(getHead(nxtBlk(blkptr)));
		ret = pevBlk(blkptr);
		set(getHead(ret), size, 0);
		set(getTail(nxtBlk(blkptr)), size, 0);
	}
	else if(!nxtAlloced)
	{
		size = size + getSize(getHead(nxtBlk(blkptr)));
		ret = blkptr;
		set(getHead(ret), size, 0);
		set(getTail(ret), size, 0);
	}
	else if(!pevAlloced)
	{
		size = size + getSize(getTail(pevBlk(blkptr)));
		ret = pevBlk(blkptr);
		set(getHead(ret), size, 0);
		set(getTail(blkptr), size, 0);
	}
	else
	{
		ret = blkptr;
	}
	return ret;
}

void* MyMallocer::occupy(void* blkptr, size_t size)
{
	size_t blksize = getSize(getHead(blkptr));
	// 最小块大小是2*DSIZE, 超过这个大小产生外部碎片，小于这个大小则产生内部碎片，防止块数量过多导致查找性能下降
	if ((blksize - size) < 2*DSIZE) {
		set(getHead(blkptr), size, 1);
		set(getTail(blkptr), size, 1);
		return blkptr;
	}
	else {
		set(getHead(blkptr), size, 1);
		set(getTail(blkptr), size, 1);
		void* nxtblk = nxtBlk(blkptr);
		set(getHead(nxtblk), blksize - size, 0);
		set(getTail(nxtblk), blksize - size, 0);
		return blkptr;
	}
}

