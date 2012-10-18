/*
 * new.cpp
 *
 *  Created on: 16 Oct 2012
 *      Author: jack
 */

#include <stdlib.h>
#include "new.h"

#ifdef DEFINE_NEW_AND_DELETE // defined in new.h if required
#warning "Defining new and delete"
void * operator new(size_t size)
{
  return malloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}
#endif // DEFINE_NEW_AND_DELETE


//#ifdef DEFINE_NEW_ARRAY_AND_DELETE_ARRAY // defined in new.h if required
#warning "Defining new[] and delete[]"
void * operator new[](size_t size)
{
    return malloc(size);
}

void operator delete[](void * ptr)
{
    free(ptr);
}
//#endif // DEFINE_NEW_ARRAY_AND_DELETE_ARRAY
