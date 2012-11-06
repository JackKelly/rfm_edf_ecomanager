/*
 * new.cpp
 *
 *  Created on: 16 Oct 2012
 *      Author: jack
 */

#include <Arduino.h>
#include "new_fix.h"

//#ifdef DEFINE_NEW_AND_DELETE // defined in new.h if required

void * operator new(size_t size)
{
  return malloc(size);
}


void operator delete(void * ptr)
{
  free(ptr);
}
//#endif // DEFINE_NEW_AND_DELETE

//#warning "DEFINING NEW[] AND DELETE[]"

void * operator new[](size_t size)
{
    return malloc(size);
}


void operator delete[](void * ptr)
{
    free(ptr);
}

void __cxa_pure_virtual(void) {}; 
