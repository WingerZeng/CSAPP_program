#pragma once
#include <stdint.h>

//ĞéÄâ¶Ñ»·¾³
class VirtualHeap {
public:
	VirtualHeap(size_t MaxSize = (size_t(1) << 31));

	void* sbrk(int64_t offset);
private:

	char* head;
	char* brk;
	char* tail;
};