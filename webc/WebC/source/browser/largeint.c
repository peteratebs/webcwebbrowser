/*
  largeint.c

  Large (64 bits) integer arithmetics library

  Written by Anders Norlander 

  This file is part of a free library for the Win32 API.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*/

#define __COMPILING_LARGEINT

#include "windows.h"
#include "largeint.h"

#define _toi(x) x
#define _toui(x) x

LARGE_INTEGER WINAPI
LargeIntegerAdd (LARGE_INTEGER i1, LARGE_INTEGER i2)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = i1.QuadPart + i2.QuadPart;

	return retVal;
}

LARGE_INTEGER WINAPI
LargeIntegerSubtract (LARGE_INTEGER i1, LARGE_INTEGER i2)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart =  i1.QuadPart - i2.QuadPart;
	return retVal;
}

LARGE_INTEGER WINAPI
LargeIntegerArithmeticShift (LARGE_INTEGER i, CCHAR n)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = i.QuadPart >> n;
	return retVal;
}

LARGE_INTEGER WINAPI
LargeIntegerShiftLeft (LARGE_INTEGER i, CCHAR n)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = i.QuadPart << n;
	return retVal;
}

LARGE_INTEGER WINAPI
LargeIntegerShiftRight (LARGE_INTEGER i, CCHAR n)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = i.QuadPart >> n;
	return retVal;
}

LARGE_INTEGER WINAPI
LargeIntegerNegate (LARGE_INTEGER i)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = -i.QuadPart;
	return retVal;
}

LARGE_INTEGER WINAPI
ConvertLongToLargeInteger (LONG l)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart =  l;
	return retVal;
}

LARGE_INTEGER WINAPI
ConvertUlongToLargeInteger (ULONG ul)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = _toi(_toui(ul));
	return retVal;
}

LARGE_INTEGER WINAPI
EnlargedIntegerMultiply (LONG l1, LONG l2)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart =  _toi(l1) * _toi(l2);
	return retVal;
}

LARGE_INTEGER WINAPI
EnlargedUnsignedMultiply (ULONG ul1, ULONG ul2)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = _toi(_toui(ul1) * _toui(ul2));
	return retVal;
}

LARGE_INTEGER WINAPI
ExtendedIntegerMultiply (LARGE_INTEGER i, LONG l)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = i.QuadPart * _toi(l);
	return retVal;
}

LARGE_INTEGER WINAPI
LargeIntegerMultiply (LARGE_INTEGER i1, LARGE_INTEGER i2)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = i1.QuadPart * i2.QuadPart;
	return retVal;
}

LARGE_INTEGER WINAPI LargeIntegerDivide (LARGE_INTEGER i1, LARGE_INTEGER i2, LARGE_INTEGER *remainder)
{
	LARGE_INTEGER retVal;
	if (remainder)
		remainder->QuadPart = i1.QuadPart % i2.QuadPart;
	retVal.QuadPart = i1.QuadPart / i2.QuadPart;
	return retVal;
}

ULONG WINAPI
EnlargedUnsignedDivide (ULARGE_INTEGER i1, ULONG i2, PULONG remainder)
{
  if (remainder)
    *remainder = (ULONG) (i1.QuadPart % _toi(i2));
  return ((ULONG) (i1.QuadPart / _toi(i2)));
}
LARGE_INTEGER WINAPI
ExtendedLargeIntegerDivide (LARGE_INTEGER i1, ULONG i2, PULONG remainder)
{
	LARGE_INTEGER retVal;
	if (remainder)
		*remainder = (ULONG) (i1.QuadPart % _toi(i2));
	retVal.QuadPart = i1.QuadPart / _toi(i2);
	return retVal;
}

/* FIXME: what is this function supposed to do? */
LARGE_INTEGER WINAPI ExtendedMagicDivide (LARGE_INTEGER i1, LARGE_INTEGER i2, CCHAR n)
{
	LARGE_INTEGER retVal;
	retVal.QuadPart = 0;
	return retVal;
}
