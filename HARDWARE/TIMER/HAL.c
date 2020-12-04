#include "HAL.h"
#include "stdio.h"
#include "stdlib.h"

//#define HAL_Free(ptr)  free(ptr)
//#define HAL_Malloc(ptr) malloc(ptr)	



void HAL_Free(void *ptr)
{
		free(ptr);
}
void *HAL_Malloc(uint32_t size)
{
		return malloc(size);
}




