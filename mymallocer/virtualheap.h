#pragma once
#include <stdint.h>

//����ѻ���
class VirtualHeap {
public:
	VirtualHeap(size_t MaxSize = (size_t(1) << 31));

	void* sbrk(int64_t offset);
private:

	char* head;
	char* brk;
	char* tail;
};