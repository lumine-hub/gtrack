#include <stdlib.h>

/*
void *gtrack_alloc(unsigned int numElements, unsigned int sizeInBytes)
{

	return MemoryP_ctrlAlloc(numElements*sizeInBytes, 0);
}
void gtrack_free(void *pFree, unsigned int sizeInBytes)
{

	MemoryP_ctrlFree(pFree,sizeInBytes);
}
*/

void* gtrack_alloc(unsigned int numElements, unsigned int sizeInBytes)
{
	return malloc(numElements * sizeInBytes);
}

void gtrack_free(void* pFree, unsigned int sizeInBytes)
{
	free(pFree);
}



