/*
 * new.h
 *
 *  Created on: 16 Oct 2012
 *      Author: jack
 */

#ifndef _CUSTOM_NEW_H_
#define _CUSTOM_NEW_H_

#include <stdlib.h>

/* It appears that version 4.7.0 of avr-gcc defined
 * new[] and delete[] so if we detect an earlier version
 * then we must define new[] and delete[] ourselves.
 * This "#define GCC_VERSION" trick from clawson:
 * http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=59453&start=all&postdays=0&postorder=asc#1002400 */

/*#define GCC_VERSION (__GNUC__ * 10000 \
 *                     + __GNUC_MINOR__ * 100 \
 *                     + __GNUC_PATCHLEVEL__)
 *
 *#if GCC_VERSION < 40700
*/

/* Arduino 1.0.0 introduced the files
 * hardware/arduino/cores/arduino/new.cpp and new.h
 * the latter of which defines NEW_H.
 * So if NEW_H is not defined then we should
 * define new and delete. */

#ifndef NEW_H
#define NEW_H
#define DEFINE_NEW_AND_DELETE
void * operator new(size_t size);
void operator delete(void * ptr);
#endif /* NEW_H */

//#endif /* GCC_VERSION */

// Arduino 1.0.1 doesn't include new[] or delete[]
#define DEFINE_NEW_ARRAY_AND_DELETE_ARRAY
void * operator new[](size_t size);
void operator delete[](void * ptr);
extern "C" void __cxa_pure_virtual(void); 

#endif /* _CUSTOM_NEW_H_ */
