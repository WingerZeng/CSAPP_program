#pragma once
#include "virtualheap.h"
#include <stdio.h>
#include <malloc.h>

VirtualHeap::VirtualHeap(size_t MaxSize /*= (1 << 31)*/)
{
	head = (char*)malloc(MaxSize);
	brk = head;
	tail = head + MaxSize;
}

void* VirtualHeap::sbrk(int64_t offset)
{
	char* old_brk = brk;
	if (offset < 0 || offset + brk >= tail) {
		printf("Invalid brk offset!\n");
		return NULL;
	}
	brk += offset;
	return (void*)old_brk;
}
